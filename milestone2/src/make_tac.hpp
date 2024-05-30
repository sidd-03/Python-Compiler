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
    string code = "";        // Construct from each node
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
        PUSH_PARAM,
        POP_PARAM,
        RET,
        FUNC_CALL,
        SHIFT_POINTER
    };

    quad();
    quad(string r, string a1, string o, string a2);     // res = arg1 op arg2
    void make_code();                                   // Recreate code
    void make_code_from_binary();                       // r = a1 op a2;
    void make_code_from_assignment();                   // r = a1;
    void make_code_from_conditional();                  // IFTRUE/FALSE a1 GOTO [filled later using check_jump()];
    void make_code_from_goto();                         // GOTO a1;
    void make_code_begin_func();                        // begin_func x;
    void make_code_end_func();                          // end_func;
    void check_jump(const int);
    void make_code_push_param();                        // pushparam a1;
    void make_code_pop_param();                         // r = popparam;
    void make_code_from_return();                       // return a1;
    void make_code_from_func_call();                    // callfunc a1;
    void make_code_shift_pointer();                     // shift stack pointer
};

#endif