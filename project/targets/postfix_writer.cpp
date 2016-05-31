// $Id: postfix_writer.cpp,v 1.29 2016/05/20 09:09:42 ist178865 Exp $ -*- c++ -*-
#include <string>
#include <sstream>
#include <algorithm>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated
#include "targets/enter_counter.h"
#include <bitset>


int _currentOffset = -1;
bool _declaring = false;
bool _entireModule = true; //Hack: The module is a sequence node
std::vector<std::string> _externsDec ;
std::vector<std::string> _invocations;
std::vector<std::string> _definitions;


//---------------------------------------------------------------------------
//     THIS IS THE VISITOR'S DEFINITION
//---------------------------------------------------------------------------

void zu::postfix_writer::addExternDeclaration(std::string id){
    if (std::find(std::begin(_externsDec), std::end(_externsDec), id) == std::end(_externsDec))
    {
        _externsDec.push_back(id);
    } // Else we already have that extern in the list
}

void zu::postfix_writer::addInvocation(std::string id){
    if (std::find(std::begin(_invocations), std::end(_invocations), id) == std::end(_invocations))
    {
        _invocations.push_back(id);
    } // Else we already have that extern in the list
}

void zu::postfix_writer::addDefinition(std::string const id){
    if (std::find(std::begin(_definitions), std::end(_definitions), id) == std::end(_definitions))
    {
        _definitions.push_back( id );
    } // Else we already have that extern in the list
}
bool zu::postfix_writer::find(std::vector<std::string> vector, std::string id){
    for(std::string str : vector){
        if(!str.compare(id)){
            return true;
        }
    }
    return false;
}

void zu::postfix_writer::processExterns(){
    for(std::string externs : _externsDec){
        if(find(_invocations,externs) && !find(_definitions,externs)){
            _pf.EXTERN(externs);
        }
    }
}

void zu::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
    bool process = false;
    if(_entireModule){
        //When sequence node is an entire module

        //Adding declarations of predefine functions
        addExternDeclaration("argc");
        addExternDeclaration("argv");
        addExternDeclaration("envp");
        addExternDeclaration("readi");
        addExternDeclaration("readd");
        addExternDeclaration("printi");
        addExternDeclaration("prints");
        addExternDeclaration("printd");
        addExternDeclaration("println");
        process = true;
        _entireModule = false;
    }
    for (size_t i = 0; i < node->size(); i++) {
        node->node(i)->accept(this, lvl);
    }
    if(process){
        processExterns();
    }
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_block_node(zu::block_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);

    _symtab.push();
    node->declaracoes()->accept(this,lvl);
    node->instrucoes ()->accept(this,lvl);
    _symtab.pop();
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {

    if(  _declaring){
        _pf.CONST(node->value());
    }else{
        _pf.INT(node->value()); // push an integer
    }
}

void zu::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
    int lbl1;

    /* generate the string */
    _pf.RODATA(); // strings are DATA readonly
    _pf.ALIGN(); // make sure we are aligned
    _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
    _pf.STR(node->value()); // output string characters
    if(_declaring == true){
        _pf.DATA();
        _pf.ALIGN();
        if(_globalSTR){
            _pf.GLOBAL(_id_being_declared, _pf.OBJ());
        }
        _pf.LABEL( _id_being_declared);
        _pf.ID(mklbl(lbl1));
    }else{
        ///* leave the address on the stack */

        _pf.TEXT(); // return to the TEXT segment
        _pf.ADDR(mklbl(lbl1)); // the string to be printed
    }
}

void zu::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
    int lb1;
    if(  _declaring){
        _pf.DOUBLE(node->value());
    }else{
        //std::cout<<"\nint :" <<node->value()<<"\n";
        //;;-- put double literal in RODATA
        _pf.RODATA();
        _pf.ALIGN();
        _pf.LABEL(mklbl(lb1 = ++_lbl));
        _pf.DOUBLE(node->value());

        //;;-- load literal onto stack
        _pf.TEXT();
        _pf.ADDR(mklbl(lb1));
        _pf.DLOAD();
    }
}
//---------------------------------------------------------------------------

