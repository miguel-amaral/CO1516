#ifndef __ZU_new_NODE_H__
#define __ZU_new_NODE_H__

#include <cdk/ast/sequence_node.h>

namespace zu {

  /**
   * Class for describing function invocation nodes.
   */
  class new_node: public cdk::expression_node {
    std::string *_name;
    cdk::sequence_node *_argumentos;

  public:
    inline new_node(int lineno, std::string * name, cdk::sequence_node *argumentos) :
        cdk::expression_node(lineno), _name(name), _argumentos(argumentos) {
    }

	inline new_node(int lineno, const char *name, cdk::sequence_node *argumentos) :
        cdk::expression_node(lineno), _name(new std::string(name)), _argumentos(argumentos) {
    }

	inline new_node(int lineno, const std::string &name, cdk::sequence_node *argumentos) :
        cdk::expression_node(lineno), _name(new std::string(name)), _argumentos(argumentos) {
    }

  public:
    inline std::string *name() {
      return _name;
    }
    inline cdk::sequence_node *argumentos() {
      return _argumentos;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_new_node(this, level);
    }

  };

} // zu

#endif
