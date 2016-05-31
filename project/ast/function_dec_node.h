// $Id: function_dec_node.h,v 1.4 2016/04/13 23:38:04 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_FUNCTIONDECNODE_H__
#define __ZU_FUNCTIONDECNODE_H__

#include <cdk/ast/basic_node.h>

namespace zu {

  /**
   * Class for describing function nodes.
   */
  class function_dec_node: public cdk::basic_node {

    basic_type *_type; //return
    std::string *_name;
    bool _local;  //!
    bool _import; //?
    cdk::sequence_node *_input;
    //cdk::expression_node *_literal;

  public:
    inline function_dec_node(int lineno, basic_type *type, std::string *name,
		bool local, bool import,  cdk::sequence_node *input) :
	/*cdk::expression_node *literal*/
        	cdk::basic_node(lineno), _type(type), _name(name), _local(local),
		_import(import), _input(input)/*, _literal(literal)*/ {
    }

	inline function_dec_node(int lineno, basic_type *type, const std::string &name,
		bool local, bool import,  cdk::sequence_node *input) :
	/*cdk::expression_node *literal*/
        	cdk::basic_node(lineno), _type(type), _name(new std::string(name)), _local(local),
		_import(import), _input(input)/*, _literal(literal)*/ {
    }

	inline function_dec_node(int lineno, basic_type *type, const char *name,
		bool local, bool import,  cdk::sequence_node *input) :
	/*cdk::expression_node *literal*/
        	cdk::basic_node(lineno), _type(type), _name(new std::string(name)), _local(local),
		_import(import), _input(input)/*, _literal(literal)*/ {
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

    inline bool import(){
      return _import;
    }

    inline cdk::sequence_node *input(){
      return _input;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_dec_node(this, level);
    }

  };

} // zu

#endif
