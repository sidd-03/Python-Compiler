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
#include "make_x86.hpp"

using namespace std;
extern vector<string> glbl_string;

bool hasPrintSubstring(const std::string& str) {
    // Search for "print" in the given string
    size_t found = str.find("print");
    
    // If "print" is found, return true
    if (found != std::string::npos) {
        return true;
    } 
    
    // Otherwise, return false
    return false;
}

const int offset_sp = 8;
int num_func = 0;
map<string, string> name_func;

asm_ins::asm_ins(){;}



asm_ins::asm_ins(string op, string a_1, string a_2, string a_3, string ai_type, string remark) : op(op), arg_1(a_1), arg_2(a_2), arg_3(a_3), asm_ins_type(ai_type), remark(remark){
    if(ai_type == "ins") {  
        if(arg_3 == "") {
            asm_code = "\t\t" + op;
            if(arg_1 != ""){
                asm_code += "\t" + arg_1;
            } 
            if(arg_2 != ""){
                asm_code += ",\t" + arg_2;
            }
        }
    }
    else if(ai_type == "segment") {  
        asm_code = op;
        if(a_1 != "") {
            asm_code += "\t" + a_1;
        }
    }
    else if(ai_type == "label") {   
        asm_code = arg_1 + ":";
    }
    else if(ai_type == "remark") {
        asm_code = "\n\t\t# " + remark;
        return;
    }
    else {                      

    }
    if(remark != ""){
        asm_code += "\t\t# " + remark;
    }
    asm_code += "\n";
}

bool gen_asm_code::check_var(string s) {   
    if(s == "") {
        cout << "Error";
        exit(1);
    }
    return !(s[0] >= '0' && s[0] <= '9') && (s[0] != '-') && (s[0] != '+');
}

bool gen_asm_code::check_main(string s) {
    cout<<"Inside bool gen_asm_code::check_main(string s): "<<endl;
    string first = "";
    for(int i = s.length() - 1; i >= 0; i--) {
        if(s[i] == '.') {
            break;
        }
        else {
            first += s[i];
        }
    }

    return first == "niam";
}

string gen_asm_code::name_prog_elem(string s) {
    if(name_func.find(s) == name_func.end()) {
        num_func++;
        name_func[s] = "func" + to_string(num_func);
    }

    return name_func[s];
}

