// $Id: type_checker.cpp,v 1.27 2016/05/20 13:32:33 ist178865 Exp $ -*- c++ -*-
#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated
#include <bitset>


#define ASSERT_NOT_NULL {if (node == nullptr) return;}

#define ASSERT_UNSPEC \
{ if (node->type() != nullptr && \
    node->type()->name() != basic_type::TYPE_UNSPEC) return; }

//---------------------------------------------------------------------------

void zu::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) {

    ASSERT_UNSPEC;
    node->type(new basic_type(4, basic_type::TYPE_INT));
}

void zu::type_checker::do_string_node(cdk::string_node * const node, int lvl) {

    ASSERT_UNSPEC;
    node->type(new basic_type(4, basic_type::TYPE_STRING));
}

void zu::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}

//---------------------------------------------------------------------------

inline void zu::type_checker::processUnaryExpression(cdk::unary_expression_node * const node, int lvl) {

    ASSERT_UNSPEC;

    node->argument()->accept(this, lvl + 2);

    basic_type::type  argType = node->argument()->type()->name();

    if (argType != basic_type::TYPE_INT && argType != basic_type::TYPE_DOUBLE)
        throw "wrong type in argument of unary expression";

    node->type(node->argument()->type());
}

void zu::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) {
    processUnaryExpression(node, lvl);
}
void zu::type_checker::do_identity_node(zu::identity_node * const node, int lvl){
    processUnaryExpression(node, lvl);
}
void zu::type_checker::do_mem_alloc_node(zu::mem_alloc_node * const node, int lvl){

    if(node->type() == nullptr){
        //First Visit
        node->type(new basic_type(0, basic_type::TYPE_UNSPEC));
        return;
    }

    if(node->type()->name() != basic_type::TYPE_POINTER){
        throw "mem node only works with pointers";

    }else{
        if(node->type()->subtype() == nullptr){
            throw "error : pointer of nothing : allock node";
        }
        node->argument()->accept(this,lvl);
        if(node->argument()->type()->name() != basic_type::TYPE_INT){
            throw "Only int number argument for allocking ";
        }
    }

}

//---------------------------------------------------------------------------

inline void zu::type_checker::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {

    ASSERT_UNSPEC;

    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    basic_type::type  leftType = node->left()->type()->name();
    basic_type::type rightType = node->right()->type()->name();

    if(leftType != basic_type::TYPE_DOUBLE && leftType != basic_type::TYPE_INT){
        throw "Incoerent Left type in binary";
    }else if(rightType != basic_type::TYPE_DOUBLE && rightType != basic_type::TYPE_INT){
        throw "Incoerent Right type in binary";
    }else{
        if(leftType == basic_type::TYPE_DOUBLE || rightType == basic_type::TYPE_DOUBLE){
            node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
        }else{
            node->type(new basic_type(4, basic_type::TYPE_INT));
        }
    }
}

inline void zu::type_checker::processIntExpression(cdk::binary_expression_node * const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    basic_type::type  leftType = node->left()->type()->name();
    basic_type::type rightType = node->right()->type()->name();

    if(leftType == basic_type::TYPE_INT && rightType == basic_type::TYPE_INT) {
        node->type(new basic_type(4, basic_type::TYPE_INT));
    }else{
        throw "Incompatible types in binary";
    }
}