void zu::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);
    node->argument()->accept(this, lvl); // determine the value

    if(node->argument()->type()->name() == basic_type::TYPE_DOUBLE)
        _pf.DNEG();
    else
        _pf.NEG(); // 2-complement
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {

    CHECK_TYPES(_compiler, _symtab, node);

	if(node->left()->type()->name() == basic_type::TYPE_POINTER){
		//cdk::expression_node *expr = node->left();
		//zu::lvalue_node  *pointer = dynamic_cast<zu::lvalue_node*>(expr);
		node->left()->accept(this, lvl);
		node->right()->accept(this, lvl);
		_pf.INT(node->left()->type()->subtype()->size()); //POINTER ALWAYS HAS SUBTYPE
		_pf.MUL();
		_pf.ADD();

	}else if(node->right()->type()->name() == basic_type::TYPE_POINTER){

		node->right()->accept(this, lvl);
		node->left()->accept(this, lvl);
		_pf.INT(node->right()->type()->subtype()->size()); //POINTER ALWAYS HAS SUBTYPE
		_pf.MUL();
		_pf.ADD();
	}
	else{
		node->left()->accept(this, lvl);

	    if(node->type()->name() == basic_type::TYPE_DOUBLE &&
	            node->left()->type()->name() == basic_type::TYPE_INT){
	        _pf.I2D();
	    }

	    node->right()->accept(this, lvl);
	    if(node->type()->name() == basic_type::TYPE_DOUBLE &&
	            node->right()->type()->name() == basic_type::TYPE_INT){
	        _pf.I2D();
	    }
	    if(node->type()->name() == basic_type::TYPE_INT){
	        _pf.ADD();
	    }else if(node->type()->name() == basic_type::TYPE_DOUBLE){
	        _pf.DADD();
	    }else{
	        throw "Add_node unsupported type";
	    }
	}
}
void zu::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);
    //node->left()->accept(this, lvl);
    //node->right()->accept(this, lvl);
    //_pf.SUB();
	if(node->left()->type()->name() == basic_type::TYPE_POINTER &&
		node->right()->type()->name() == basic_type::TYPE_POINTER){
			if(node->left()->type()->subtype()->name() != node->right()->type()->subtype()->name()){
				throw "can only sub pointers of same type";
			}else{
				node->left()->accept(this, lvl);
				node->right()->accept(this, lvl);
				_pf.SUB();
				_pf.INT(node->left()->type()->subtype()->size());
				_pf.DIV();
			}

	}else if(node->left()->type()->name() == basic_type::TYPE_POINTER){
		//cdk::expression_node *expr = node->left();
		//zu::lvalue_node  *pointer = dynamic_cast<zu::lvalue_node*>(expr);
		node->left()->accept(this, lvl);
		node->right()->accept(this, lvl);
		_pf.INT(node->left()->type()->subtype()->size()); //POINTER ALWAYS HAS SUBTYPE
		_pf.MUL();
		_pf.SUB();

	}else if(node->right()->type()->name() == basic_type::TYPE_POINTER){

		node->right()->accept(this, lvl);
		node->left()->accept(this, lvl);
		_pf.INT(node->right()->type()->subtype()->size()); //POINTER ALWAYS HAS SUBTYPE
		_pf.MUL();
		_pf.SUB();
	}
	else{
		node->left()->accept(this, lvl);

	    if(node->type()->name() == basic_type::TYPE_DOUBLE &&
	            node->left()->type()->name() == basic_type::TYPE_INT){
	        _pf.I2D();
	    }

	    node->right()->accept(this, lvl);
	    if(node->type()->name() == basic_type::TYPE_DOUBLE &&
	            node->right()->type()->name() == basic_type::TYPE_INT){
	        _pf.I2D();
	    }
	    if(node->type()->name() == basic_type::TYPE_INT){
	        _pf.SUB();
	    }else if(node->type()->name() == basic_type::TYPE_DOUBLE){
	        _pf.DSUB();
	    }else{
	        throw "Add_node unsupported type";
	    }
	}
}
void zu::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    node->left()->accept(this, lvl);
    if(node->type()->name() == basic_type::TYPE_DOUBLE &&
            node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if(node->type()->name() == basic_type::TYPE_DOUBLE &&
            node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    if(node->type()->name() == basic_type::TYPE_DOUBLE ){
        _pf.DMUL();
    }else{
        _pf.MUL();
    }
}
void zu::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    node->left()->accept(this, lvl);
    if(node->type()->name() == basic_type::TYPE_DOUBLE &&
            node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if(node->type()->name() == basic_type::TYPE_DOUBLE &&
            node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    if(node->type()->name() == basic_type::TYPE_DOUBLE ){
        _pf.DDIV();
    }else{
        _pf.DIV();
    }
}
void zu::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.MOD();
}
//---------------------------------------------------------------------------
void zu::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    bool doubleScenario = (node->left()->type()->name() == basic_type::TYPE_DOUBLE) ||  (node->right()->type()->name() == basic_type::TYPE_DOUBLE);

    node->left()->accept(this, lvl);
    if(doubleScenario && node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if(doubleScenario && node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    if( doubleScenario) {
        _pf.DCMP();
        _pf.INT(0);
        _pf.LT();

    } else if(node->type()->name() == basic_type::TYPE_INT){
        _pf.LT();
    } else {
        throw "lt_node unsupported type : postfix";
    }
}
void zu::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    bool doubleScenario = (node->left()->type()->name() == basic_type::TYPE_DOUBLE) ||  (node->right()->type()->name() == basic_type::TYPE_DOUBLE);

    node->left()->accept(this, lvl);
    if(doubleScenario && node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if(doubleScenario && node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    if( doubleScenario) {
        _pf.DCMP();
        _pf.INT(0);
        _pf.LE();
    } else if(node->type()->name() == basic_type::TYPE_INT){
        _pf.LE();
    } else {
        throw "le_node unsupported type : postfix";
    }
}
void zu::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    bool doubleScenario = (node->left()->type()->name() == basic_type::TYPE_DOUBLE) ||  (node->right()->type()->name() == basic_type::TYPE_DOUBLE);

    node->left()->accept(this, lvl);
    if(doubleScenario && node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if(doubleScenario && node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    if( doubleScenario) {
        _pf.DCMP();
        _pf.INT(0);
        _pf.GE();
    } else if(node->type()->name() == basic_type::TYPE_INT){
        _pf.GE();
    } else {
        throw "ge_node unsupported type : postfix";
    }
}
void zu::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    bool doubleScenario = (node->left()->type()->name() == basic_type::TYPE_DOUBLE) ||  (node->right()->type()->name() == basic_type::TYPE_DOUBLE);

    node->left()->accept(this, lvl);
    if(doubleScenario && node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if(doubleScenario && node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    if( doubleScenario) {
        _pf.DCMP();
        _pf.INT(0);
        _pf.GT();
    } else if(node->type()->name() == basic_type::TYPE_INT){
        _pf.GT();
    } else {
        throw "gt_node unsupported type : postfix";
    }
}
//---------------------------------------------------------------------------
void zu::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    bool doubleScenario = (node->left()->type()->name() == basic_type::TYPE_DOUBLE) ||  (node->right()->type()->name() == basic_type::TYPE_DOUBLE);

    node->left()->accept(this, lvl);
    if(doubleScenario && node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if(doubleScenario && node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    if( doubleScenario) {
        _pf.DCMP();
        _pf.INT(0);
        _pf.NE();

    } else if(node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_POINTER){
        _pf.NE();
    } else {
        throw "ne_node unsupported type : postfix";
    }
}
void zu::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    bool doubleScenario = (node->left()->type()->name() == basic_type::TYPE_DOUBLE) ||  (node->right()->type()->name() == basic_type::TYPE_DOUBLE);

    node->left()->accept(this, lvl);
    if(doubleScenario && node->left()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if(doubleScenario && node->right()->type()->name() == basic_type::TYPE_INT){
        _pf.I2D();
    }

    if( doubleScenario) {
        _pf.DCMP();
        _pf.INT(0);
        _pf.EQ();

    } else if(node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_POINTER){
        _pf.EQ();
    } else {
        throw "eq_node unsupported type : postfix";
    }

}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_rvalue_node(zu::rvalue_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);
    node->lvalue()->accept(this, lvl);

    if( node->type()->name() == basic_type::TYPE_DOUBLE ){
        _pf.DLOAD();
    } else if( node->type()->name() == basic_type::TYPE_INT ||
              node->type()->name() == basic_type::TYPE_POINTER ||
               node->type()->name() == basic_type::TYPE_STRING) {
        _pf.LOAD();
    }
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_lvalue_node(zu::lvalue_node * const node, int lvl) {
		throw "using abstract class";
    /*
     * CHECK_TYPES(_compiler, _symtab, node);
  // simplified generation: all variables are global
  _pf.ADDR(node->value());
*/
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_assignment_node(zu::assignment_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);

    if(node->type()->name() == basic_type::TYPE_INT ||
            node->type()->name() == basic_type::TYPE_STRING ||
            node->type()->name() == basic_type::TYPE_POINTER){
        node->rvalue()->accept(this, lvl); // determine the new value
        _pf.DUP();
        node->lvalue()->accept(this, lvl); // where to store the value
        _pf.STORE(); // store the value at address
    }else if(node->type()->name() == basic_type::TYPE_DOUBLE){
        node->rvalue()->accept(this, lvl); // determine the new value
        if(node->rvalue()->type()->name() == basic_type::TYPE_INT){
            _pf.I2D();
        }
        _pf.DDUP();
        node->lvalue()->accept(this, lvl); // where to store the value
        _pf.DSTORE();
    }
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_function_def_node(zu::function_def_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);
    // generate the main function (RTS mandates that its name be "_main")

    std::string name = "";
    _currentFunctionName = *node->name();
    std::shared_ptr<zu::symbol> symbol = _symtab.find(*node->name());

    if(*node->name() == "zu"){
        name = "_main";
    } else if(*node->name() == "_main"){
        name = "zu";
    } else{
        name = *node->name();
    }
    addDefinition(name);


    _pf.TEXT();
    _pf.ALIGN();
    if(symbol->isGlobal())
        _pf.GLOBAL(name, _pf.FUNC());

    _pf.LABEL(name);

    //Local Variables
    enter_counter* visitorCounter = new enter_counter(_compiler);
    node->accept(visitorCounter, lvl);
    int  counter = visitorCounter->getCounter();

    //counter += node->type()->size(); counter has return already
    _pf.ENTER(counter);

    //Return Type
    _declaring = false;


    delete(visitorCounter);

    _symtab.push();
    // Take Care of the arguments
    _currentOffset = 8;
    if(node->input() != nullptr){

        for (size_t i = 0 ; i < node->input()->size() ; i++) {
            zu::var_dec_node *arg = (zu::var_dec_node*) node->input()->node(i);
            arg->accept(this,lvl);
        }
    }


    _currentOffset = 0 - node->type()->size();
    if(node->literal()){

        node->literal()->accept(this,lvl); //Put value on stack

        //put return variable on stack
        if( node->type()->name() == basic_type::TYPE_DOUBLE ){
			if(node->literal()->type()->name() == basic_type::TYPE_INT ){
				_pf.I2D();
			}
            _pf.LOCAL(-8);
            _pf.DSTORE();
        } else if( node->type()->name() != basic_type::TYPE_VOID ){
            _pf.LOCA(-4);
        }
    }

    symbol->offset( _currentOffset );


    node->corpo()->accept(this, lvl);

// end the function
    if( node->type()->name() == basic_type::TYPE_DOUBLE ){
        _pf.LOCAL(-8);
        _pf.DLOAD();
        _pf.DPOP();
    } else if( node->type()->name() != basic_type::TYPE_VOID ){
        _pf.LOCV(-4);
        _pf.POP();
    }
    _pf.LEAVE();
    _pf.RET();

    _currentOffset = -1;
    _symtab.pop();
    // these are just a few library function imports
    _currentFunctionName = "";
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_evaluation_node(zu::evaluation_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);
    node->argument()->accept(this, lvl); // determine the value
    if (node->argument()->type()->name() == basic_type::TYPE_INT) {
        _pf.TRASH(4); // delete the evaluated value
    }
    else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
        _pf.TRASH(4); // delete the evaluated value's address
    }
    else if (node->argument()->type()->name() == basic_type::TYPE_POINTER) {
        _pf.TRASH(4); // delete the evaluated value's address
    }
    else if (node->argument()->type()->name() == basic_type::TYPE_VOID) {
        //      _pf.TRASH(0); // delete the evaluated value's address
    }
    else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE) {
        _pf.TRASH(8); // delete the evaluated value's address
    }
    else {
        std::bitset<32> x(node->argument()->type()->name());
        std::cerr << "ERROR: CANNOT HAPPEN! evaluation_node : TYPE : " << x << std::endl;
        exit(1);
    }
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_print_node(zu::print_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);
    node->argument()->accept(this, lvl); // determine the value to print
    if (node->argument()->type()->name() == basic_type::TYPE_INT) {
        _pf.CALL("printi");
        addInvocation("printi");
        _pf.TRASH(4); // delete the printed value

    } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
        _pf.CALL("prints");
        addInvocation("prints");
        _pf.TRASH(4); // delete the printed value's address

    } else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE) {
        _pf.CALL("printd");
        addInvocation("printd");
        _pf.TRASH(8); // delete the printed value's address

    } else {
        std::bitset<32> x(node->argument()->type()->name());
        std::cerr << "ERROR: CANNOT HAPPEN! do_print_node : TYPE : " << x << std::endl;
        exit(1);
    }
    //_pf.CALL("println"); // print a newline
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_println_node(zu::println_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);
    node->argument()->accept(this, lvl); // determine the value to print
    if (node->argument()->type()->name() == basic_type::TYPE_INT) {
        _pf.CALL("printi");
        addInvocation("printi");
        _pf.TRASH(4); // delete the printed value

    } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
        _pf.CALL("prints");
        addInvocation("prints");
        _pf.TRASH(4); // delete the printed value's address

    } else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE) {
        _pf.CALL("printd");
        addInvocation("printd");
        _pf.TRASH(8); // delete the printed value's address

    } else {
        std::bitset<32> x(node->argument()->type()->name());
        std::cerr << "ERROR: CANNOT HAPPEN! do_print_node : TYPE : " << x << std::endl;
        exit(1);
    }
    _pf.CALL("println"); // print a newline
    addInvocation("println");
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_read_node(zu::read_node * const node, int lvl) {
    CHECK_TYPES(_compiler, _symtab, node);



    if(node->type()->name() == basic_type::TYPE_DOUBLE){
        _pf.CALL("readd");
        addInvocation("readd");
        _pf.DPUSH();
        //_pf.DSTORE();
    } else if (node->type()->name() == basic_type::TYPE_INT){
        _pf.CALL("readi");
        addInvocation("readi");
        _pf.PUSH();
        //_pf.STORE();
    } else {
        throw "unsupported opperation";
    }
}

//---------------------------------------------------------------------------
/* Possivel teste pratico!
void zu::postfix_writer::do_while_node(zu::while_node * const node, int lvl) {
  int lbl1, lbl2;
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  node->condition()->acce 35 : C-03-34-N-ok C:1 Y:2 L:3 R:4
pt(this, lvl);
  _pf.JZ(mklbl(lbl2 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));node->argument()->type()->name()
  _pf.LABEL(mklbl(lbl2));
}
*/

//---------------------------------------------------------------------------

void zu::postfix_writer::do_for_node(zu::for_node * const node, int lvl){
    int cond= ++_lbl; //cond
    int incr= ++_lbl; //Incrnode->argument()->type()->name()
    int end = ++_lbl; //End

	_symtab.push();


    //INIT
    //TODO CLEAN STACK
    node->init()->accept(this,lvl);

    //Cond
    _cycleBreakVec.push_back(end);
    _cycleContVec.push_back(incr);

    _pf.ALIGN();
    _pf.LABEL(mklbl(cond));
    node->condition()->accept(this,lvl);
    _pf.JZ(mklbl(end));

    //Code
    node->block()->accept(this,lvl);

    //Incr
    _pf.ALIGN();
    _pf.LABEL(mklbl(incr));
    node->incr()->accept(this,lvl);
    _pf.JMP(mklbl(cond));

    //End
    //TODO CLEAN STACK
    _pf.ALIGN();
    _pf.LABEL(mklbl(end));
    _cycleContVec.pop_back() ;
    _cycleBreakVec.pop_back();

	_symtab.pop();

}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_if_node(zu::if_node * const node, int lvl) {
    int lbl1;
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->block()->accept(this, lvl);
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_if_else_node(zu::if_else_node * const node, int lvl) {
    int lbl1, lbl2;

    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->thenblock()->accept(this, lvl + 2);
    _pf.JMP(mklbl(lbl2 = ++_lbl));
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl1));
    node->elseblock()->accept(this, lvl + 2);
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_and_node(zu::and_node * const node, int lvl){
    int lbl1;
    CHECK_TYPES(_compiler, _symtab, node);

    node->left()->accept(this, lvl);
    _pf.DUP();
    _pf.JZ(mklbl(lbl1 = ++_lbl)); 		//Se o 1º arg for falso, ja nao avalia o 2º

    node->right()->accept(this, lvl);
    _pf.AND();

    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_break_node(zu::break_node * const node, int lvl){
    int lbl = _cycleBreakVec.back();
    _pf.JMP(mklbl(lbl));
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_continue_node(zu::continue_node * const node, int lvl){
    int lbl = _cycleContVec.back();
    _pf.JMP(mklbl(lbl));
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_function_call_node(zu::function_call_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);

    std::string name = "";
    if(*node->name() == "zu")
        name = "_main";
    else if(*node->name() == "_main")
        name = "zu";
    else
        name = *node->name();

    std::shared_ptr<zu::symbol> symbol = _symtab.find(*node->name());




    int thrashSize = 0;
    cdk::sequence_node *args = node->argumentos();
    if(args != nullptr){
        for(int i = args->size(); i > 0; i--){
            cdk::expression_node * arg = (cdk::expression_node*) args->node(i-1);

            arg->accept(this, lvl);

            //implic conversion from int to double
            if((symbol->args().at(i-1)->name() == basic_type::TYPE_DOUBLE) && (arg->type()->name() == basic_type::TYPE_INT)){
                _pf.I2D();
            }
            thrashSize += arg->type()->size();
        }
    }
    _pf.CALL(name);
    addInvocation(name);
    _pf.TRASH(thrashSize);

    if( node->type()->name() == basic_type::TYPE_DOUBLE ){
        _pf.DPUSH();
    } else if( node->type()->name() != basic_type::TYPE_VOID ){
        _pf.PUSH();
    }
}

//---------------------------------------------------------------------------
void zu::postfix_writer::do_function_dec_node(zu::function_dec_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);

    if(node->import()){
        //ADD EXTERN..
//        _pf.EXTERN(*(node->name()));
        addExternDeclaration(*(node->name()));
    }
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_identity_node(zu::identity_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);

    node->argument()->accept(this, lvl); // determine the value
}
//---------------------------------------------------------------------------

void zu::postfix_writer::do_index_node(zu::index_node * const node, int lvl){
	CHECK_TYPES(_compiler, _symtab, node);

    if(node->base()->type()->name() == basic_type::TYPE_POINTER){
        //cdk::expression_node *expr = node->left();
        //zu::lvalue_node  *pointer = dynamic_cast<zu::lvalue_node*>(expr);
        node->base()->accept(this, lvl);
        node->position()->accept(this, lvl);
        _pf.INT(node->base()->type()->subtype()->size()); //POINTER ALWAYS HAS SUBTYPE
        _pf.MUL();
        _pf.ADD();

    }else if(node->position()->type()->name() == basic_type::TYPE_POINTER){

        node->position()->accept(this, lvl);
        node->base()->accept(this, lvl);
        _pf.INT(node->position()->type()->subtype()->size()); //POINTER ALWAYS HAS SUBTYPE
        _pf.MUL();
        _pf.ADD();
    }else{
        throw "index_node unsupported type";
    }
}


//---------------------------------------------------------------------------

void zu::postfix_writer::do_mem_alloc_node(zu::mem_alloc_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);

    node->argument()->accept(this, lvl); // Coloca numero de Argumentos



    int sizeOf = node->type()->subtype()->size();
    _pf.INT(sizeOf);//Tamanho de um double
    _pf.MUL();		//Num de bytes a reservar
    _pf.ALLOC(); 	//Reserva memoria
    _pf.SP();		//Deixa no topo da pilha o endereco do SP


}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_not_node(zu::not_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);
    node->argument()->accept(this, lvl); // determine the value
    _pf.NOT(); // logic not INTS only
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_or_node(zu::or_node * const node, int lvl){
    int lbl1;
    CHECK_TYPES(_compiler, _symtab, node);

    node->left()->accept(this, lvl);
    _pf.DUP();
    _pf.JNZ(mklbl(lbl1 = ++_lbl)); 		//Se o 1º arg for verdadeiro, ja nao avalia o 2º

    node->right()->accept(this, lvl);
    _pf.OR();

    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_return_node(zu::return_node * const node, int lvl){

	//get current Function Type
    std::shared_ptr<zu::symbol> currFunc = _symtab.find( _currentFunctionName );

	if(currFunc == nullptr){
		throw "return node out of Function context";
	}

	if( currFunc->type()->name() == basic_type::TYPE_DOUBLE ){
		_pf.LOCAL(-8);
		_pf.DLOAD();
        _pf.DPOP();
	} else if( currFunc->type()->name() != basic_type::TYPE_VOID ){
		_pf.LOCV(-4);
        _pf.POP();
	}
    _pf.LEAVE();
    _pf.RET();
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_var_dec_node(zu::var_dec_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);

    const std::string &id = *(node->name());
    if((!_currentFunctionName.compare(id)) && _currentFunctionName.compare("")){
        throw id + "redeclared";
    }


    std::shared_ptr<zu::symbol> symbol = _symtab.find(id);

    if(_currentOffset > 0){
        //Dealing with function arguments
        symbol->offset(_currentOffset);
        _currentOffset += node->type()->size();
    } else if(_currentOffset != -1) {
        //Dealing with locar variables
        _currentOffset -= node->type()->size();
        symbol->offset(_currentOffset);
    } else {
        if( !symbol->import() ){

            //Then it is a global variable
            _pf.BSS();
            _pf.ALIGN();   // make sure we are aligned
            _pf.LABEL(id); // name variable location
            _pf.BYTE(node->type()->size());
            _pf.TEXT();    // return to the TEXT segment
        }else{
            addExternDeclaration(id);
        }
    }
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_var_def_node(zu::var_def_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);

    const std::string &id = *(node->name());
    if((!_currentFunctionName.compare(id)) && _currentFunctionName.compare("")){
        throw id + "redeclared";
    }
    addDefinition(id);
    _id_being_declared = id;
    std::shared_ptr<zu::symbol> symbol = _symtab.find(id);
    //std::cout << "\nvar def offset: " << _currentOffset << std::endl;

    if( _currentOffset > 0){
        throw "\033[31minternal error.. : vardef current offset > 0 : do_var_def_node \033[0m";
    } else if(_currentOffset != -1){
        _currentOffset -= node->type()->size();
        symbol->offset(_currentOffset);
        _declaring = false;


        node->expression()->accept(this, lvl);
        if(node->type()->name() == basic_type::TYPE_DOUBLE){
            if(node->expression()->type()->name() == basic_type::TYPE_INT){
                _pf.I2D();
            }
            _pf.LOCAL( _currentOffset );
            _pf.DSTORE();
        }else{
            _pf.LOCA( _currentOffset );

        }
    } else {
        //Global Context
        symbol->offset(-1);

        _declaring = true;
        if(symbol->type()->name() == basic_type::TYPE_STRING){

            if(symbol->isGlobal()){
                _globalSTR = true;
            } else {
                _globalSTR = false;
            }
            node->expression()->accept(this, lvl);
            _globalSTR = false;

        }else{
            _pf.DATA();
            _pf.ALIGN();
            if(symbol->isGlobal()){
                _pf.GLOBAL(id,_pf.OBJ());
            }
            _pf.LABEL(id);

            node->expression()->accept(this, lvl);


        }
        _declaring = false;

        _pf.TEXT(); // return to the TEXT segment
    }

}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_identifier_node(zu::identifier_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);
    // simplified generation: all variables are global

    std::string id = *(node->value());

    std::shared_ptr<zu::symbol> symbol = _symtab.find(id);
    int offset = symbol->offset();

    if(offset != -1){
        //Local Variable
        //No need to check for void function case, symbol already contains the exact value
        _pf.LOCAL(offset);
    }else{
        //Global variable
        _pf.ADDR(id); //Value is identifier name
        //Only need to do this in global variables
        addInvocation(id);
    }
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_addressOf_node(zu::addressOf_node * const node, int lvl){
    CHECK_TYPES(_compiler, _symtab, node);

    node->lvalue()->accept(this, lvl); //Ao visitar o lvalue ele deixa no topo da pilha o seu endereco
}

//---------------------------------------------------------------------------

void zu::postfix_writer::do_new_node(zu::new_node * const node, int lvl){
  //TODO:FIXME:XXX
}