vector<asm_ins> gen_asm_code::gen_x86_code(quad q, int a, int b, int c){
    vector<asm_ins> asm_insts;
    asm_ins ins;

    if(q.code == ""){
        return asm_insts;        
    }
    else{
        if(q.made_from != quad::SHIFT_POINTER && q.made_from != quad::POP_PARAM){
            ins = asm_ins("", "", "", "", "remark", q.code.substr(2, q.code.size() - 2));
            asm_insts.push_back(ins);
        }
    }

    if(q.is_target) {  
        ins = asm_ins("", "L" + to_string(q.ins_line), "", "", "label");
        asm_insts.push_back(ins);
    }
    if(q.made_from == quad::BINARY){            

        if(q.op == "+"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("add", "$" + q.arg2, "%rdx");
            }
            else{
                ins = asm_ins("add", to_string(b) + "(%rbp)", "%rdx");
            }
        }
        else if(q.op == "-"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("sub", "$" + q.arg2, "%rdx");
            }
            else{
                ins = asm_ins("sub", to_string(b) + "(%rbp)", "%rdx");
            }
        }
        else if(q.op == "*"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("imul", "$" + q.arg2, "%rdx");
            }
            else{
                ins = asm_ins("imul", to_string(b) + "(%rbp)", "%rdx");
            }
        }
        else if(q.op == "/" || q.op == "//"){
            if(!check_var(q.arg1)){ 
                ins = asm_ins("movq", "$" + q.arg1, "%rax");
                asm_insts.push_back(ins);
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rax");
                asm_insts.push_back(ins);                
            }
            ins = asm_ins("cqto");
            asm_insts.push_back(ins);

            if(!check_var(q.arg2)){ 
                ins = asm_ins("movq", "$" + q.arg2, "%rbx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rbx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("div", "%rbx", ""); 
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "%rax", "%rdx");
        }
        else if(q.op == "%"){
            if(!check_var(q.arg1)){   
                ins = asm_ins("movq", "$" + q.arg1, "%rax");
                asm_insts.push_back(ins);
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rax");
                asm_insts.push_back(ins);                
            }
            ins = asm_ins("cqto");
            asm_insts.push_back(ins);

            if(!check_var(q.arg2)){  
                ins = asm_ins("movq", "$" + q.arg2, "%rbx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rbx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("idiv", "%rbx", "");  
        }
        else if(q.op == "<<"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("sal", "%cl", "%rdx");
        }
        else if(q.op == ">>"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("sar", "%cl", "%rdx");
        }
        else if(q.op == ">>>"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("shr", "%cl", "%rdx");
        }
        else if(q.op == ">"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);

            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "%rdx", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("jl", "1f");  
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f"); 
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        else if(q.op == "<"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);

            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "%rdx", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("jg", "1f");  
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f"); 
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        else if(q.op == ">="){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);

            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "%rdx", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("jle", "1f");  // true
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f"); // false
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        else if(q.op == "<="){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);

            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "%rdx", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("jge", "1f");  
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f"); 
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        else if(q.op == "=="){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);

            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "%rdx", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("je", "1f");  // true
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f"); // false
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        else if(q.op == "!="){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);

            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rcx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rcx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "%rdx", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("jne", "1f");  // true
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f"); // false
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        else if(q.op == "&"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("and", "$" + q.arg2, "%rdx");
            }
            else{
                ins = asm_ins("and", to_string(b) + "(%rbp)", "%rdx");
            }     
        }
        else if(q.op == "|"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("or", "$" + q.arg2, "%rdx");
            }
            else{
                ins = asm_ins("or", to_string(b) + "(%rbp)", "%rdx");
            }     
        }
        else if(q.op == "^"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("xor", "$" + q.arg2, "%rdx");
            }
            else{
                ins = asm_ins("xor", to_string(b) + "(%rbp)", "%rdx");
            }
        }
        else if(q.op == "AND"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("je", "1f");
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("je", "1f");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        else if(q.op == "OR"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jne", "1f");     // true
            asm_insts.push_back(ins);
            if(!check_var(q.arg2)){
                ins = asm_ins("movq", "$" + q.arg2, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jne", "1f");     // true
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");     // false
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        asm_insts.push_back(ins);
        
        ins = asm_ins("movq", "%rdx", to_string(c) + "(%rbp)");
        asm_insts.push_back(ins);
    }
    else if(q.made_from == quad::UNARY){        // b(b) = op a(a)
        if(q.op == "~"){
            if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);
            ins = asm_ins("not", "%rdx", "");
        }
        else if(q.op == "NOT"){
             if(!check_var(q.arg1)){
                ins = asm_ins("movq", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            }
            asm_insts.push_back(ins);

       
                ins = asm_ins("movq", "$1", "%rcx");
            
            asm_insts.push_back(ins);
            ins = asm_ins("cmp", "%rdx", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("jne", "1f");  // true
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$0", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f"); // false
            asm_insts.push_back(ins);
            ins = asm_ins("", "1", "", "", "label");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "$1", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("jmp", "2f");
            asm_insts.push_back(ins);
            ins = asm_ins("", "2", "", "", "label");
        }
        else if(q.op == "-"){
            ins = asm_ins("xor", "%rdx", "%rdx");
            asm_insts.push_back(ins);
            if(!check_var(q.arg1)){
                ins = asm_ins("sub", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("sub", to_string(a) + "(%rbp)", "%rdx");
            }
        }
        else if(q.op == "+"){
            ins = asm_ins("xor", "%rdx", "%rdx");
            asm_insts.push_back(ins);
            if(!check_var(q.arg1)){
                ins = asm_ins("add", "$" + q.arg1, "%rdx");
            }
            else{
                ins = asm_ins("add", to_string(a) + "(%rbp)", "%rdx");
            }
        }
        asm_insts.push_back(ins);
        
        ins = asm_ins("movq", "%rdx", to_string(b) + "(%rbp)");
        asm_insts.push_back(ins);
    }
    else if(q.made_from == quad::ASSIGNMENT){   
    int flag = 0;
    for(int i = 0 ; i< glbl_string.size() ; i++){
        if(glbl_string[i] == q.arg1) {
            flag = 1;
            string temp = "str" + to_string(i);
            ins = asm_ins("leaq" , temp + "(%rip)" , "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("movq", "%rdx", to_string(b) + "(%rbp)");
            asm_insts.push_back(ins);
            break;
        }
    }
        if(!check_var(q.arg1) && flag == 0){
            ins = asm_ins("movq", "$" + q.arg1, to_string(b) + "(%rbp)");
            asm_insts.push_back(ins);
        }
        else if( flag == 0){
            ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
            asm_insts.push_back(ins);            
            ins = asm_ins("movq", "%rdx", to_string(b) + "(%rbp)");
            asm_insts.push_back(ins);
        }

    }
    else if(q.made_from == quad::CONDITIONAL){  
        if(!check_var(q.arg1)){
            ins = asm_ins("movq", "$" + q.arg1, "%rdx");
        }
        else{
            ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
        }
        asm_insts.push_back(ins);
        ins = asm_ins("cmp", "$0", "%rdx");
        asm_insts.push_back(ins);
        
        if(q.op == "if_false"){
            ins = asm_ins("je", "L" + to_string(b));
        }
        else if(q.op == "if_true"){
            ins = asm_ins("jne", "L" + to_string(b));
        }
        asm_insts.push_back(ins);
    } 
    else if(q.made_from == quad::GOTO){         
        ins = asm_ins("jmp", "L" + to_string(a));
        asm_insts.push_back(ins);
    }
    else if(q.made_from == quad::STORE){        
        if(!check_var(q.arg1)){
            ins = asm_ins("movq", "$" + q.arg1, "%rax");
        }
        else{
            ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rax");
        }
        asm_insts.push_back(ins);
        
        ins = asm_ins("movq", to_string(c) + "(%rbp)", "%rdx");
        asm_insts.push_back(ins);

        if(q.arg2 == "" || !check_var(q.arg2)) {
            ins = asm_ins("movq", "%rax", q.arg2 + "(%rdx)");
            asm_insts.push_back(ins);
        }
        else {
            cout << "Unknown TAC `" << q.code << "`. Cannot make load from this code!" << endl;
            exit(1);  
        }
    }
    else if(q.made_from == quad::LOAD){        
        ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdx");
        asm_insts.push_back(ins);

        if(q.arg2 == "" || !check_var(q.arg2)) {
            ins = asm_ins("movq", q.arg2 + "(%rdx)", "%rdx");
            asm_insts.push_back(ins);
        }
        else {
            cout << "Unknown TAC `" << q.code << "`. Cannot make load from this code!" << endl;
            exit(1);
        }

        ins = asm_ins("movq", "%rdx", to_string(c) + "(%rbp)");
        asm_insts.push_back(ins);
    }
    else if(q.made_from == quad::BEGIN_FUNC) {  
        if(b == 1) {        
            ins = asm_ins("", "main", "", "", "label");
            asm_insts.push_back(ins);
        }
else{
    ins = asm_ins("", name_prog_elem(q.arg1), "", "", "label");     
        asm_insts.push_back(ins);
}
        
        ins = asm_ins("pushq", "%rbp");     
        asm_insts.push_back(ins);
        ins = asm_ins("movq", "%rsp", "%rbp");    
        asm_insts.push_back(ins);
        ins = asm_ins("pushq", "%rbx");
        asm_insts.push_back(ins);
        ins = asm_ins("pushq", "%rdi");
        asm_insts.push_back(ins);
        ins = asm_ins("pushq", "%rsi");
        asm_insts.push_back(ins);
        ins = asm_ins("pushq", "%r12");
        asm_insts.push_back(ins);
        ins = asm_ins("pushq", "%r13");
        asm_insts.push_back(ins);
        ins = asm_ins("pushq", "%r14");
        asm_insts.push_back(ins);
        ins = asm_ins("pushq", "%r15");
        asm_insts.push_back(ins);
        if(a > 0) {
            ins = asm_ins("sub", "$" + to_string(a), "%rsp");
            asm_insts.push_back(ins);
        }
    }
    else if(q.made_from == quad::RET) {    
        if(q.arg1 != "") {     
            if(!check_var(q.arg1)) {
                ins = asm_ins("movq", "$" + q.arg1, "%rax");
            }
            else {
                ins = asm_ins("movq", to_string(b) + "(%rbp)", "%rax");
            }
            asm_insts.push_back(ins);
        }
        
        ins = asm_ins("add", "$" + to_string(a), "%rsp");   
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%r15");                    
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%r14");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%r13");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%r12");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%rsi");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%rdi");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%rbx");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%rbp");
        asm_insts.push_back(ins);

        ins = asm_ins("ret");
        asm_insts.push_back(ins);
    }
    else if(q.made_from == quad::END_FUNC) {
        if(a == 1) {        
            ins = asm_ins("movq", "$60", "%rax");
            asm_insts.push_back(ins);
            ins = asm_ins("xor", "%rdi", "%rdi");
            asm_insts.push_back(ins);
            ins = asm_ins("syscall");
            asm_insts.push_back(ins);
        }
        else {                 
            ins = asm_ins("add", "$" + to_string(b), "%rsp");  
            asm_insts.push_back(ins);
            ins = asm_ins("popq", "%r15");                     
            asm_insts.push_back(ins);
            ins = asm_ins("popq", "%r14");
            asm_insts.push_back(ins);
            ins = asm_ins("popq", "%r13");
            asm_insts.push_back(ins);
            ins = asm_ins("popq", "%r12");
            asm_insts.push_back(ins);
            ins = asm_ins("popq", "%rsi");
            asm_insts.push_back(ins);
            ins = asm_ins("popq", "%rdi");
            asm_insts.push_back(ins);
            ins = asm_ins("popq", "%rbx");
            asm_insts.push_back(ins);
            ins = asm_ins("popq", "%rbp");
            asm_insts.push_back(ins);
            ins = asm_ins("ret");
            asm_insts.push_back(ins);
        }
    }
    else if(q.made_from == quad::SHIFT_POINTER) {
    }
    else if(q.made_from == quad::FUNC_CALL) {
       
        if(a == 0 && this ->name_prog_elem(q.arg1) != "main") {       
            ins = asm_ins("pushq", "%rax");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%r8");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%r9");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%r10");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%r11");
            asm_insts.push_back(ins);
        }
        
        ins = asm_ins("call", this -> name_prog_elem(q.arg1));      
        if(hasPrintSubstring( this -> name_prog_elem(q.arg1)) ) {
            cerr<<"Printing"<<endl;
            ins = asm_ins("call", "print");   
        }
        if(q.arg1 == "len") {
            ins = asm_ins("call", "len");
        }
        asm_insts.push_back(ins);

        if(hasPrintSubstring( this -> name_prog_elem(q.arg1)) ) {          
            ins = asm_ins("add", "$8", "%rsp");
            asm_insts.push_back(ins);
        }
        else if(this -> name_prog_elem(q.arg1) == "alloc_mem") {
            ins = asm_ins("add", "$8", "%rsp");             
            asm_insts.push_back(ins);
        }
        else if(a > 0) {                           
            ins = asm_ins("add", "$" + to_string(a*offset_sp), "%rsp");
            asm_insts.push_back(ins);
        }
    }
    else if(q.made_from == quad::RETURN_VAL) {
        
        if(q.result != "") {     
            ins = asm_ins("mov", "%rax", to_string(a) + "(%rbp)");
            asm_insts.push_back(ins);
        }

        ins = asm_ins("popq", "%r11");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%r10");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%r9");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%r8");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%rdx");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%rcx");
        asm_insts.push_back(ins);
        ins = asm_ins("popq", "%rax");
        asm_insts.push_back(ins);
    }
    else if(q.made_from == quad::PRINT_STR) {
        int flag = 0;
        for(int i= 0 ; i< glbl_string.size() ; i++){
            if(glbl_string[i] == q.arg1){
                ins = asm_ins("leaq", "str" + to_string(i) + "(%rip)" , "%rdi");
                asm_insts.push_back(ins);
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            ins = asm_ins("movq", to_string(a) + "(%rbp)", "%rdi"); 
            asm_insts.push_back(ins);
        }
        ins = asm_ins("call", "puts");
        asm_insts.push_back(ins);
    }
    else if(q.made_from == quad::PUSH_PARAM){   
        if(b == 1) {      
            ins = asm_ins("pushq", "%rax");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%rcx");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%rdx");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%r8");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%r9");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%r10");
            asm_insts.push_back(ins);
            ins = asm_ins("pushq", "%r11");
            asm_insts.push_back(ins);
        }
        if(!check_var(q.arg1)) { 
            ins = asm_ins("pushq", "$" + q.arg1, "");
            asm_insts.push_back(ins);
        } 
        else {
            ins = asm_ins("pushq", to_string(a) + "(%rbp)"); 
            asm_insts.push_back(ins);    
        }
    }

    return asm_insts;
}

