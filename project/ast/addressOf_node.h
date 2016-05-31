// $Id: addressOf_node.h,v 1.1 2016/04/13 22:52:35 ist179620 Exp $
#ifndef __ZU_ADDRESSOF_NODE_H__
#define __ZU_ADDRESSOF_NODE_H__

#include <cdk/ast/expression_node.h>
#include "ast/lvalue_node.h"

namespace zu {

  /**
   * Class for describing rvalue nodes.
   */
  class addressOf_node: public cdk::expression_node {
    lvalue_node *_lvalue;

  public:
    inline addressOf_node(int lineno, lvalue_node *lvalue) :
        cdk::expression_node(lineno), _lvalue(lvalue) {
    }

  public:
    inline cdk::expression_node *lvalue() {
      return _lvalue;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_addressOf_node(this, level);
    }

  };

} // zu

#endif
