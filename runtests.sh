#!/bin/bash
# Isto foi feito para ajudar o pessoal a correr testes de compiladores automaticamente
# Qualquer coisa agradeçam ao Miguel Ventura, ou então deixem lá isso

# Mudar o caminho da variavel DIR para o directorio onde estao os testes

#dailies
DIR="tests-zu-daily-201604071153/"
EXPECTED="tests-zu-daily-201604071153/expected/"

#simple
#DIR="tests-zu-daily-201604071153/auto-tests/"
# EXPECTED="tests-zu-daily-201604071153/expected/"


#--------------------------------------------------------------------------------------#

COUNTER=0
COMPLETE_TESTS=0
FAILED=0
COMPILERFAIL=()
YASMFAIL=()
LDFAIL=()
DIFFFAIL=()
flagDif=0

#--------------------------------------------------------------------------------------#

if [ $1 ]
then
	flagDif=1
fi

for file in ${DIR}*.zu
do
  let COUNTER=COUNTER+1
  # detecta numero do teste
  FILENAME=$(basename $file)
  NUM=`echo "$FILENAME" | cut -d'-' -f3`

  # Se foi fornecido um intervalo
  if [[ -n "$1" && -n "$2" ]]; then
    # limite inferior
    if [ "$NUM" -lt "$1" ]; then
      continue
    fi

    #limite superior
    if [ "$NUM" -gt "$2" ]; then
      break
    fi
  fi

  # apenas 1 argumento, numero de testes a correr
  if [[ -n "$1" && -z "$2" ]]; then
    if [ "$COUNTER" -gt "$1" ]; then
        break
    fi
  fi


  # comando a ser executado
  NAME=`echo "$file" | cut -d'.' -f1`
  N=`echo "$FILENAME" | cut -d'.' -f1`

  if [[ "$COUNTER" -eq "1" ]]; then
    echo "-----------------------------------------------------"
  fi

  # executar o compilador
  printf "%3s :%13s " "$COUNTER" "$N"
  { ./project/zu "$file"; } #>& "$NAME.output";
  if [[ "$?" -eq "0" ]]; then
    echo -e -n "C:\e[32m1\e[0m "
  else
    echo #echo -e    ".. C:\e[31mF\e[0m ";
    COMPILERFAIL+=("$N")
    let FAILED=FAILED+1
	continue
  fi

  # produzir o ficheiro binario
  cd $DIR;
  { yasm -felf32 "$N.asm"; } #>& /dev/null
  if [[ "$?" -eq "0" ]]; then
    echo -e -n  "Y:\e[32m2\e[0m "
  else
    echo #echo -e     "Y:\e[31mF\e[0m ";
	let FAILEDTESTS=FAILEDTESTS+1
      YASMFAIL+=("$N")
	  continue
  fi

  # gerar o executavel linkando a biblioteca RTS
  { ld -m elf_i386 -o "$N"exec "$N.o" -lrts; } #>& /dev/null
  if [[ "$?" -eq "0" ]]; then
    echo -e -n "L:\e[32m3\e[0m "
  else
    echo #echo -e    "L:\e[31mF\e[0m ";
	let FAILEDTESTS=FAILEDTESTS+1
	continue

  fi

  #Executar o programa
  { ./"$N"exec > "$N.out"; } >& /dev/null
  if [[ "$?" -eq "0" ]]; then
	  echo -e -n "R:\e[32m4\e[0m "
#  else
#	  echo #echo -e    "R:\e[31mF\e[0m ";
#	  let FAILEDTESTS=FAILEDTESTS+1
#	  continue
  fi
  { cd ../; } >& /dev/null

  #echo
  #echo
  #echo
  DIFF=$(diff -u -w -E -B "$NAME.out" "$EXPECTED$N.out")
  if [ "$DIFF" != "" ];
  then
      let FAILEDTESTS=FAILEDTESTS+1
      echo #echo -e "D:\e[31mFAILED\e[0m"

      DIFFFAIL+=("$N")
      if [ $flagDif -eq 1 ]
      then
	  echo -n -e "$DIFF\n"
      fi
  else
      echo -e "D:\e[32m5\e[0m"
	  let COMPLETE_TESTS=COMPLETE_TESTS+1
  fi

done
#--------------------------------------------------------------------------------------#

read 

echo
echo
echo $(($COUNTER)) " testes efectuados, falhas abaixo:"
echo
echo "COMPILADOR ZU:"
for i in "${COMPILERFAIL[@]}"
do
   echo "    !falha :" $i
done

echo "YASM:"
for i in "${YASMFAIL[@]}"
do
   echo "    !falha :" $i
done

echo "LD:"
for i in "${LDFAIL[@]}"
do
   echo "    !falha :" $i
done

echo "DIFF:"
for i in "${DIFFFAIL[@]}"
do
   echo "    !falha :" $i
done

echo
echo "At least OK:" $(($COUNTER - $FAILED)) "/" $(($COUNTER))
echo "Passam     :" $COMPLETE_TESTS "/" $(($COUNTER))
echo
echo "Nota:Se precisares podes ver os .output gerados para ver o que aconteceu durante o ./zu file.zu"
echo "Está tudo despachado, até à próxima!"
echo