gen_asm_code::gen_asm_code() {        
    prog_elems.clear();
    gen_code.clear();
}

void gen_asm_code::code_append(asm_ins ins) {
    this -> gen_code . push_back(ins);
}

void gen_asm_code::get_prog_elems(AstNode* root) {
    vector<quad> subroutine;

    bool func_started = false;

    for(quad q : root -> ta_codes) {
        if(q.made_from == quad::BEGIN_FUNC) {
            func_started = true;
        }

        if(func_started) {
            subroutine.push_back(q);
        }

        if(q.made_from == quad::END_FUNC) {
            func_started = false;
            if(subroutine.size()){
                this -> prog_elems.push_back(subroutine);
                subroutine.clear();
            }
        }
    }
}

void gen_asm_code::first_gen() {
    asm_ins ins;
    ins = asm_ins(".data", "", "", "", "segment");
    this -> gen_code.push_back(ins);
    if(glbl_string.size()>0){
    for(int i = 0  ; i< glbl_string.size()  ; i++){
        string temp = "str" + to_string(i) + ":" + ".string" ;
        ins = asm_ins(temp , "\""+glbl_string[i]+"\\n"+"\"", "", "", "ins");
        this -> gen_code.push_back(ins);
    }
    }
    ins = asm_ins("integer_format:", ".asciz", "\"%ld\\n\"", "", "ins");
    this -> gen_code.push_back(ins);

    ins = asm_ins(".global", "main", "", "", "segment");      
    this -> gen_code.push_back(ins);

}
void gen_asm_code::gen_prog_bb(vector<quad> BB, prog_elem_info* elem) {
    for(quad q : BB) {
        vector<asm_ins> asm_insts;
        if(q.made_from == quad::CONDITIONAL){
            //cout<<"Conditional operation"<<endl;
            int a = elem -> check_info[q.arg1].offset_bp;
            int b = q.abs_jump;
            asm_insts = this -> gen_x86_code(q, a, b);
        }
        else if(q.made_from == quad::GOTO){
            //<<"Goto operation"<<endl;
            asm_insts = this -> gen_x86_code(q, q.abs_jump);
        }
        else if(q.made_from == quad::BINARY){
            //cout<<"Binary operation"<<endl;
            int c = elem -> check_info[q.result].offset_bp;
            int a = elem -> check_info[q.arg1].offset_bp;
            int b = elem -> check_info[q.arg2].offset_bp;
            asm_insts = this -> gen_x86_code(q, a, b, c);            
        }
        else if(q.made_from == quad::UNARY){    
            int y = elem -> check_info[q.result].offset_bp;
            int x = elem -> check_info[q.arg1].offset_bp;
            asm_insts = this -> gen_x86_code(q, x, y);           
        }
        else if(q.made_from == quad::ASSIGNMENT){   
        //cout<<"Assignment operation"<<endl;
            int b = elem -> check_info[q.result].offset_bp;
            int a = elem -> check_info[q.arg1].offset_bp;
            asm_insts = this -> gen_x86_code(q, a, b);                
        }
         else if(q.made_from == quad::PRINT_STR){   
        //cout<<"Assignment operation"<<endl;
            int b = elem -> check_info[q.result].offset_bp;
            int a = elem -> check_info[q.arg1].offset_bp;
            asm_insts = this -> gen_x86_code(q, a, b);                
        }
        else if(q.made_from == quad::STORE){       
            int x = elem -> check_info[q.arg1].offset_bp;
            int y = elem -> check_info[q.arg2].offset_bp;   
            int z = elem -> check_info[q.result].offset_bp;

            asm_insts = this -> gen_x86_code(q, x, y, z);
        }
        else if(q.made_from == quad::LOAD){        
            int x = elem -> check_info[q.arg1].offset_bp;
            int y = elem -> check_info[q.arg2].offset_bp; 
            int z = elem -> check_info[q.result].offset_bp;

            asm_insts = this -> gen_x86_code(q, x, y, z);
        }
        else if(q.made_from == quad::PUSH_PARAM){  
        //cout<<"Push parameter operation"<<endl;
            int x = elem -> check_info[q.arg1].offset_bp;
            elem -> params++;
            asm_insts = this -> gen_x86_code(q, x, elem -> params);
        }
        else if(q.made_from == quad::POP_PARAM){  
        //cout<<"Pop parameter operation"<<endl;
            // no need to do anything really

            asm_insts = this -> gen_x86_code(q);
        }
        else if(q.made_from == quad::FUNC_CALL) {
            //<<"Function call operation"<<endl;
            asm_insts = this -> gen_x86_code(q, elem -> params);
            elem -> params = 0;         
        }
        else if(q.made_from == quad::RETURN_VAL) {
            asm_insts = this -> gen_x86_code(q, elem -> check_info[q.result].offset_bp);
        }
        else if(q.made_from == quad::BEGIN_FUNC) {  
        //cout<<"Begin function operation"<<endl;
            if(check_main(q.arg1)) {
                elem -> is_main = true;
            }
            asm_insts = this -> gen_x86_code(q, elem -> size - 8 * offset_sp, elem -> is_main);        
        }
        else if(q.made_from == quad::END_FUNC) {  
        //cout<<"End function operation"<<endl;
            // ideally only reaches this place in a void function
            asm_insts = this -> gen_x86_code(q, elem -> is_main, elem -> size - 8 * offset_sp);
        }
        else if(q.made_from == quad::SHIFT_POINTER) {      
        //cout<<"Shift pointer operation"<<endl;
            asm_insts = this -> gen_x86_code(q);
        }
        else if(q.made_from == quad::RET) {     
            asm_insts = this -> gen_x86_code(q, elem -> size - 8 * offset_sp, elem -> check_info[q.arg1].offset_bp);
        }
        else{
            asm_insts = this -> gen_x86_code(q);
        }

        for(asm_ins ins : asm_insts) {
            //cout<<"Appending asm_ins"<<endl;    
            this -> code_append(ins);
        }
    }
}

