// $Id: enter_counter.h,v 1.2 2016/05/18 23:22:11 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_SEMANTICS_ENTER_COUNTER_H__
#define __ZU_SEMANTICS_ENTER_COUNTER_H__

#include <string>
#include <iostream>
#include "targets/basic_ast_visitor.h"

namespace zu {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class enter_counter: public basic_ast_visitor {

  private:
	  int _counter;

  public:
	  enter_counter(std::shared_ptr<cdk::compiler> compiler) : basic_ast_visitor(compiler), _counter(0){}

  public:
    ~enter_counter() {
    }
  public:
	int getCounter(){
		return _counter;
	}

  public:
    void do_sequence_node(cdk::sequence_node * const node, int lvl);
	void do_function_def_node(zu::function_def_node * const node, int lvl);
	void do_var_dec_node(zu::var_dec_node * const node, int lvl);
	void do_var_def_node(zu::var_def_node * const node, int lvl);
	void do_block_node(zu::block_node * const node, int lvl);

  public:
    void do_integer_node(cdk::integer_node * const node, int lvl){}
    void do_string_node(cdk::string_node * const node, int lvl){}
    void do_double_node(cdk::double_node * const node, int lvl){}

  public:
    void do_neg_node(cdk::neg_node * const node, int lvl){}

  public:
    void do_add_node(cdk::add_node * const node, int lvl){}
    void do_sub_node(cdk::sub_node * const node, int lvl){}
    void do_mul_node(cdk::mul_node * const node, int lvl){}
    void do_div_node(cdk::div_node * const node, int lvl){}
    void do_mod_node(cdk::mod_node * const node, int lvl){}
    void do_lt_node(cdk::lt_node * const node, int lvl){}
    void do_le_node(cdk::le_node * const node, int lvl){}
    void do_ge_node(cdk::ge_node * const node, int lvl){}
    void do_gt_node(cdk::gt_node * const node, int lvl){}
    void do_ne_node(cdk::ne_node * const node, int lvl){}
    void do_eq_node(cdk::eq_node * const node, int lvl){}

  public:
    void do_lvalue_node(zu::lvalue_node * const node, int lvl){}
    void do_rvalue_node(zu::rvalue_node * const node, int lvl){}

  public:
    //void do_program_node(zu::program_node * const node, int lvl){}
    //void do_function_node(zu::function_node * const node, int lvl){}
    void do_evaluation_node(zu::evaluation_node * const node, int lvl){}
    void do_print_node(zu::print_node * const node, int lvl){}
    void do_read_node(zu::read_node * const node, int lvl){}
    void do_assignment_node(zu::assignment_node * const node, int lvl){}

  public:

    //void do_while_node(zu::while_node * const node, int lvl){}
    void do_if_node(zu::if_node * const node, int lvl);
    void do_if_else_node(zu::if_else_node * const node, int lvl);

  public:
    void do_and_node(zu::and_node * const node, int lvl){}
    void do_break_node(zu::break_node * const node, int lvl){}
    void do_continue_node(zu::continue_node * const node, int lvl){}
    void do_for_node(zu::for_node * const node, int lvl);
    void do_function_call_node(zu::function_call_node * const node, int lvl){}
    void do_identity_node(zu::identity_node * const node, int lvl){}
    void do_index_node(zu::index_node * const node, int lvl){}
    void do_mem_alloc_node(zu::mem_alloc_node * const node, int lvl){}
    void do_not_node(zu::not_node * const node, int lvl){}
    void do_or_node(zu::or_node * const node, int lvl){}
    void do_println_node(zu::println_node * const node, int lvl){}
    void do_return_node(zu::return_node * const node, int lvl){}
    void do_identifier_node(zu::identifier_node * const node, int lvl){}
    void do_addressOf_node(zu::addressOf_node * const node, int lvl){}
	void do_function_dec_node(zu::function_dec_node * const node, int lvl){}


    void do_new_node(zu::new_node * const node, int lvl);
  };

} // zu

#endif
