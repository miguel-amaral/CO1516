// $Id: postfix_writer.h,v 1.12 2016/05/19 17:30:20 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_SEMANTICS_PF_WRITER_H__
#define __ZU_SEMANTICS_PF_WRITER_H__

#include <string>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace zu {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<zu::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;
    //bool _declaring;
    std::string _id_being_declared ;

    std::vector<int> _cycleBreakVec;
    std::vector<int> _cycleContVec;
    std::string _currentFunctionName;
    bool _globalSTR;


  private:
    void addExternDeclaration(const std::string id);
    void addInvocation(const std::string id);
    void addDefinition(const std::string id);
    void processExterns();
    bool find(std::vector<std::string> vector, std::string id);

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<zu::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0) {

    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
    void do_sequence_node(cdk::sequence_node * const node, int lvl);

  public:
    void do_integer_node(cdk::integer_node * const node, int lvl);
    void do_string_node(cdk::string_node * const node, int lvl);
    void do_double_node(cdk::double_node * const node, int lvl);

  public:
    void do_neg_node(cdk::neg_node * const node, int lvl);

  public:
    void do_add_node(cdk::add_node * const node, int lvl);
    void do_sub_node(cdk::sub_node * const node, int lvl);
    void do_mul_node(cdk::mul_node * const node, int lvl);
    void do_div_node(cdk::div_node * const node, int lvl);
    void do_mod_node(cdk::mod_node * const node, int lvl);
    void do_lt_node(cdk::lt_node * const node, int lvl);
    void do_le_node(cdk::le_node * const node, int lvl);
    void do_ge_node(cdk::ge_node * const node, int lvl);
    void do_gt_node(cdk::gt_node * const node, int lvl);
    void do_ne_node(cdk::ne_node * const node, int lvl);
    void do_eq_node(cdk::eq_node * const node, int lvl);

  public:
    void do_lvalue_node(zu::lvalue_node * const node, int lvl);
    void do_rvalue_node(zu::rvalue_node * const node, int lvl);

  public:
    //void do_program_node(zu::program_node * const node, int lvl);
    //void do_function_node(zu::function_node * const node, int lvl);
    void do_evaluation_node(zu::evaluation_node * const node, int lvl);
    void do_print_node(zu::print_node * const node, int lvl);
    void do_read_node(zu::read_node * const node, int lvl);
    void do_assignment_node(zu::assignment_node * const node, int lvl);

  public:

    //void do_while_node(zu::while_node * const node, int lvl);
    void do_if_node(zu::if_node * const node, int lvl);
    void do_if_else_node(zu::if_else_node * const node, int lvl);

  public:
    void do_and_node(zu::and_node * const node, int lvl);
    void do_break_node(zu::break_node * const node, int lvl);
    void do_continue_node(zu::continue_node * const node, int lvl);
    void do_for_node(zu::for_node * const node, int lvl);
    void do_function_call_node(zu::function_call_node * const node, int lvl);
    void do_function_dec_node(zu::function_dec_node * const node, int lvl);
    void do_function_def_node(zu::function_def_node * const node, int lvl);
    void do_identity_node(zu::identity_node * const node, int lvl);
    void do_index_node(zu::index_node * const node, int lvl);
    void do_mem_alloc_node(zu::mem_alloc_node * const node, int lvl);
    void do_not_node(zu::not_node * const node, int lvl);
    void do_or_node(zu::or_node * const node, int lvl);
    void do_println_node(zu::println_node * const node, int lvl);
    void do_return_node(zu::return_node * const node, int lvl);
    void do_var_dec_node(zu::var_dec_node * const node, int lvl);
    void do_var_def_node(zu::var_def_node * const node, int lvl);
    void do_identifier_node(zu::identifier_node * const node, int lvl);
    void do_addressOf_node(zu::addressOf_node * const node, int lvl);
    void do_block_node(zu::block_node * const node, int lvl);
    void do_new_node(zu::new_node * const node, int lvl);
  };

} // zu

#endif
