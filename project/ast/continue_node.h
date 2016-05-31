// $Id: continue_node.h,v 1.2 2016/03/17 18:52:34 ist179620 Exp $ -*- c++ -*-
#ifndef __ZU_CONTINUENODE_H__
#define __ZU_CONTINUENODE_H__

#include <cdk/ast/basic_node.h>

namespace zu {


  class continue_node: public cdk::basic_node {
  public:
    /**
     * Simple constructor.
     *
     * @param lineno the source code line number corresponding to
     * the node
     */
    inline continue_node(int lineno) :
        basic_node(lineno) {
    }

    /**
     * @param sp semantic processor visitor
     * @param level syntactic tree level
     */
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_continue_node(this, level);
    }

  };

} // zu

#endif
