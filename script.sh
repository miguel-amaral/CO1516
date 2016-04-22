################################################################################
################################################################################
############################ Reading Nodes from file ###########################
i=0
nodesArray=()
while IFS=$'\r\n' read -r -a name
do
	nodesArray[i]=$name
	i=$((i + 1))
	echo $i
	echo $name
done < real_nodes.txt
############################ Reading Nodes from file ###########################
################################################################################
################################################################################


################################################################################
################################################################################
############################ Writing in header Files ###########################
for file in postfix_writer.h type_checker.h xml_writer.h
do
	for node in ${nodesArray[@]}
	do
 		echo "    void do_$node(zu::$node * const node, int lvl);" >> $file
	done
	echo -e "\e[31;1m [[[[[[[[[[]]]]]]]]] \e[33;1mPLEASE REMOVE LAST }}} \e[31;1m [[[[[[[[[[]]]]]]]]]"
	echo -e "$file"
	sleep 2
	vim $file
done
for file in basic_ast_visitor.h
do
	for node in ${nodesArray[@]}
	do
 		echo "  virtual void do_$node(zu::$node * const node, int lvl) = 0;" >> $file
	done
	echo -e "\e[31;1m [[[[[[[[[[]]]]]]]]] \e[33;1mPLEASE REMOVE LAST }}} \e[31;1m [[[[[[[[[[]]]]]]]]]"
	echo -e "$file"
	sleep 2
	vim $file
done


############################ Writing in header Files ###########################
################################################################################
################################################################################


################################################################################
################################################################################
############################ Writing in source Files ###########################
for file in postfix_writer type_checker xml_writer
do
	for node in ${nodesArray[@]}
	do
		echo  "
//---------------------------------------------------------------------------

void zu::$file::do_$node(zu::$node * const node, int lvl){
  //TODO:FIXME:XXX
}" >> $file.cpp
	done
	echo -e "$file.cpp"
done
############################ Writing in source Files ###########################
################################################################################
################################################################################
echo -e -n "\e[32;1mWell Done, now sit back and relax, you've done a WONDERFUL job\n\t\t\t\e[33;1mKAPPINHA\n\e[0m"
