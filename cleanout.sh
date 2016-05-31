#!/bin/bash
# Isto foi feito para ajudar o pessoal a correr testes de compiladores automaticamente
# Qualquer coisa agradeçam ao Miguel Ventura, ou então deixem lá isso

# Mudar o caminho da variavel DIR para o directorio onde estao os testes
DIR="tests-zu-daily-201604071153/"

( cd project ; make clean )

#--------------------------------------------------------------------------------------#
echo
echo -e ">> \e[33;1mCleaning\e[0m"
echo 

#--------------------------------------------------------------------------------------#
cd $DIR
rm *.out *.output *.asm *.xml *.o *exec


#--------------------------------------------------------------------------------------#
echo -e ">> \e[33;1mJob Done\e[0m"
echo 