void zu::type_checker::do_add_node(cdk::add_node * const node, int lvl) {
	ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    basic_type::type  leftType = node->left()->type()->name();
    basic_type::type rightType = node->right()->type()->name();

    if(leftType == basic_type::TYPE_POINTER && rightType == basic_type::TYPE_INT){
		node->type(new basic_type(4, basic_type::TYPE_POINTER));

		node->type()->_subtype = new basic_type(node->left()->type()->subtype()->size(), node->left()->type()->subtype()->name());

	}else if(leftType == basic_type::TYPE_INT && rightType == basic_type::TYPE_POINTER){
        node->type(new basic_type(4, basic_type::TYPE_POINTER));

		node->type()->_subtype = new basic_type(node->right()->type()->subtype()->size(), node->right()->type()->subtype()->name());

    }else if(leftType == basic_type::TYPE_INT && rightType == basic_type::TYPE_INT) {
        node->type(new basic_type(4, basic_type::TYPE_INT));
    }else if( (leftType == basic_type::TYPE_DOUBLE && rightType == basic_type::TYPE_DOUBLE) ||
              (leftType == basic_type::TYPE_INT && rightType == basic_type::TYPE_DOUBLE)    ||
              (leftType == basic_type::TYPE_DOUBLE && rightType == basic_type::TYPE_INT)) {

        node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
    }else{
        throw "Incompatible types in binary";
    }
}
void zu::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) {
	ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    basic_type::type  leftType = node->left()->type()->name();
    basic_type::type rightType = node->right()->type()->name();

    if(	(leftType == basic_type::TYPE_POINTER && rightType == basic_type::TYPE_INT)		||
        (leftType == basic_type::TYPE_INT && rightType == basic_type::TYPE_POINTER ) 	||
		(leftType == basic_type::TYPE_POINTER && rightType == basic_type::TYPE_POINTER )){
        	node->type(new basic_type(4, basic_type::TYPE_POINTER));

    }else if(leftType == basic_type::TYPE_INT && rightType == basic_type::TYPE_INT) {
        node->type(new basic_type(4, basic_type::TYPE_INT));

    }else if( (leftType == basic_type::TYPE_DOUBLE && rightType == basic_type::TYPE_DOUBLE) ||
              (leftType == basic_type::TYPE_INT && rightType == basic_type::TYPE_DOUBLE)    ||
              (leftType == basic_type::TYPE_DOUBLE && rightType == basic_type::TYPE_INT)) {

        node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
    }else{
        throw "Incompatible types in binary";
    }
}
void zu::type_checker::do_mul_node(cdk::mul_node * const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void zu::type_checker::do_div_node(cdk::div_node * const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void zu::type_checker::do_mod_node(cdk::mod_node * const node, int lvl) {
    processIntExpression(node, lvl);
}

inline void zu::type_checker::processComparativeNode(cdk::binary_expression_node * const node, int lvl){
    processBinaryExpression(node, lvl);
    delete node->type();
    node->type(new basic_type(4, basic_type::TYPE_INT));
}

void zu::type_checker::do_lt_node(cdk::lt_node * const node, int lvl) {
    processComparativeNode(node, lvl);
}
void zu::type_checker::do_le_node(cdk::le_node * const node, int lvl) {
    processComparativeNode(node, lvl);
}
void zu::type_checker::do_ge_node(cdk::ge_node * const node, int lvl) {
    processComparativeNode(node, lvl);
}
void zu::type_checker::do_gt_node(cdk::gt_node * const node, int lvl) {
    processComparativeNode(node, lvl);
}

inline void zu::type_checker::processEqNode(cdk::binary_expression_node * const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    basic_type::type  leftType = node->left()->type()->name();
    basic_type::type rightType = node->right()->type()->name();

    if(leftType != basic_type::TYPE_DOUBLE && leftType != basic_type::TYPE_INT && leftType != basic_type::TYPE_POINTER){
        throw "Incoerent Left type in binary";
    }else if(rightType != basic_type::TYPE_DOUBLE && rightType != basic_type::TYPE_INT && rightType != basic_type::TYPE_POINTER){
        throw "Incoerent Right type in binary";
    }else if((leftType == basic_type::TYPE_POINTER && rightType != basic_type::TYPE_POINTER) || (leftType != basic_type::TYPE_POINTER && rightType == basic_type::TYPE_POINTER)){
        throw "Can only compare pointers with pointers";
    }else{
        node->type(new basic_type(4, basic_type::TYPE_INT));
    }
}

void zu::type_checker::do_ne_node(cdk::ne_node * const node, int lvl) {
    processEqNode(node, lvl);
}
void zu::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) {
    processEqNode(node, lvl);
}

//---------------------------------------------------------------------------

void zu::type_checker::do_rvalue_node(zu::rvalue_node * const node, int lvl) {

    ASSERT_UNSPEC;
    node->lvalue()->accept(this, lvl);
//    node->type(node->lvalue()->type());
    node->type(new basic_type(node->lvalue()->type()->size(),node->lvalue()->type()->name()));
	if(node->type()->name() == basic_type::TYPE_POINTER){
		node->type()->_subtype = new basic_type(node->lvalue()->type()->subtype()->size(), node->lvalue()->type()->subtype()->name());
	}
}

//---------------------------------------------------------------------------

void zu::type_checker::do_lvalue_node(zu::lvalue_node * const node, int lvl) {
    std::cout << "using abstract class\n";
    throw "using abstract class";
}

//---------------------------------------------------------------------------

void zu::type_checker::do_identifier_node(zu::identifier_node * const node, int lvl){
    ASSERT_UNSPEC;


    const std::string &id = *(node->value());

    std::shared_ptr<zu::symbol> symbol = _symtab.find(id);
    if (symbol == nullptr) throw id + " undeclared";
    // hackish stuff...

//    node->type(symbol->type());
    node->type(new basic_type(symbol->type()->size(),symbol->type()->name()));
	if(node->type()->name() == basic_type::TYPE_POINTER){
		node->type()->_subtype = new basic_type(symbol->type()->subtype()->size(), symbol->type()->subtype()->name());
	}
}

//---------------------------------------------------------------------------

void zu::type_checker::do_assignment_node(zu::assignment_node * const node, int lvl) {
    ASSERT_UNSPEC;


    node->lvalue()->accept(this, lvl + 2);
    node->rvalue()->accept(this, lvl + 2);

    basic_type::type  leftType = node->lvalue()->type()->name();
    basic_type::type rightType = node->rvalue()->type()->name();

    if(rightType == basic_type::TYPE_UNSPEC){

        //Mem Alloc or Read Node
        basic_type * tipo = new basic_type(node->lvalue()->type()->size(), leftType);
        if(tipo->name() == basic_type::TYPE_POINTER){
            tipo->_subtype = new basic_type(node->lvalue()->type()->subtype()->size(), node->lvalue()->type()->subtype()->name());
        }
        node->rvalue()->type(tipo);
        node->rvalue()->accept(this,lvl);//Now it will check itself

        basic_type * mainTipo = new basic_type(node->lvalue()->type()->size(), node->lvalue()->type()->name());
        if(mainTipo->name() == basic_type::TYPE_POINTER){
            mainTipo->_subtype = new basic_type(node->lvalue()->type()->subtype()->size(), node->lvalue()->type()->subtype()->name());
        }
        node->type(mainTipo);
    }else if( (leftType == basic_type::TYPE_DOUBLE && rightType == basic_type::TYPE_INT) ||
            (leftType == basic_type::TYPE_INT && rightType == basic_type::TYPE_DOUBLE)){

        node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
    } else if(leftType != rightType){
        std::bitset<32> x(leftType);
        std::cout<<"\n type checker : assign : left" << x<< "\n";
        std::bitset<32> y(rightType);
        std::cout<<" type checker : assign : righ " << y<< "\n";
        throw "Incoerent Left and Right type in atribution";
    }else{

        if(leftType == basic_type::TYPE_DOUBLE){

            node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
        }
        if(leftType == basic_type::TYPE_INT){

            node->type(new basic_type(4, basic_type::TYPE_INT));
        }
        if(leftType == basic_type::TYPE_STRING){

            node->type(new basic_type(4, basic_type::TYPE_STRING));
        }
        if(leftType == basic_type::TYPE_POINTER){

            node->type(new basic_type(4, basic_type::TYPE_POINTER));
        }
    }
}

//---------------------------------------------------------------------------

void zu::type_checker::do_evaluation_node(zu::evaluation_node * const node, int lvl) {
    node->argument()->accept(this, lvl + 2);
//	if(node->argument()->type()->name() == basic_type::TYPE_POINTER){
//	//	node->type()->_subtype = new basic_type(node->argument()->type()->subtype()->size(), node->argument()->type()->subtype()->name());
//	}
}

void zu::type_checker::do_print_node(zu::print_node * const node, int lvl) {

    node->argument()->accept(this, lvl + 2);
    if (node->argument()->type()->name() == basic_type::TYPE_POINTER)
        throw "cannot print pointers";
}

//---------------------------------------------------------------------------

void zu::type_checker::do_println_node(zu::println_node * const node, int lvl){
    node->argument()->accept(this, lvl + 2);
    if (node->argument()->type()->name() == basic_type::TYPE_POINTER)
        throw "cannot print pointers";
}
//---------------------------------------------------------------------------

void zu::type_checker::do_read_node(zu::read_node * const node, int lvl) {
    //node->argument()->accept(this, lvl + 2);
    if(node->type() == nullptr){
        //First Visit
        node->type(new basic_type(0, basic_type::TYPE_UNSPEC));
        return;
    }
    if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE){
        throw "Read node only reads ints or doubles";
    }
}

