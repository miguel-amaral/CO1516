#ifndef __ZU_INDEX_NODE_H__
#define __ZU_INDEX_NODE_H__

#include <cdk/ast/expression_node.h>

namespace zu {

  /**
   * Class for describing pointer index nodes.
   */
  class index_node: public lvalue_node {
    cdk::expression_node *_base;
    cdk::expression_node *_position;

  public:
    inline index_node(int lineno, cdk::expression_node *base, cdk::expression_node *position) :
        lvalue_node(lineno), _base(base), _position(position) {
    }

    inline cdk::expression_node *base() {
      return _base;
    }

    inline cdk::expression_node *position() {
      return _position;
    }


    void accept(basic_ast_visitor *sp, int level) {
      sp->do_index_node(this, level);
    }

  };

} // zu

#endif
