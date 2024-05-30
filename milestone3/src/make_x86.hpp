#ifndef make_X86_HPP
#define make_X86_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <set>
#include <queue>
#include <stack>
#include <utility>
#include <cmath>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <cstring>
#include "make_tac.hpp"
#include "ast.hpp"

using namespace std;

struct asm_ins{
    string op = "";
    string arg_1 = "";
    string arg_2 = "";
    string arg_3 = "";
    string asm_code = "";
    string asm_ins_type = "";
    string remark = "";

    asm_ins();
    asm_ins(string, string a_1 = "", string a_2 = "", string a_3 = "", string ai_type = "ins", string remark = "");
};
struct prog_elem{
    string n = "";
    int offset_bp = 0;         
    prog_elem();
    prog_elem(string, int);
};

struct prog_elem_info{
    string n;
    bool is_main = false;
    map<string, prog_elem> check_info;
    int size;
    int params = 0;

    prog_elem_info();
    void fill_prog_elem_info(vector<quad> elem_ins);
    bool check_var(string s);
};

struct gen_asm_code {
    vector< vector<quad> > prog_elems;
    vector<asm_ins> gen_code;
    vector<prog_elem_info* > prog_elem_infos;
    
    gen_asm_code();
    void code_append(asm_ins ins);
    void code_print(string s);

    void get_prog_elems(AstNode* root);    
    void gen_elem_bb(vector<quad>, prog_elem*);  
    
    bool check_var(string s);
    bool check_main(string s);
    string name_prog_elem(string s);;          

    void first_gen();    
    void second_gen(AstNode* root);   
    void const_prog_elem_gen();    
    void gen_prog_elem(vector<quad> elem_ins);  
    void gen_prog_bb(vector<quad> BB, prog_elem_info*);
    vector<asm_ins> gen_x86_code(quad, int a = 0, int b = 0, int c = 0);    
};


#endif