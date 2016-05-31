#include <string>
#include <sstream>
#include "targets/enter_counter.h"
#include "ast/all.h"  // all.h is automatically generated

void zu::enter_counter::do_function_def_node(zu::function_def_node * const node, int lvl){
	//Memoria para a variavel de retorno
	if(node->type()->name() == basic_type::TYPE_INT ||
	   node->type()->name() == basic_type::TYPE_POINTER ||
	   node->type()->name() == basic_type::TYPE_STRING
		)
		_counter += 4;
	else if(node->type()->name() == basic_type::TYPE_DOUBLE)
		_counter += 8;
	else if(node->type()->name() == basic_type::TYPE_VOID)
		//VOID has size 0
		;

	node->corpo()->accept(this, lvl + 2);
}

void zu::enter_counter::do_block_node(zu::block_node * const node, int lvl){
	node->declaracoes()->accept(this, lvl);
	node->instrucoes()->accept(this, lvl);
}

void zu::enter_counter::do_sequence_node(cdk::sequence_node * const node, int lvl){
	for (size_t i = 0; i < node->size(); i++)
    	node->node(i)->accept(this, lvl);
}
void zu::enter_counter::do_var_dec_node(zu::var_dec_node * const node, int lvl){
	_counter += node->type()->size();
}
void zu::enter_counter::do_var_def_node(zu::var_def_node * const node, int lvl){
	_counter += node->type()->size();
}

void zu::enter_counter::do_for_node(zu::for_node * const node, int lvl){
	node->init()->accept(this, lvl);
	node->condition()->accept(this, lvl);
	node-> incr()->accept(this, lvl);
	node->block()->accept(this, lvl);
}

void zu::enter_counter::do_if_node(zu::if_node * const node, int lvl){
	node->block()->accept(this, lvl);
}

void zu::enter_counter::do_if_else_node(zu::if_else_node * const node, int lvl){
	node->thenblock()->accept(this, lvl);
	node->elseblock()->accept(this, lvl);
}

//---------------------------------------------------------------------------

void zu::enter_counter::do_new_node(zu::new_node * const node, int lvl){
  //TODO:FIXME:XXX
}
