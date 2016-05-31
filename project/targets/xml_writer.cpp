// $Id: xml_writer.cpp,v 1.19 2016/05/20 09:09:42 ist178865 Exp $ -*- c++ -*-
#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

//---------------------------------------------------------------------------

void zu::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  if(node != nullptr){
	  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
	  for (size_t i = 0; i < node->size(); i++){
		  if(node != nullptr)
		  	node->node(i)->accept(this, lvl + 2);
	  }
	  closeTag(node, lvl);
  }
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  processSimple(node, lvl);
}

void zu::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  processSimple(node, lvl);
}
void zu::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
    processSimple(node, lvl);
}

//---------------------------------------------------------------------------

inline void zu::xml_writer::processUnaryExpression(cdk::unary_expression_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void zu::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

inline void zu::xml_writer::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void zu::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void zu::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_addressOf_node(zu::addressOf_node * const node, int lvl) {
	CHECK_TYPES(_compiler, _symtab, node);
	openTag(node, lvl);
	node->lvalue()->accept(this, lvl + 4);
	closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_rvalue_node(zu::rvalue_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  if(node->lvalue()->type()->name() == basic_type::TYPE_POINTER){
      openTag("lvalue subtype", lvl + 4);
	  processBasicType(node->lvalue()->type()->subtype(), lvl + 6);
      closeTag("lvalue subtype", lvl + 4);
  }
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_lvalue_node(zu::lvalue_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_assignment_node(zu::assignment_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  openTag("lvalue", lvl + 2);
  node->lvalue()->accept(this, lvl + 2);
  closeTag("lvalue", lvl + 2);
  //if(node->rvalue() != nullptr ){
	  openTag("rvalue", lvl + 2);
	  node->rvalue()->accept(this, lvl + 4);
	  closeTag("rvalue", lvl + 2);
  //}
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

//void zu::xml_writer::do_function_node(zu::function_node * const node, int lvl) {}
/*void zu::xml_writer::do_program_node(zu::program_node * const node, int lvl) {
  openTag(node, lvl);
  node->statements()->accept(this, lvl + 4);
  closeTag(node, lvl);
}*/

//---------------------------------------------------------------------------

void zu::xml_writer::do_evaluation_node(zu::evaluation_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void zu::xml_writer::do_print_node(zu::print_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_read_node(zu::read_node * const node, int lvl) {
	CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  //node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------
/*
void zu::xml_writer::do_while_node(zu::while_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}
*/
//---------------------------------------------------------------------------

void zu::xml_writer::do_if_node(zu::if_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void zu::xml_writer::do_if_else_node(zu::if_else_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_and_node(zu::and_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_break_node(zu::break_node * const node, int lvl){
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_continue_node(zu::continue_node * const node, int lvl){
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_for_node(zu::for_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  openTag("init", lvl + 2);
  node->init()->accept(this, lvl + 4);
  closeTag("init", lvl + 2);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("increment", lvl + 2);
  node->incr()->accept(this, lvl + 4);
  closeTag("increment", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_function_call_node(zu::function_call_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);

  openTag(node, lvl);
  openTag("name", lvl + 2);
  processString(node->name(), lvl + 4);
  closeTag("name", lvl + 2);
  if(node->argumentos() != nullptr){
      openTag("arguments", lvl + 2);
      node->argumentos()->accept(this, lvl + 4);
      closeTag("arguments", lvl + 2);
  }
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_function_dec_node(zu::function_dec_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);

  openTag("name", lvl + 2);
  processString(node->name(), lvl + 4);
  closeTag("name", lvl + 2);
  openTag("local", lvl + 2);
  processBoolean(node->local(), lvl + 4);
  closeTag("local", lvl + 2);
  openTag("import", lvl + 2);
  processBoolean(node->import(), lvl + 4);
  closeTag("import", lvl + 2);

  if(node->input() != nullptr){
      openTag("args", lvl + 2);
      node->input()->accept(this, lvl + 4);
      closeTag("args", lvl + 2);
  }
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_function_def_node(zu::function_def_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  openTag("name", lvl + 2);
  processString(node->name(), lvl + 4);
  closeTag("name", lvl + 2);
  openTag("local", lvl + 2);
  processBoolean(node->local(), lvl + 4);
  closeTag("local", lvl + 2);
  if(node->input() != nullptr){
      openTag("args", lvl + 2);
      node->input()->accept(this, lvl + 4);
      closeTag("args", lvl + 2);
  }
  openTag("literal", lvl + 2);
  if(node->literal() != nullptr)
  	node->literal()->accept(this, lvl + 4);
  else{
	  openTag("default return value", lvl + 4);closeTag("", lvl + 4);
  }
  closeTag("literal", lvl + 2);
  openTag("corpo", lvl + 2);
  node->corpo()->accept(this, lvl + 4);
  closeTag("corpo", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_identity_node(zu::identity_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_index_node(zu::index_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  openTag("base", lvl + 2);
  node->base()->accept(this, lvl + 4);
  closeTag("base", lvl + 2);
  openTag("position", lvl + 2);
  node->position()->accept(this, lvl + 4);
  closeTag("position", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_mem_alloc_node(zu::mem_alloc_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  processUnaryExpression(node, lvl);


}

//---------------------------------------------------------------------------

void zu::xml_writer::do_not_node(zu::not_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_or_node(zu::or_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);

  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_println_node(zu::println_node * const node, int lvl){
  CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_return_node(zu::return_node * const node, int lvl){
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_var_dec_node(zu::var_dec_node * const node, int lvl){
	CHECK_TYPES(_compiler, _symtab, node);

    openTag(node, lvl);
    openTag("type", lvl + 2);
    processBasicType(node->type(), lvl + 4);
	if(node->type()->name() == basic_type::TYPE_POINTER){
		openTag("subtype", lvl + 4);
	    processBasicType(node->type()->subtype(), lvl + 6);
	    closeTag("subtype", lvl + 4);
	}
    closeTag("type", lvl + 2);
    openTag("name", lvl + 2);
    processString(node->name(), lvl + 4);
    closeTag("name", lvl + 2);
    openTag("local", lvl + 2);
    processBoolean(node->local(), lvl + 4);
    closeTag("local", lvl + 2);
    openTag("import", lvl + 2);
    processBoolean(node->import(), lvl + 4);
    closeTag("import", lvl + 2);
    closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_var_def_node(zu::var_def_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);
    openTag(node, lvl);
    openTag("type", lvl + 2);
    processBasicType(node->type(), lvl + 4);
    closeTag("type", lvl + 2);
    openTag("name", lvl + 2);
    processString(node->name(), lvl + 4);
    closeTag("name", lvl + 2);
    openTag("local", lvl + 2);
    processBoolean(node->local(), lvl + 4);
    closeTag("local", lvl + 2);
    openTag("expression", lvl + 2);
    node->expression()->accept(this, lvl + 4);
    closeTag("expression", lvl + 2);
    closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_identifier_node(zu::identifier_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);
    openTag ("identifier_node", lvl + 2);
    processString(node->value(), lvl +4);
    closeTag("identifier_node", lvl + 2);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_block_node(zu::block_node * const node, int lvl){

    CHECK_TYPES(_compiler, _symtab, node);
    openTag ("do_block_node", lvl + 2);
    openTag ("declaracoes", lvl + 4);
    node->declaracoes()->accept(this,lvl+6);
    closeTag ("declaracoes", lvl + 4);
    openTag ("instrucoes", lvl + 4);
    node->instrucoes ()->accept(this,lvl+6);
    closeTag ("instrucoes", lvl + 4);
    closeTag ("do_block_node", lvl + 2);
}

//---------------------------------------------------------------------------

void zu::xml_writer::do_new_node(zu::new_node * const node, int lvl){
  //TODO:FIXME:XXX
}