void gen_asm_code::gen_elem_bb(vector<quad> elem, prog_elem* info) {    
    //cout << "Generating basic blocks" << endl;
    set<int> leaders;
    vector<quad > BB;

    int base_offset = elem[0].ins_line;
    leaders.insert(base_offset);

    for(quad q : elem) {
        if(q.made_from == quad::CONDITIONAL || q.made_from == quad::GOTO) {
            //cout<<"Conditional or Goto operation"<<endl;
            leaders.insert(q.abs_jump);
            leaders.insert(q.ins_line + 1);
        }
        else if(q.made_from == quad::FUNC_CALL) {
            //cout<<"Function call operation"<<endl;
            leaders.insert(q.ins_line);
            leaders.insert(q.ins_line + 1); 
        }
    }

    vector<int> ascending_leaders;
    for(int leader : leaders) { 
        ascending_leaders.push_back(leader); 
    }
    
    int prev_leader = ascending_leaders[0];
    for(int i = 1; i < ascending_leaders.size(); i++) {
        BB.clear();
        
        for(int j = prev_leader; j < ascending_leaders[i]; j++) {
            BB.push_back(elem[j - base_offset]);
        }
        prev_leader = ascending_leaders[i];

        this -> gen_elem_bb(BB, info);
    }

    BB.clear();
    int final_leader = ascending_leaders[ascending_leaders.size() - 1];
    for(int i = final_leader; i - base_offset < elem.size(); i++) {
        BB.push_back(elem[i - base_offset]);
    }

    this -> gen_elem_bb(BB, info);
}

