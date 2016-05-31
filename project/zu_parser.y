%{
// $Id: zu_parser.y,v 1.19 2016/05/20 09:09:42 ist178865 Exp $
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE       compiler->scanner()->lineno()
#define yylex()    compiler->scanner()->scan()
#define yyerror(s) compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%union {
  int                    i;    /* integer value */
  std::string            *s;    /* symbol name or string literal */
  cdk::basic_node        *node;    /* node pointer */
  cdk::sequence_node    *sequence;
  cdk::expression_node    *expression; /* expression nodes */
  zu::lvalue_node        *lvalue;
  basic_type            *type; /*tipo de dados*/
  double                d;
  zu::block_node        *block;
};

%token <i> tINTEGER
%token <d> tDOUBLE
%token <s> tIDENTIFIER tSTRING
%token tPRINTLN tBREAK tCONTINUE tRETURN

%nonassoc tIFX
%nonassoc ':'

//%nonassoc ','
%right '='
%left '|'
%left '&'
%nonassoc '~'
%left tEQ tNE
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY
%nonassoc '['

%type <node>      dec   instruction conditional_instruction iteration_instruction dec_var f_dec_var function funcvariable
%type <sequence> decs fdecs opt_fcallargs fargs funcvars vars opt_exprs exprs fcallargs instructions
%type <expression> expr literal
%type <lvalue> lval
%type <type> types
%type <s> bigString
%type <block> fbody

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%
file  : decs                                                                    { compiler->ast($1); }
      |                                                                         { compiler->ast(new cdk::nil_node(LINE));}
      ;

decs  : decs dec                                                                {$$ = new cdk::sequence_node(LINE, $2, $1);    }
      | dec                                                                     {$$ = new cdk::sequence_node(LINE, $1);}
      ;

dec   : dec_var ';'                                                             { $$ = $1; }
      | function                                                                { $$ = $1; }
      ;

dec_var: types tIDENTIFIER                                                      { $$ = new zu::var_dec_node(LINE, $1, $2, true, false); }
       | types tIDENTIFIER     '=' expr                                         { $$ = new zu::var_def_node(LINE, $1, $2, true, $4)   ; }
       | types tIDENTIFIER '!' '=' expr                                         { $$ = new zu::var_def_node(LINE, $1, $2, false,$5)   ; }
       | types tIDENTIFIER '!'                                                  { $$ = new zu::var_dec_node(LINE, $1, $2, false,false); }
       | types tIDENTIFIER '?'                                                  { $$ = new zu::var_dec_node(LINE, $1, $2, false,true) ; }
       ;


function: types tIDENTIFIER         '(' fargs ')'                               { $$ = new zu::function_dec_node(LINE, $1, $2, true,  false, $4);      }
        | types tIDENTIFIER '!'    '(' fargs ')'                                { $$ = new zu::function_dec_node(LINE, $1, $2, false, false, $5);      }
        | types tIDENTIFIER '?'    '(' fargs ')'                                { $$ = new zu::function_dec_node(LINE, $1, $2, true,  true,  $5);      }
        | types tIDENTIFIER     '(' fargs ')' '=' literal fbody                 { $$ = new zu::function_def_node(LINE, $1, $2, true,  $4,      $7, $8); }
        | types tIDENTIFIER '!' '(' fargs ')' '=' literal fbody                 { $$ = new zu::function_def_node(LINE, $1, $2, false, $5,      $8, $9); }
        | types tIDENTIFIER     '(' fargs ')' fbody                             { $$ = new zu::function_def_node(LINE, $1, $2, true , $4, nullptr, $6); }
        | types tIDENTIFIER '!' '(' fargs ')' fbody                             { $$ = new zu::function_def_node(LINE, $1, $2, false, $5, nullptr, $7); }
        | '!' tIDENTIFIER         '(' fargs ')'                                 { $$ = new zu::function_dec_node(LINE, new basic_type(0, basic_type::TYPE_VOID), $2, true,  false, $4);        }
        | '!' tIDENTIFIER   '!'    '(' fargs ')'                                { $$ = new zu::function_dec_node(LINE, new basic_type(0, basic_type::TYPE_VOID), $2, false, false, $5);        }
        | '!' tIDENTIFIER   '?'    '(' fargs ')'                                { $$ = new zu::function_dec_node(LINE, new basic_type(0, basic_type::TYPE_VOID), $2, true,  true,  $5);        }
        | '!' tIDENTIFIER         '(' fargs ')' fbody                           { $$ = new zu::function_def_node(LINE, new basic_type(0, basic_type::TYPE_VOID), $2, true,   $4, nullptr, $6); }
        | '!' tIDENTIFIER   '!' '(' fargs ')' fbody                             { $$ = new zu::function_def_node(LINE, new basic_type(0, basic_type::TYPE_VOID), $2, false,  $5, nullptr, $7); }
        ;

