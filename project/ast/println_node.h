// $Id: println_node.h,v 1.1 2016/03/17 00:32:15 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_PRINTLNNODE_H__
#define __ZU_PRINTLNNODE_H__

#include <cdk/ast/expression_node.h>

namespace zu {

  /**
   * Class for describing print nodes.
   */
  class println_node: public cdk::basic_node {
    cdk::expression_node *_argument;

  public:
    inline println_node(int lineno, cdk::expression_node *argument) :
        cdk::basic_node(lineno), _argument(argument) {
    }

  public:
    inline cdk::expression_node *argument() {
      return _argument;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_println_node(this, level);
    }

  };

} // zu

#endif