//---------------------------------------------------------------------------
/*
void zu::type_checker::do_while_node(zu::while_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}
*/
//---------------------------------------------------------------------------

void zu::type_checker::do_if_node(zu::if_node * const node, int lvl) {

    node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void zu::type_checker::do_if_else_node(zu::if_else_node * const node, int lvl) {

    node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

//void zu::type_checker::do_function_node(zu::function_node * const node, int lvl) {}

//---------------------------------------------------------------------------

void zu::type_checker::do_and_node(zu::and_node * const node, int lvl){
    processIntExpression(node, lvl);
}
void zu::type_checker::do_not_node(zu::not_node * const node, int lvl){
    ASSERT_UNSPEC;
    node->argument()->accept(this, lvl + 2);
    basic_type::type  argType = node->argument()->type()->name();
    if (argType != basic_type::TYPE_INT)
        throw "wrong type in argument of logic not";
//    node->type(node->argument()->type());
    node->type(new basic_type(node->argument()->type()->size(),node->argument()->type()->name()));

}
void zu::type_checker::do_or_node(zu::or_node * const node, int lvl){
    processIntExpression(node, lvl);
}
//---------------------------------------------------------------------------

void zu::type_checker::do_break_node(zu::break_node * const node, int lvl){
    //Nothing to check
}

//---------------------------------------------------------------------------

void zu::type_checker::do_continue_node(zu::continue_node * const node, int lvl){
    //Nothing to check
}

//---------------------------------------------------------------------------

void zu::type_checker::do_for_node(zu::for_node * const node, int lvl){
    size_t size = node->condition()->size();
    for( size_t index = 0 ; index < size ; index++){
        node->condition()->node(index)->accept(this,lvl);
    }

    cdk::expression_node *last_expression = (cdk::expression_node *) node->condition()->node(size-1);
    if( last_expression->type()->name() != basic_type::TYPE_INT){
        throw "Invalid Condition in repeat node";
    }
}

//---------------------------------------------------------------------------

void zu::type_checker::do_function_call_node(zu::function_call_node * const node, int lvl){
    const std::string &id = *(node->name());


    std::shared_ptr<zu::symbol> symbol = _symtab.find(id);
    if ( symbol == nullptr ) { //Não existe na tabela de simbolos
        throw id + " :function not declared";
    } else if( !symbol->isFunction() ){ ////XXX:Tiago Discuss
        throw id + " :function does not exists";
    } else { //Function is declared
        //Check Args types
        std::vector<basic_type*> nodeInputArgs;

        //Registar tipos dos argumentos
        if(node->argumentos() != nullptr){
            for (size_t i = 0; i < node->argumentos()->size(); i++) { //XXX:ARGS 0->8
                zu::var_dec_node *arg = (zu::var_dec_node*) node->argumentos()->node(i);
                arg->accept(this,lvl);
                nodeInputArgs.push_back(arg->type());
            }
        }

        if(symbol->args().size() != nodeInputArgs.size()){
            std::cout << "\nsymbol: " <<  symbol->args().size() << "\nnode: " << nodeInputArgs.size() << "\n";
            throw id + " declared with different number of arguments";
        }

        for (size_t i = 0; i < symbol->args().size()  &&  i < nodeInputArgs.size(); i++)
        {
            if(symbol->args().at(i)->name() != nodeInputArgs.at(i)->name()){ //XXX:ARGS 0->8
                if(symbol->args().at(i)->name() == basic_type::TYPE_DOUBLE && nodeInputArgs.at(i)->name() == basic_type::TYPE_INT){
                    //do nothing, implicit conversion case int to double
                } else {
                    throw id + " declared with other types of arguments";
                }
            }
        }

//        node->type(symbol->type());
        node->type(new basic_type(symbol->type()->size(),symbol->type()->name()));
    }
}

//---------------------------------------------------------------------------

void zu::type_checker::do_function_dec_node(zu::function_dec_node * const node, int lvl){
    const std::string &id = *(node->name());
    std::vector<basic_type*> nodeInputArgs;

    //Registar tipos dos argumentos
    _symtab.push(); //We dont want declarations on the outer lvl (function lvl) -> args not global
    if(node->input() != nullptr){
        for (size_t i = 0; i < node->input()->size(); i++) {
            zu::var_dec_node *arg = (zu::var_dec_node*) node->input()->node(i);
            arg->accept(this,lvl);
            nodeInputArgs.push_back(arg->type());
        }
    }
    _symtab.pop();

    // put in the symbol table
    if (!_symtab.insert(id, std::make_shared<zu::symbol>(new basic_type(node->type()->size(),node->type()->name()),id,nodeInputArgs,nullptr,!node->local(),node->import()))) {     //nullptr is default return value; irrelevant at declaration
        throw "function " + id + " redeclared";
    }
}

//---------------------------------------------------------------------------

void zu::type_checker::do_function_def_node(zu::function_def_node * const node, int lvl){

    //std::cout<<"\n function def enter\n";
    const std::string &id = *(node->name());
    std::vector<basic_type*> nodeInputArgs ;

    //Build Args types
    //currentOfsset = 8;
    _symtab.push(); //We dont want declarations on the outer lvl (function lvl) -> args not global
    if(node->input() != nullptr){
        for (size_t i = 0; i < node->input()->size() ; i++) {
            zu::var_dec_node *arg = (zu::var_dec_node*) node->input()->node(i);
            arg->accept(this,lvl);
            nodeInputArgs.push_back(arg->type());
        }
    }
    _symtab.pop();

    std::shared_ptr<zu::symbol> symbol = _symtab.find_local(id);
    if (symbol != nullptr) { //Ja existe na tabela de simbolos
        if( symbol->functionDefined() ) {
            throw id + " already defined";
        } else {
            symbol->functionDefined(true);

            //Check arguments types
            if(symbol->args().size() != nodeInputArgs.size()){
                std::cout << "\nsymbol: " <<  symbol->args().size() << "\nnode: " << nodeInputArgs.size() << "\n";
                throw id + " already declared with different number of arguments" ;
            }
            for (size_t i = 0; i < symbol->args().size()  &&  i < nodeInputArgs.size(); i++)
            {
                if(symbol->args().at(i)->name() != nodeInputArgs.at(i)->name()) {
                    throw id + " already declared with other types of arguments";
                }
            }

            //Check return type
            if(symbol->type()->name() != node->type()->name()) {
                throw id + " already declared with other return type";
            }

            //Take care of return value consistency
            if(node->literal() != nullptr){
                basic_type::type returnValue = node->type()->name();
                node->literal()->accept(this,lvl);
                basic_type::type defaultReturnValue = node->literal()->type()->name();
                if (defaultReturnValue != returnValue) {
					if (returnValue == basic_type::TYPE_DOUBLE && defaultReturnValue == basic_type::TYPE_INT) {
						//implicit conversion from
					}else{
                    	throw "inconsistent return types on function " + id;
					}
                }
            } else {
                if(node->type()->name() == basic_type::TYPE_INT)
                    node->literal(new cdk::integer_node(node->lineno(),0));
                else if(node->type()->name() == basic_type::TYPE_POINTER)
                    node->literal(new cdk::integer_node(node->lineno(),0)); //Pointers are mere masked ints..
            }
        }
    } else { //Completly new symbol

        //Take care of return value consistency
        if(node->literal() != nullptr){
            basic_type::type returnValue = node->type()->name();
            node->literal()->accept(this,lvl);
            basic_type::type defaultReturnValue = node->literal()->type()->name();
			if (defaultReturnValue != returnValue) {
				if (returnValue == basic_type::TYPE_DOUBLE && defaultReturnValue == basic_type::TYPE_INT) {
					//implicit conversion from
				}else{
					throw "inconsistent return types on function " + id;
				}
			}
        } else {
            if(node->type()->name() == basic_type::TYPE_INT)
                node->literal(new cdk::integer_node(node->lineno(),0));
            else if(node->type()->name() == basic_type::TYPE_POINTER)
                node->literal(new cdk::integer_node(node->lineno(),0)); //Pointers are mere masked ints..
        }
        std::shared_ptr<zu::symbol> s = std::make_shared<zu::symbol>(node->type(),id,nodeInputArgs,node->literal(),!node->local(), false);

        s->functionDefined(true);
        if (!_symtab.insert(id, s)) { // put in the symbol table
            throw id + " redeclared : internal error : we messed something up... "; //
        }
    }
    node->corpo()->accept(this,lvl);

}

//---------------------------------------------------------------------------

void zu::type_checker::do_index_node(zu::index_node * const node, int lvl){
    ASSERT_UNSPEC;

    node->base()->accept(this, lvl);
    node->position()->accept(this, lvl);

    if(node->base()->type()->name() == basic_type::TYPE_POINTER){
        if(node->position()->type()->name() == basic_type::TYPE_INT){
            //node->type(new basic_type(4, basic_type::TYPE_POINTER));
            //node->type()->_subtype = new basic_type(node->base()->type()->subtype()->size(), node->base()->type()->subtype()->name());
            node->type(new basic_type(node->base()->type()->subtype()->size(), node->base()->type()->subtype()->name()));
        } else if (node->position()->type()->name() == basic_type::TYPE_UNSPEC) {
            //Read Node only
            basic_type * tipo = new basic_type(4,basic_type::TYPE_INT);
            node->position()->type(tipo);
            node->position()->accept(this,lvl);//Now it will check itself
            node->type(new basic_type(node->base()->type()->subtype()->size(), node->base()->type()->subtype()->name()));
        } else {
            throw "unsupported operation in index node";
        }
    } else if(node->position()->type()->name() == basic_type::TYPE_POINTER){
        if(node->base()->type()->name() == basic_type::TYPE_INT){
            //        node->type(new basic_type(4, basic_type::TYPE_POINTER));
            //		node->type()->_subtype = new basic_type(node->position()->type()->subtype()->size(), node->position()->type()->subtype()->name());
            node->type(new basic_type(node->position()->type()->subtype()->size(),  node->position()->type()->subtype()->name()));
        } else if(node->base()->type()->name() == basic_type::TYPE_UNSPEC) {
            //Read Node only
            basic_type * tipo = new basic_type(4,basic_type::TYPE_INT);
            node->base()->type(tipo);
            node->base()->accept(this,lvl);//Now it will check itself
            node->type(new basic_type(node->position()->type()->subtype()->size(),  node->position()->type()->subtype()->name()));
        }else{
            throw "Index node incompatible types";
        }
    }else{
        throw "Index node incompatible types";
    }

}

//---------------------------------------------------------------------------

void zu::type_checker::do_return_node(zu::return_node * const node, int lvl){
    //Nothing to check
}

//---------------------------------------------------------------------------

void zu::type_checker::do_var_dec_node(zu::var_dec_node * const node, int lvl){
    //ASSERT_UNSPEC; construtor defino tipo da variavel no yacc

    //Variables Import

    const std::string &id = *(node->name());

    std::shared_ptr<zu::symbol> symbol = _symtab.find_local(id);

    basic_type * tipo = new basic_type(node->type()->size(),node->type()->name());
    if(tipo->name() == basic_type::TYPE_POINTER){
        tipo->_subtype = new basic_type(node->type()->subtype()->size(), node->type()->subtype()->name());
    }

    if(symbol != nullptr){
        if(symbol->import() && !node->local()){
            if(tipo->name() != symbol->type()->name()){
                throw "incompatible redeclaration " + id;
            } else if(tipo->name() == basic_type::TYPE_POINTER){
                if(tipo->subtype()->name() != node->type()->subtype()->name()){
                    throw "incompatible redeclaration " + id;
                }
            }
        } else {
            throw "incompatible redeclaration " + id;
        }
    }

    std::shared_ptr<zu::symbol> s = std::make_shared<zu::symbol>(tipo, id, !node->local(),node->import() );
    if (!_symtab.insert(id, s)){ // put in the symbol table
        throw id + " redeclared on line " + std::to_string(node->lineno());
    }
}
//---------------------------------------------------------------------------

void zu::type_checker::do_var_def_node(zu::var_def_node * const node, int lvl){
    //ASSERT_UNSPEC; construtor defino tipo da variavel no yacc
    const std::string &id = *(node->name());
    basic_type * tipo = new basic_type(node->type()->size(),node->type()->name());
    if(tipo->name() == basic_type::TYPE_POINTER){
        tipo->_subtype = new basic_type(node->type()->subtype()->size(), node->type()->subtype()->name());
    }
    if (!_symtab.insert(id, std::make_shared<zu::symbol>(tipo, id,  !node->local(), false ))){ // put in the symbol table
        throw id + " redeclared";
    }

    node->expression()->accept(this, lvl);
    if(node->type()->name() != node->expression()->type()->name()){
        if(node->type()->name() == basic_type::TYPE_POINTER && node->expression()->type()->name() == basic_type::TYPE_INT){
            cdk::expression_node * expr = node->expression();
            cdk::integer_node * i = dynamic_cast<cdk::integer_node *> (expr);
            if(i->value() != 0){
                throw "pointer literal : only 0 allowed";
            }
        }else if(node->expression()->type()->name() == basic_type::TYPE_UNSPEC){

                //Mem Alloc or Read Node
                basic_type * tipo = new basic_type(node->type()->size(), node->type()->name());
                if(tipo->name() == basic_type::TYPE_POINTER){
                    tipo->_subtype = new basic_type(node->type()->subtype()->size(), node->type()->subtype()->name());
                }
                node->expression()->type(tipo);
                node->expression()->accept(this,lvl);//Now it will check itself

                //basic_type * mainTipo = new basic_type(node->type()->size(), node->type()->name());
                //if(mainTipo->name() == basic_type::TYPE_POINTER){
                //    mainTipo->_subtype = new basic_type(node->type()->subtype()->size(), node->type()->subtype()->name());
                //}
                //node->type(mainTipo);


        }else if(node->type()->name() == basic_type::TYPE_DOUBLE && node->expression()->type()->name() == basic_type::TYPE_INT){
            //Implicit declaration
            cdk::expression_node * expr = node->expression();
            cdk::integer_node * i = dynamic_cast<cdk::integer_node *> (expr);
            int value = i->value();
            node->expression(new cdk::double_node(i->lineno(),value));
            delete expr;
        }else{
        //     std::bitset<32> n(node->type()->name());
        //     std::bitset<32> e(node->expression()->type()->name());
        //
        //     std::cout << "\nnode: " << n << std::endl;
        //     std::cout << "expr: " << e << std::endl;
            throw "incompatiple types in definition of var " + id;
        }
    }
}

//---------------------------------------------------------------------------

void zu::type_checker::do_addressOf_node(zu::addressOf_node * const node, int lvl){
    //evrything is possible .. after yacc filter
    node->lvalue()->accept(this,lvl);
    node->type(new basic_type(4,basic_type::TYPE_POINTER));
    node->type()->_subtype = new basic_type(node->lvalue()->type()->size(),node->lvalue()->type()->name());
}

//---------------------------------------------------------------------------

void zu::type_checker::do_block_node(zu::block_node * const node, int lvl){

    _symtab.push();
    node->declaracoes()->accept(this,lvl);
    node->instrucoes ()->accept(this,lvl);
    _symtab.pop();

}

//---------------------------------------------------------------------------

void zu::type_checker::do_sequence_node(cdk::sequence_node * const node, int lvl) {
//    for (size_t i = 0; i < node->size(); i++) {
//        node->node(i)->accept(this, lvl);
//    }
}

//---------------------------------------------------------------------------

void zu::type_checker::do_new_node(zu::new_node * const node, int lvl){
  //TODO:FIXME:XXX
}