fargs   :                                                                       { $$ = nullptr; }
        | funcvars                                                              { $$ = $1; }
        ;

funcvars: funcvariable                                                          { $$ = new cdk::sequence_node(LINE, $1);}
        | funcvars ',' funcvariable                                             { $$ = new cdk::sequence_node(LINE, $3, $1);}
        ;

funcvariable:    types tIDENTIFIER                                              { $$ = new zu::var_dec_node(LINE, $1, $2, true, false); }
                        ;

types   : '#'                                                                   { $$ = new basic_type(4, basic_type::TYPE_INT);         }
        | '%'                                                                   { $$ = new basic_type(8, basic_type::TYPE_DOUBLE);     }
        | '$'                                                                   { $$ = new basic_type(4, basic_type::TYPE_STRING);     }
        | '<' types '>'                                                         { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2;     }
        ;

fbody   : '{' fdecs instructions '}'                                            { $$ = new zu::block_node(LINE, $2, $3);}
        | '{' instructions '}'                                                  { $$ = new zu::block_node(LINE,  new cdk::sequence_node(LINE, new cdk::nil_node(LINE)), $2);}
        | '{' fdecs '}'                                                         { $$ = new zu::block_node(LINE, $2,  new cdk::sequence_node(LINE, new cdk::nil_node(LINE)));}
        | '{' '}'                                                               { $$ = new zu::block_node(LINE,  new cdk::sequence_node(LINE, new cdk::nil_node(LINE)),  new cdk::sequence_node(LINE, new cdk::nil_node(LINE)));}
        ;

instructions : instruction                                                      { $$ = new cdk::sequence_node(LINE, $1); }
             | instructions instruction                                             { $$ = new cdk::sequence_node(LINE, $2, $1);}
             ;

fdecs   : fdecs f_dec_var ';'                                                   { $$ = new cdk::sequence_node(LINE,$2,$1);}
        | f_dec_var    ';'                                                      { $$ = new cdk::sequence_node(LINE,$1);}
        ;


f_dec_var   : types tIDENTIFIER                                                 { $$ = new zu::var_dec_node(LINE, $1, $2, true, false); }
            | types tIDENTIFIER    '=' expr                                     { $$ = new zu::var_def_node(LINE, $1, $2, true, $4)   ; }
            ;

instruction : expr ';'                                                          { $$ = new zu::evaluation_node(LINE, $1); }
            | expr '!'                                                          { $$ = new zu::print_node(LINE, $1); }
            | expr tPRINTLN                                                     { $$ = new zu::println_node(LINE, $1); }
            | tBREAK                                                            { $$ = new zu::break_node(LINE); }
            | tCONTINUE                                                         { $$ = new zu::continue_node(LINE); }
            | tRETURN                                                           { $$ = new zu::return_node(LINE); }
            | conditional_instruction                                           { $$ = $1; }
            | iteration_instruction                                             { $$ = $1; }
            | fbody                                                             { $$ = $1; }
            ;

conditional_instruction : '[' expr ']' '#' instruction %prec tIFX               { $$ = new zu::if_node(LINE, $2, $5); }
                        | '[' expr ']' '?' instruction %prec tIFX               { $$ = new zu::if_node(LINE, $2, $5); }
                        | '[' expr ']' '?' instruction ':' instruction          { $$ = new zu::if_else_node(LINE, $2, $5, $7); }
                        ;


iteration_instruction:'[' vars ';' exprs ';' opt_exprs ']' instruction          { $$ = new zu::for_node(LINE, $2, $4, $6, $8); }
                     |'[' vars ';'           ';' opt_exprs ']' instruction      { $$ = new zu::for_node(LINE, $2, new cdk::sequence_node(LINE,new zu::not_node(LINE,0)), $5, $7); }
                     |'[' opt_exprs ';'      ';' opt_exprs ']' instruction      { $$ = new zu::for_node(LINE, $2, new cdk::sequence_node(LINE,new zu::not_node(LINE,0)), $5, $7); }
                     |'[' opt_exprs ';' exprs ';' opt_exprs ']' instruction     { $$ = new zu::for_node(LINE, $2, $4, $6, $8); }
                     ;


