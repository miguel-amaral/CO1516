// $Id: function_def_node.h,v 1.7 2016/05/16 21:19:23 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_FUNCTIONDEFNODE_H__
#define __ZU_FUNCTIONDEFNODE_H__

#include <cdk/ast/basic_node.h>

namespace zu {

  /**
   * Class for describing function nodes.
   */
  class function_def_node: public cdk::basic_node {

    basic_type *_type; //return
    std::string *_name;
    bool _local;
    cdk::sequence_node *_input;
	cdk::expression_node *_literal;
    zu::block_node *_corpo;


  public:
    inline function_def_node(int lineno, basic_type *type, std::string *name,
		bool local, cdk::sequence_node *input, cdk::expression_node *literal, zu::block_node *corpo) :
        	cdk::basic_node(lineno), _type(type), _name(name), _local(local),
		_input(input), _literal(literal), _corpo(corpo) {
    }

	inline function_def_node(int lineno, basic_type *type, const std::string &name,
		bool local, cdk::sequence_node *input, cdk::expression_node *literal, zu::block_node *corpo) :
        	cdk::basic_node(lineno), _type(type), _name(new std::string(name)), _local(local),
		_input(input), _literal(literal), _corpo(corpo) {
    }

	inline function_def_node(int lineno, basic_type *type, const char *name,
		bool local, cdk::sequence_node *input, cdk::expression_node *literal, zu::block_node *corpo) :
        	cdk::basic_node(lineno), _type(type), _name(new std::string(name)), _local(local),
		_input(input), _literal(literal), _corpo(corpo) {
    }

  public:
    inline basic_type *type() {
      return _type;
    }

    inline std::string *name(){
      return _name;
    }

    inline bool local(){
      return _local;
    }

    inline cdk::sequence_node *input(){
      return _input;
    }

	inline cdk::expression_node *literal(){
      return _literal;
    }

    inline cdk::expression_node *literal(cdk::expression_node *literal){
        return _literal = literal;
    }

    inline zu::block_node *corpo(){
      return _corpo;
    }


    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_def_node(this, level);
    }

  };

} // zu

#endif
