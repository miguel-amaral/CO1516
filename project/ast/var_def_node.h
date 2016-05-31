#ifndef __ZU_VAR_DEF_NODE_H__
#define __ZU_VAR_DEF_NODE_H__

#include <string>
#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>

namespace zu {

  /**
   * Class for describing variable define nodes.
   */
  class var_def_node: public cdk::basic_node {

    basic_type* _type;
    std::string *_name;
    bool _local;
    cdk::expression_node *_expression;

  public:
    inline var_def_node(int lineno, basic_type* type, std::string* name, bool local, cdk::expression_node *expression) :
        cdk::basic_node(lineno), _type(type), _name(name), _local(local), _expression(expression) {
    }

	inline var_def_node(int lineno, basic_type* type, const std::string &name, bool local, cdk::expression_node *expression) :
        cdk::basic_node(lineno), _type(type), _name(new std::string(name)), _local(local), _expression(expression) {
    }

	inline var_def_node(int lineno, basic_type* type, const char *name, bool local, cdk::expression_node *expression) :
        cdk::basic_node(lineno), _type(type), _name(new std::string(name)), _local(local), _expression(expression) {
    }

  public:

    inline bool local()
    {
      return _local;
    }

    inline basic_type* type()
    {
      return _type;
    }

    inline std::string* name()
    {
      return _name;
    }

    inline cdk::expression_node* expression()
    {
      return _expression;
    }

    inline cdk::expression_node* expression(cdk::expression_node* expression)
    {
        return _expression = expression;
    }


    void accept(basic_ast_visitor *sp, int level) {
      sp->do_var_def_node(this, level);
    }

  };

} // zu

#endif
