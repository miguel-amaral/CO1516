// $Id: identifier_node.h,v 1.7 2016/05/15 14:10:06 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_IDENTIFIER_H__
#define __ZU_IDENTIFIER_H__

#include "lvalue_node.h"
#include <string>

namespace zu {

  /**
   * Class for describing syntactic tree leaves for holding identifier
   * values.
   */
  class identifier_node: public lvalue_node {
	  std::string *_value;
  public:
    inline identifier_node(int lineno, std::string *s) :
        lvalue_node(lineno), _value(s) {
    }

	inline identifier_node(int lineno, const std::string &s) :
        lvalue_node(lineno), _value(new std::string(s)) {
    }

	inline identifier_node(int lineno, const char *s) :
        lvalue_node(lineno), _value(new std::string(s)) {
    }

	inline std::string *value(){
		return _value;
	}


	/**
	 * @param sp semantic processor visitor
	 * @param level syntactic tree level
	 */
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_identifier_node(this, level);
    }

  };

} // zu

#endif
