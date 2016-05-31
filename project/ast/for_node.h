// $Id: for_node.h,v 1.2 2016/04/13 11:12:31 ist178865 Exp $ -*- c++ -*-
#ifndef __CDK_FORNODE_H__
#define __CDK_FORNODE_H__

#include <cdk/ast/expression_node.h>

namespace zu {

  /**
   * Class for describing for-cycle nodes.
   */
  class for_node: public cdk::basic_node {
    cdk::sequence_node *_init;
    cdk::sequence_node *_condition;
    cdk::sequence_node *_incr;
    cdk::basic_node *_block;

  public:
    inline for_node(int lineno, cdk::sequence_node *init,
		cdk::sequence_node *condition, cdk::sequence_node *incr, cdk::basic_node *block) :
        basic_node(lineno), _init(init), _condition(condition), _incr(incr), _block(block) {
    }

  public:

    inline cdk::sequence_node *init() {
      return _init;
    }

    inline cdk::sequence_node *condition() {
      return _condition;
    }

    inline cdk::sequence_node *incr() {
      return _incr;
    }
    inline cdk::basic_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_for_node(this, level);
    }

  };

} // zu

#endif
