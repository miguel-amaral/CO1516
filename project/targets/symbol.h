// $Id: symbol.h,v 1.8 2016/05/20 13:32:33 ist178865 Exp $ -*- c++ -*-
#ifndef __ZU_SEMANTICS_SYMBOL_H__
#define __ZU_SEMANTICS_SYMBOL_H__

#include <string>
#include <cdk/basic_type.h>

namespace zu {

    class symbol {
      basic_type *_type;
      std::string _name;

      //if function
      bool _isFunction;
      bool _functionDefined = false;//Se for true, a funcao ja foi definida (cc apenas foi declarada).

      std::vector<basic_type*> _args;
      cdk::expression_node *_retLiteral;

      //if var
      int _offset = -1;

      bool _isGlobal;
      bool _import;


    public:
      inline symbol(basic_type *type, const std::string &name, bool global, bool import):
          _type(type), _name(name), _isFunction(false), _isGlobal(global), _import(import){
      }

      inline symbol(basic_type *type, const std::string &name, std::vector<basic_type*> args,cdk::expression_node *retLiteral, bool isGlobal , bool import ) :
          _type(type), _name(name), _isFunction(true), _args(args), _retLiteral(retLiteral), _isGlobal(isGlobal), _import(import){
      }

      virtual ~symbol() {
        delete _type;
      }

      inline basic_type *type() const {
        return _type;
      }
      inline const std::string &name() const {
        return _name;
      }

      inline bool import() const {
        return _import;
      }

      inline bool isFunction() const {
        return _isFunction;
      }

      //Function Defined
      inline bool functionDefined(bool functionDefined) {
          return _functionDefined = functionDefined;
      }

      inline bool functionDefined() const {
          return _functionDefined;
      }

      //Args
      inline  std::vector<basic_type*> args() const {
        return _args;
      }

      inline  cdk::expression_node *retLiteral() const {
        return _retLiteral;
      }

      inline  int offset() const {
        return _offset;
      }
      inline  int offset(int offset) {
          return _offset = offset;
      }

      inline bool isGlobal() const {
        return _isGlobal;
      }
      inline bool isGlobal(bool isGlobal) {
        return _isGlobal = isGlobal;
      }
    };

} // zu

#endif
