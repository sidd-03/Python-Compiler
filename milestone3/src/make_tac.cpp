#include "make_tac.hpp"
#include "ast.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std;
quad::quad(string r, string a1, string o, string a2) : result(r), arg1(a1), op(o), arg2(a2) {}
quad::quad() {}


void quad::make_code_from_assignment(){
    made_from = ASSIGNMENT;
    code = "\t\t" + result + " " + op + " " + arg1 + ";\n";
    op = " = ";
    arg2 = "";
    cout<<code<<endl;
}

void quad::make_code_from_binary(){
    made_from = BINARY;
    code = "\t\t" + result + " = " + arg1 + " " + op + " " + arg2 + ";\n";
    cout<<code<<endl;
}

void quad::make_code_from_conditional(){
    made_from = CONDITIONAL;
    // if(!rel_jump){
    //     rel_jump = stoi(arg2.substr(1, arg2.size()-1));
    // }
    code = "\t\t" + op + " " + arg1 + " goto "+ arg2 +";\n";
    cout<<code<<endl;
    result = "";
    
}
void quad::make_code_from_goto(){
    made_from = GOTO;
    // if(!rel_jump){
    //     rel_jump = stoi(arg1.substr(1, arg1.size()-1));
    // }
    code = "\t\t" + op + " "+arg1+";\n";       // op is "GOTO"
    arg2 = "";
    result = "";
    cout<<code<<endl;
}

void quad::check_jump(const int ins_line){
    this->ins_line = ins_line;
    if(rel_jump){
        abs_jump = ins_line + rel_jump;
        code += to_string(abs_jump) + ";\n";
    }
}
void quad::make_code_begin_func() {
    made_from = BEGIN_FUNC;
    code = "\t\tbegin_func " + arg1 + "\n";
    arg2 = "";
    op = "";
    result = "";
    cout<<code<<endl;
}

void quad::make_code_end_func() {
    made_from = END_FUNC;
    code = "\t\tend_func\n";
    arg1 = "";
    arg2 = "";
    op = "";
    result = "";
    cout<<code<<endl;
}

void quad::make_code_push_param(){
    made_from = PUSH_PARAM;
    code = "\t\t" + op + " " + arg1 + ";\n";

    arg2 = "";
    result = "";
    cout<<code<<endl;
}

void quad::make_code_pop_param() {
    made_from = POP_PARAM;
    code = "\t\t" + result + " = pop_param;\n";

    arg2 = "";
    arg1 = "";
    op = "";
    cout<<code<<endl;
}
void quad::make_code_from_return() {
    made_from = RET;
    if(arg1 != ""){
        code = "\t\t" + op + " " + arg1 + ";\n";    // op is "return"
    }
    else{
        code = "\t\t" + op + ";\n";                 // op is "return"
    }
    arg2 = "";
    result = "";
    cout<<code<<endl;
}
void quad::make_code_shift_pointer() {
    made_from = SHIFT_POINTER;
    code = "\t\tshift_pointer " + arg1 + "\n";

    result = "";
    op = "";
    arg2 = "";
    cout<<code<<endl;
}
void quad::make_code_from_func_call(){
    made_from = FUNC_CALL;
    code = "\t\t" + op + " " + arg1 + ";\n";
    result = "";
    arg2 = "";
    cout<<code<<endl;
}
void quad::make_code_from_return_val() {
    made_from = RETURN_VAL;
    if(result != "") {
        code = "\t\t" + result + " = " + "return_value;\n";
    }
    else {
        code = "\t\treturn_value;\n";
    }

    arg1 = "";
    arg2 = "";
}

void quad::make_code_from_store(){
    made_from = STORE;
    code = "\t\t*(" + result;
    if(arg2 != ""){
        code += " + " + arg2;
    }
    code += ") = " + arg1 + ";\n";
    op = "";
    cout<<"Making code store: "<<code<<endl;
}

void quad::make_code_from_load(){
    made_from = LOAD;
    code = "\t\t" + result + " = *(" + arg1;
    if(arg2 != "") {
        code = code + " + " + arg2;
    }
    code = code + ");\n";
    op = "";
    cout<<"Making code load: "<<code<<endl;
}
void quad::make_code_from_unary(){
    made_from = UNARY;
    code = "\t\t" + result + " = " + op + "(" + arg1 + ");\n";
    arg2 = "";
}
void quad::make_code_print_string(){
    made_from = PRINT_STR;
    code = "\t\tprint  (" + arg1 + ");\n";
}


void quad::make_code(){
    if(this -> made_from == BINARY){
        this -> make_code_from_binary();
    }
    else if(this -> made_from == ASSIGNMENT){
        this -> make_code_from_assignment();
    }
    else if(this -> made_from == CONDITIONAL){
        this -> make_code_from_conditional();
    }
    else if(this -> made_from == GOTO){
        this -> make_code_from_goto();
    }
    else if(this -> made_from == PUSH_PARAM){
        this -> make_code_push_param();
    }
    else if(this -> made_from == BEGIN_FUNC){
        this -> make_code_begin_func();
    }
    else if(this -> made_from == END_FUNC){
        this -> make_code_end_func();
    }
    else if(this -> made_from == POP_PARAM) {
        this -> make_code_pop_param();
    }
     else if(this -> made_from == RET){
        this -> make_code_from_return();
    }
    else if(this -> made_from == FUNC_CALL){
        this -> make_code_from_func_call();
    }
    else if(this -> made_from == SHIFT_POINTER){
        this -> make_code_shift_pointer();
    }
    else if(this -> made_from == RETURN_VAL) {
        this -> make_code_from_return_val();
    }
    else if(this -> made_from == STORE){
        this -> make_code_from_store();
    }
    else if(this -> made_from == LOAD){
        this -> make_code_from_load();
    }
    else if(this -> made_from == UNARY){
        this -> make_code_from_unary();
    }
    else if(this -> made_from == PRINT_STR){
        this -> make_code_print_string();
    }
}  
