// $Id: mem_alloc_node.h,v 1.3 2016/04/14 19:36:13 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_MEMALLOCNODE_H__
#define __ZU_MEMALLOCNODE_H__

//#include <cdk/ast/data_node.h>
#include <cdk/ast/unary_expression_node.h>

namespace zu {

  /**
   * Class for describing syntactic tree leaves for holding data
   * buffers. This class does not inherit from the <tt>Simple</tt>
   * template.
   */
  class mem_alloc_node : public cdk::unary_expression_node {

  public:
    /**
     * Constructor for nodes that hold opaque data buffers.
     * Each buffer is characterized by its data and the
     * corresponding data size.
     *
     * @param lineno the source code line number corresponding to this node
     * @param data the opaque data buffer
     * @param nbytes the size (bytes) of the data buffer
     */

    inline mem_alloc_node (int lineno, cdk::expression_node *arg) :
			cdk::unary_expression_node(lineno, arg) {
    }


    /**
     * The destructor. We have defined it explicitly (even though
     * it was not needed) to emphasize that the data buffer is
     * <b>not</b> destroyed when the node itself dies.
     */
    ~mem_alloc_node() {
    }


    /**
     * @param sp semantic processor visitor
     * @param level syntactic tree level
     */
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_mem_alloc_node(this, level);
    }


  };

} // zu

#endif