void gen_asm_code::const_prog_elem_gen() {
    //cout << "Generating fixed subroutines" << endl;
    name_func["print"] = "print";
    name_func["alloc_mem"] = "alloc_mem";
}

void gen_asm_code::second_gen(AstNode* root) {
    //cout << "Generating x86 code" << endl;
    asm_ins ins(".text", "", "", "", "segment");
    this -> gen_code.push_back(ins);
    //cout << "Generating fixed subroutines in gen_text" << endl;
    this -> const_prog_elem_gen();
    //cout << "Generating tac subroutines in gen_text" << endl;
    this -> get_prog_elems(root);      // get the subroutines from entire TAC

    for(auto elem : this -> prog_elems) {
        prog_elem_info* info_tab = new prog_elem_info();
        info_tab -> fill_prog_elem_info(elem);

        this -> prog_elem_infos.push_back(info_tab);
        this -> gen_prog_bb(elem, info_tab);
    }
    //cout << "Code generation complete" << endl;
}
void gen_asm_code::code_print(string asm_file) {
    cout<< "Printing code to file: " << asm_file << endl;
    ofstream out(asm_file);
    
    if(asm_file == "") {
        for(auto ins : this -> gen_code) {
            cout << ins.asm_code;
        }
    }
    else {
        for(auto ins : this -> gen_code) {
            out << ins.asm_code;
        }
    }

    ifstream print_func("print_func.s");
    string line;

    while(getline(print_func, line)){
        out << line << '\n';
    }

    ifstream print_str("print_str.s");
    while(getline(print_str, line)){
        out << line << '\n';
    }
    ifstream alloc_mem("allocmem.s");
    while(getline(alloc_mem, line)) {
        out << line << '\n';
    }
    ifstream len("len_func.s");
    while(getline(len, line)) {
        out << line << '\n';
    }
}

