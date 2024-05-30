#ifndef TAC_HPP
#define TAC_HPP

#pragma once
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

struct quad{
string op;
    string arg1;
    string arg2;
    string result;
    string code = ""; 
    int rel_jump = 0, abs_jump = 0, ins_line = 0;
    int made_from = 0;
    bool is_target = false;
    enum code_code {
        BINARY,
        ASSIGNMENT,
        CONDITIONAL,
        GOTO,
        BEGIN_FUNC,
        END_FUNC,
        RETURN_VAL,
        PUSH_PARAM,
        POP_PARAM,
        RET,
        FUNC_CALL,
        SHIFT_POINTER,
        STORE,
        LOAD,
        UNARY,
        PRINT_STR
    };

    quad();
    quad(string r, string a1, string o, string a2);     
    void make_code();                              
    void make_code_from_binary();                  
    void make_code_from_assignment();                   
    void make_code_from_conditional();                 
    void make_code_from_goto();                        
    void make_code_begin_func();                     
    void make_code_end_func(); 
    void check_jump(const int);
    void make_code_from_return_val();
    void make_code_push_param(); 
    void make_code_pop_param(); 
    void make_code_from_return(); 
    void make_code_from_func_call(); 
    void make_code_shift_pointer();
    void make_code_from_store();                
    void make_code_from_load(); 
    void make_code_from_unary(); 
    void make_code_print_string();
};

#endif