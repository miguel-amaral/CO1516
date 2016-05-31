// $Id: break_node.h,v 1.2 2016/03/17 18:52:34 ist179620 Exp $ -*- c++ -*-
#ifndef __ZU_BREAKNODE_H__
#define __ZU_BREAKNODE_H__

#include <cdk/ast/basic_node.h>

namespace zu {


  class break_node: public cdk::basic_node {
  public:
    /**
     * Simple constructor.
     *
     * @param lineno the source code line number corresponding to
     * the node
     */
    inline break_node(int lineno) :
        basic_node(lineno) {
    }

    /**
     * @param sp semantic processor visitor
     * @param level syntactic tree level
     */
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_break_node(this, level);
    }

  };

} // Zu

#endif