prog_elem::prog_elem(){;}

prog_elem::prog_elem(string n, int offset_bp) {
    this -> n = n;
    this -> offset_bp = offset_bp;
}

prog_elem_info::prog_elem_info(){;}

bool prog_elem_info::check_var(string s) {  
    return !(s[0] >= '0' && s[0] <= '9') && (s[0] != '-') && (s[0] != '+');
}

void prog_elem_info::fill_prog_elem_info(vector<quad> elem_ins) {
    int pop_cnt = 2;        
    int offset_lcl = 8;   
    
    for(quad q : elem_ins) {
        if(q.made_from == quad::BEGIN_FUNC || q.made_from == quad::SHIFT_POINTER || q.made_from == quad::FUNC_CALL) {   
        //cout<<"Begin function operation"<<endl;
            continue; 
        }
         
        if(q.made_from == quad::POP_PARAM) {
            //cout<<"Pop parameter operation"<<endl;
            this -> check_info[q.result] = prog_elem(q.result, offset_sp*pop_cnt);
            pop_cnt++;
        }
        else {
            if(q.made_from == quad::CONDITIONAL) {
                //cout<<"Conditional operation"<<endl;
                if(this -> check_info.find(q.arg1) == this -> check_info.end() && check_var(q.arg1)) {
                    this -> check_info[q.arg1] = prog_elem(q.arg1, -offset_sp*offset_lcl);
                    offset_lcl++;
                }
            }
            else if(q.made_from == quad::GOTO){
                //cout<<"Goto operation"<<endl;
                continue;
            }
            else {
                if(q.arg1 != "" && this -> check_info.find(q.arg1) == this -> check_info.end() && check_var(q.arg1)) {
                    this -> check_info[q.arg1] = prog_elem(q.arg1, -offset_sp*offset_lcl);
                    offset_lcl++;
                }
                else if(q.arg2 != "" && this -> check_info.find(q.arg2) == this -> check_info.end() && check_var(q.arg2)) {
                    this -> check_info[q.arg2] = prog_elem(q.arg2, -offset_sp*offset_lcl);
                    offset_lcl++;
                }
                else if(q.result != "" && this -> check_info.find(q.result) == this -> check_info.end() && check_var(q.result)) {
                    this -> check_info[q.result] = prog_elem(q.result, -offset_sp*offset_lcl);
                    offset_lcl++;
                }
            }
        }
    }

    this -> size = offset_sp * offset_lcl;   
}