vars    : dec_var                                                               { $$ = new cdk::sequence_node(LINE,$1);}
        | vars ','dec_var                                                       { $$ = new cdk::sequence_node(LINE,$3,$1);}
        ;

opt_exprs :                                                                     { $$ = new cdk::sequence_node(LINE,new cdk::nil_node(LINE));}
          | exprs                                                               { $$ = new cdk::sequence_node(LINE,$1);}
          ;

exprs : expr                                                                    { $$ = new cdk::sequence_node(LINE,$1);}
      | exprs ',' expr                                                          { $$ = new cdk::sequence_node(LINE,$3,$1);}
      ;

expr  : '+' expr %prec tUNARY                                                   { $$ = new zu::identity_node(LINE, $2);  }
      | '-' expr %prec tUNARY                                                   { $$ = new cdk::neg_node(LINE, $2);          }
      | '~' expr                                                                { $$ = new zu::not_node(LINE, $2);              }
      | expr '&' expr                                                           { $$ = new zu::and_node(LINE, $1,$3);      }
      | expr '|' expr                                                           { $$ = new zu::or_node(LINE, $1,$3);          }
      | lval '?'     %prec tUNARY                                               { $$ = new zu::addressOf_node(LINE, $1); }
      | expr '+' expr                                                           { $$ = new cdk::add_node(LINE, $1, $3);  }
      | expr '-' expr                                                           { $$ = new cdk::sub_node(LINE, $1, $3);  }
      | expr '*' expr                                                           { $$ = new cdk::mul_node(LINE, $1, $3);  }
      | expr '/' expr                                                           { $$ = new cdk::div_node(LINE, $1, $3);  }
      | expr '%' expr                                                           { $$ = new cdk::mod_node(LINE, $1, $3);  }
      | expr '<' expr                                                           { $$ = new cdk::lt_node(LINE, $1, $3);      }
      | expr '>' expr                                                           { $$ = new cdk::gt_node(LINE, $1, $3);      }
      | expr tGE expr                                                           { $$ = new cdk::ge_node(LINE, $1, $3);      }
      | expr tLE expr                                                           { $$ = new cdk::le_node(LINE, $1, $3);      }
      | expr tNE expr                                                           { $$ = new cdk::ne_node(LINE, $1, $3);      }
      | expr tEQ expr                                                           { $$ = new cdk::eq_node(LINE, $1, $3);      }
      | '(' expr ')'                                                            { $$ = $2; }
      | lval                                                                    { $$ = new zu::rvalue_node(LINE, $1);      }
      | lval '=' expr                                                           { $$ = new zu::assignment_node(LINE, $1, $3); }
      | literal                                                                 { $$ = $1; }
      | tIDENTIFIER '(' opt_fcallargs ')'                                       { $$ = new zu::function_call_node(LINE, $1, $3); }
      | '@'                                                                     { $$ = new zu::read_node(LINE); }
      | '[' expr ']'                                                            { $$ = new zu::mem_alloc_node(LINE, $2); }
      ;

opt_fcallargs   :                                                               { $$ = nullptr; }
                | fcallargs                                                     { $$ = $1; }
                ;

fcallargs    : expr                                                             { $$ = new cdk::sequence_node(LINE,$1); }
            | fcallargs ',' expr                                                { $$ = new cdk::sequence_node(LINE,$3,$1); }
            ;


lval: tIDENTIFIER                                                               { $$ = new zu::identifier_node(LINE,$1);}
    | expr '[' expr ']'                                                         { $$ = new zu::index_node(LINE,$1,$3);  }
    ;

literal : tINTEGER                                                              { $$ = new cdk::integer_node(LINE, $1); }
        | bigString                                                             { $$ = new cdk::string_node(LINE, $1); }
        | tDOUBLE                                                               { $$ = new cdk::double_node(LINE, $1); }
        ;

bigString : tSTRING                                                             { $$ = $1; }
          | bigString tSTRING                                                   { $$ = new std::string(*$1 + *$2); delete $1;delete $2; }
          ;

%%
