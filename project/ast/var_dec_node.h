#ifndef __ZU_VAR_DEC_NODE_H__
#define __ZU_VAR_DEC_NODE_H__

#include <string>
#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>

namespace zu {

  /**
   * Class for describing variable declaration nodes.
   */
  class var_dec_node: public cdk::basic_node {

    basic_type *_type;
    std::string *_name;
	bool _local; //!
	bool _import;//?

  public:
    inline var_dec_node(int lineno, basic_type* type, std::string* name, bool local, bool import) :
        cdk::basic_node(lineno), _type(type), _name(name), _local(local), _import(import) {
    }

	inline var_dec_node(int lineno, basic_type* type, const std::string &name, bool local, bool import) :
        cdk::basic_node(lineno), _type(type), _name(new std::string(name)), _local(local), _import(import) {
    }

	inline var_dec_node(int lineno, basic_type* type, const char *name, bool local, bool import) :
        cdk::basic_node(lineno), _type(type), _name(new std::string(name)), _local(local), _import(import) {
    }

  public:

    inline bool local()
    {
      return _local;
    }

    inline bool import()
    {
      return _import;
    }

    inline basic_type* type()
    {
      return _type;
    }

    inline std::string* name()
    {
      return _name;
    }


    void accept(basic_ast_visitor *sp, int level) {
      sp->do_var_dec_node(this, level);
    }

  };

} // zu

#endif
