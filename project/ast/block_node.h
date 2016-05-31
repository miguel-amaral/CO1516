// $Id: block_node.h,v 1.1 2016/05/15 14:10:06 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_BLOCKNODE_H__
#define __ZU_BLOCKNODE_H__

#include <cdk/ast/basic_node.h>

namespace zu {

  /**
   * Class for describing block nodes.
   */
  class block_node: public cdk::basic_node {
    cdk::sequence_node *_declaracoes;
    cdk::sequence_node *_instrucoes;

  public:
    inline block_node(int lineno, cdk::sequence_node *declaracoes, cdk::sequence_node *instrucoes) :
        cdk::basic_node(lineno), _declaracoes(declaracoes), _instrucoes(instrucoes) {
    }

  public:
    inline cdk::sequence_node *declaracoes() {
      return _declaracoes;
    }
    
    inline cdk::sequence_node *instrucoes() {
      return _instrucoes;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_block_node(this, level);
    }

  };

} // zu

#endif
