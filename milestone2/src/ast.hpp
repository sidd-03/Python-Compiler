#ifndef NEW_AST_H
#define NEW_AST_H

#pragma once
#include "make_tac.hpp"
#include <iostream>
#include <vector>
using namespace std;

class AstNode
{
public:
    int node_number = 0;
    string name = "undefined";
    string label = "undefined";
    int line_no;
    vector<quad> ta_codes;
    virtual void add(AstNode *node)=0;
    virtual void generate_quad() =0;
    virtual void print() const = 0;
    void append_tac(vector<quad> (&tacs))
    {
    for(auto (&tac) : tacs){
        this -> ta_codes.push_back(tac);
    }
    tacs.clear();
    }
string get_var_from_node(){
    /* if(this -> name == "#Name#"){
        string s = "";
        for(auto (&child) : this -> children){
            s += child -> name;
        }
        return s;
    }
    else */
    
    return "t" + to_string(this -> node_number);
}
    string get_value();
   void append_tac(AstNode* v){
    for(auto (&tac) : v -> ta_codes){
        this -> ta_codes.push_back(tac);
    }
    v -> ta_codes.clear();
}
    void print_tac() {
         int ins_count = 1;

    for(auto (&q) : this -> ta_codes) {
        
  
                cout << ins_count << (ins_count >= 100 ? ":" : ":\t") << q.code;
    
           
            ins_count++;
        }

   
    
    }
    virtual ~AstNode() {}
};
class NumberNode : public AstNode
{

    public:
    int value;
    NumberNode(string name, string label, int value);
    void add(AstNode *node) override;
    void print() const override;
    void generate_quad() override { return;}
    
};
class StringNode : public AstNode
{
public:
    string value;
    StringNode(string name, string label, string value)
    {
        this->name = name;
        this->label = label;
        this->value = value;
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }
    void generate_quad() override { return;}
    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << ": " << value << "\"]" << endl;
    }
};
class BoolNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    BoolNode()
    {
        this->name = "bool Statement";
        this->label = "Bool";
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << "\"]" << endl;
    }

    void generate_quad() override { return;}
    ~BoolNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};
class IdentifierNode : public AstNode
{

public:
    vector<AstNode*> next;
    string value = "undefined";
    vector<float> v;
    int is_dotted = 0;
    IdentifierNode(string name, string label, string value, int line_no)
    {
        this->name = name;
        this->label = label;
        this->value = value;
        this->line_no = line_no;
    }
    IdentifierNode(string name, string label, string value,vector<float> v, int line_no)
    {
        this->name = name;
        this->label = label;
        this->value = value;
        this->line_no = line_no;
        this -> v = v;
    }
     IdentifierNode(string name, string label, string value, int line_no , int is_dotted)
    {
        this->name = name;
        this->label = label;
        this->value = value;
        this->line_no = line_no;
        this -> is_dotted = is_dotted;
    }
    void add(AstNode * node) override
    {
        next.push_back(node);
        cout<<"IdentifierNode add "<<this->name<<" "<<node->name<<endl;
    }
    
    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << ": " << value << "\"]" << endl;
        for(const auto &arg: next){
             cout << "\t" << name << " -> " << arg->name << ";" << endl;
            arg->print();
        }
    }
    void generate_quad() override
    {

    if(next.size()==0) return;

    string arg1 = "";
    quad q("","","","");
    for(const auto &child : this->next) {
        cout<<"Inside generate_quad IdentifierNode: child->label"<<child->label<<endl;
        if (child->label == "Identifier") {       
        IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(child);
        arg1 = temp1->value;
        quad q(child->get_var_from_node(), arg1 , "=" , "");
        cout<<"Inside generate_quad IdentifierNode: here"<<endl;
        q.make_code_from_assignment();
    this->ta_codes.push_back(q);
        } 
        else if (child->label == "float" || child->label == "int") {
        NumberNode* temp1 = dynamic_cast<NumberNode*>(child);
        arg1 =to_string(temp1->value);
        quad q(child->get_var_from_node(), arg1 , "=" , "");
        q.make_code_from_assignment();
    this->ta_codes.push_back(q);
        } 
        else if (child->label == "string") {
        StringNode* temp1 = dynamic_cast<StringNode*>(child);
        arg1 = temp1->value;
        quad q(child->get_var_from_node(), arg1 , "=" , "");
        q.make_code_from_assignment();
    this->ta_codes.push_back(q);
        }
        else if (child->label == "True" || child->label == "False") {
        BoolNode* temp1 = dynamic_cast<BoolNode*>(child);
        arg1 = temp1->label;
        quad q(child->get_var_from_node(), arg1 , "=" , "");
        q.make_code_from_assignment();
    this->ta_codes.push_back(q);
        }
        else {
        // Handle error: unexpected label
        child->generate_quad();
        this -> append_tac(child);
        // arg1 = child->get_var_from_node() ;
        // quad q(child->get_var_from_node(), arg1 , "=" , "");
        // q.make_code_from_assignment();
        //   this->ta_codes.push_back(q);
        }

    

    quad q1(this->get_var_from_node(), this->value , "=" , "");
    q1.make_code_from_assignment();
    this->ta_codes.push_back(q1);

    quad q2(this->next[0]->get_var_from_node(), this->next[0]->get_var_from_node() , "*" , "8");
    q2.make_code_from_binary();
    this -> ta_codes.push_back(q2);

    quad q3(this->get_var_from_node(), this->get_var_from_node() , "+" , this->next[0]->get_var_from_node());
    q3.make_code_from_binary();
    this -> ta_codes.push_back(q3);

        
        }
    }
};

class Arg : public AstNode
{
private:
    vector<AstNode *> next;

public:
    Arg(const string &name)
    {
        this->name = name;
        this->label = "Argument";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }
    void generate_quad() override { return;}
    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &arg : next)
        {
            arg->print();
        }
    }
};

class Args : public AstNode
{

public:
     vector<AstNode *> next;
    Args(const string &name)
    {
        this->name = name;
        this->label = "Arguments";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }
    void generate_quad() override { 
        for(const auto &child: next){
            child->generate_quad();
            this->append_tac(child);
      }
      return;
    }
    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;

        for (const auto &arg : next)
        {
            cout << "\t" << name << " -> " << arg->name << ";" << endl;
            arg->print();
        }
    }
};

class StatementsNode : public AstNode
{
public:
     vector<AstNode *> next;
    StatementsNode(const string &name)
    {
        this->name = name;
        this->label = "Statements";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override {
        for(auto (&child) : next)
        {
        child -> generate_quad();
        this -> append_tac(child);
        cout<<"Inside StatementsNode genrated and appended "<<child->label<<endl;
        } 
        
    }
    ~StatementsNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

class FunctionNode : public AstNode
{
public:
   vector<AstNode *> next;
    FunctionNode(const string &name)
    {
        this->name = name;
        this->label = "Declare Fun";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << " : " << name << "\"]" << endl;
        for (const auto &item : next)
        {
            cout << "\t" << name << " -> " << item->name << ";" << endl;
            item->print();
        }
    }
     void generate_quad() override { 
        
        quad q("", this->name, "", "");
        q.make_code_begin_func();
        this -> ta_codes.push_back(q);
        cout<<"Inside FunctionNode: xyz "<<endl;
        Args* arguments = dynamic_cast<Args*>(this->next[0]);
        string arg1 = "";
       //cout<<"Inside FunctionNode: abc "<<endl;
       if(arguments != NULL){ 

       for(int i = arguments->next.size() - 1; i >= 0; i--) {
        cout<<"Inside FunctionNode: i "<<i<<endl;
        // if (arguments->next[i]->label == "Identifier") {  
        // IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(arguments->next[i]);
        // arg1 = temp1->value;
        // } 
        // else if (arguments->next[i]->label == "float" || arguments->next[i]->label == "int") {
        // NumberNode* temp1 = dynamic_cast<NumberNode*>(arguments->next[i]);
        // arg1 = to_string(temp1->value);
        // } 
        // else if (arguments->next[i]->label == "string") {
        // StringNode* temp1 = dynamic_cast<StringNode*>(arguments->next[i]);
        // arg1 = temp1->value;
        // }
        // else if (arguments->next[i]->label == "True" || arguments->next[i]->label == "False") {
        // BoolNode* temp1 = dynamic_cast<BoolNode*>(arguments->next[i]);
        // arg1 = temp1->label;
        // }
        // else {
        // // Handle error: unexpected label
        // arg1 = arguments->next[i]->get_var_from_node();
        //  }
         arg1 = arguments->next[i]->get_value();

        q = quad(arg1, "", "pop_param", "");
        q.make_code_pop_param();
        this -> ta_codes.push_back(q);

        }
       }
        cout<<"Inside FunctionNode: abc "<<endl;
        StatementsNode*temp;
    if(arguments != NULL) {
       temp  = dynamic_cast<StatementsNode*>(this->next[1]);
    }   
    else temp  = dynamic_cast<StatementsNode*>(this->next[0]);
        for(auto (&child) : temp->next)
        {
        child -> generate_quad();
        this -> append_tac(child);
        cout<<child->name<<endl;
        } 
        q.make_code_end_func();
        this -> ta_codes.push_back(q);
      return;
        }
    ~FunctionNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
}; 

class ClassNode : public AstNode
{
private:
    static vector<ClassNode*> allClasses;
    vector<AstNode *> next;

    
public:
    ClassNode(const string &name);
    void add(AstNode *node) override;
    void print() const override;
    ClassNode* findParentClassNode(const std::string& parentClassName);
    void generate_quad() override;
    ~ClassNode();
};

class ConstructorNode : public AstNode {
private:
     vector<AstNode *> next;

public:
    ConstructorNode(const std::string &name)
       {
        this->name = name;
        this->label = "Constructor: ";
    }

    void add(AstNode *node) override {
        next.push_back(node);
    }

    void print() const override {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
           for (const auto &item : next)
        {
            cout << "\t" << name << " -> " << item->name << ";" << endl;
            item->print();
        }
    }
    void generate_quad() override { 
         quad q("", this->name, "", "");
        q.make_code_begin_func();
        this -> ta_codes.push_back(q);
        cout<<"Inside ConstNode: xyz "<<endl;
        Args* arguments = dynamic_cast<Args*>(this->next[0]);
        string arg1 = "";
       //cout<<"Inside ConstNode: abc "<<endl;
       if(arguments != NULL){ 

       for(int i = arguments->next.size() - 1; i >= 0; i--) {
        cout<<"Inside ConstNode: i "<<i<<endl;
        // if (arguments->next[i]->label == "Identifier") {  
        // IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(arguments->next[i]);
        // arg1 = temp1->value;
        // } 
        // else if (arguments->next[i]->label == "float" || arguments->next[i]->label == "int") {
        // NumberNode* temp1 = dynamic_cast<NumberNode*>(arguments->next[i]);
        // arg1 = to_string(temp1->value);
        // } 
        // else if (arguments->next[i]->label == "string") {
        // StringNode* temp1 = dynamic_cast<StringNode*>(arguments->next[i]);
        // arg1 = temp1->value;
        // }
        // else if (arguments->next[i]->label == "True" || arguments->next[i]->label == "False") {
        // BoolNode* temp1 = dynamic_cast<BoolNode*>(arguments->next[i]);
        // arg1 = temp1->label;
        // }
        // else {
        // // Handle error: unexpected label
        // arg1 = arguments->next[i]->get_var_from_node();
        //  }
         arg1 = arguments->next[i]->get_value();

        q = quad(arg1, "", "pop_param", "");
        q.make_code_pop_param();
        this -> ta_codes.push_back(q);

        }
       }
        cout<<"Inside ConstNode: abc "<<endl;
        StatementsNode*temp;
    if(arguments != NULL) {
       temp  = dynamic_cast<StatementsNode*>(this->next[1]);
    }   
    else temp  = dynamic_cast<StatementsNode*>(this->next[0]);
        for(auto (&child) : temp->next)
        {
        child -> generate_quad();
        this -> append_tac(child);
        cout<<child->name<<endl;
        } 
        q.make_code_end_func();
        this -> ta_codes.push_back(q);


        return; 
        }

    ~ConstructorNode() {
       for (const auto &arg : next)
        {
            delete arg;
        }
    }
}; //destructor node




class ControlFlowBlockNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    ControlFlowBlockNode()
    {
        this->name = "Control Flow Block";
        this->label = "Block of Control Flow (if,elif,else)";
    }
    void add(AstNode *node) override
    {
        next.push_back(node);
    }
    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { return;}
    ~ControlFlowBlockNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
}; //not used

class ControlFlowBlockContentNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    ControlFlowBlockContentNode(const string &name)
    {
        this->name = name;
        this->label = "Content of Control Flow";
    }
    void add(AstNode *node) override
    {
        next.push_back(node);
    }
    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { return;}
    ~ControlFlowBlockContentNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
}; //not used

class LoopBlockNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    LoopBlockNode()
    {
        this->name = "Loop Block";
        this->label = "Block of Loop";
    }
    void add(AstNode *node) override
    {
        next.push_back(node);
    }
    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { return;}
    ~LoopBlockNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
}; //not used

class BlockNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    BlockNode(const string &name)
    {
        this->name = name;
        this->label = "Block";
    }
    void add(AstNode *node) override
    {
        next.push_back(node);
    }
    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { 
         for(auto (&child) : next)
        {
        child -> generate_quad();
        this -> append_tac(child);
        } 
        //return;
        }
    ~BlockNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

class BreakStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    BreakStatementNode()
    {
        this->name = "Break Statement";
        this->label = "break";
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << "\"]" << endl;
    }
    void generate_quad() override { 
        string op = "goto";
        string arg1 = "BREAK";
        quad q("", arg1, op, "");

        q.code = "\t\tgoto BREAK;\n";
        q.made_from = quad::GOTO;

        this -> ta_codes.push_back(q);
       // return;
        }
    ~BreakStatementNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

class ContinueStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    ContinueStatementNode()
    {
        this->name = "Continue Statement";
        this->label = "Continue";
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << "\"]" << endl;
    }
    void generate_quad() override {
            string op = "goto";
        string arg1 = "CONTINUE";
        quad q("", arg1, op, "");

        q.code = "\t\tgoto CONTINUE;\n";
        q.made_from = quad::GOTO;

        this -> ta_codes.push_back(q);
        // return;
        }
    ~ContinueStatementNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

class PassStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    PassStatementNode()
    {
        this->name = "pass Statement";
        this->label = "Pass";
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << "\"]" << endl;
    }
    void generate_quad() override { return;}
    ~PassStatementNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};


class FunctionCallNode : public AstNode
{

public:
    vector<AstNode *> next;
    int size = 0;
    string type;
    FunctionCallNode(const string &name)
    {
        this->name = name;
        this->label = "Function Call";
    }
FunctionCallNode(const string &name , int size)
    {
        this->name = name;
        this->label = "Function Call";
        this -> size = size;
    }
    FunctionCallNode(const string &name , int size , string type)
    {
        this->name = name;
        this->label = "Function Call";
        this -> size = size;
        this->type = type;
    }
    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << " : " << name << "\"]" << endl;
        vector<AstNode *>::iterator it;
        for (const auto &item : next)
        {
            cout << "\t" << name << " -> " << item->name << ";" << endl;
            item->print();
        }
    }
    void generate_quad() override { 
        cout<<"Inside generate_quad FunctionCallNode: "<<endl;
      //  cout<<"Inside generate_quad FunctionCallNode: this->next[0]->label: "<<this->next[0]->label<<endl;
    if(this->next.size() == 0) cout<<"Function without arguments"<<endl;
    if(this->next.size() != 0) {
        cout<<"Inside generate_quad FunctionCallNode: 2"<<endl;
        Args* arguments = dynamic_cast<Args*>(this->next[0]);
        cout<<"Inside generate_quad FunctionCallNode: arguments->next_size: "<<arguments->next.size()<<endl;
        string arg1 = "";
        vector<string> args;
        int i=0;
       for(const auto &child : arguments->next) {
        cout<<"Inside generate_quad FunctionCallNode: child->label"<<child->label<<endl;
        if (child->label == "Identifier") {       
        IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(child);
        args.push_back(temp1->value);
        cout<<"Inside generate_quad FunctionCallNode: here"<<endl;
        } 
        else if (child->label == "float" || child->label == "int") {
        NumberNode* temp1 = dynamic_cast<NumberNode*>(child);
        args.push_back(to_string(temp1->value)) ;
        } 
        else if (child->label == "string") {
        StringNode* temp1 = dynamic_cast<StringNode*>(child);
        args.push_back(temp1->value);
        }
        else if (child->label == "True" || child->label == "False") {
        BoolNode* temp1 = dynamic_cast<BoolNode*>(child);
        args.push_back (temp1->label);
        }
        else {
        // Handle error: unexpected label
        child->generate_quad();
        this -> append_tac(child);
        args.push_back(child->get_var_from_node()) ;

         }
        cout<<"Function arg: "<<args[i]<<endl;
        i++;
        }

        for(auto arg : args){
            quad q("", arg, "push_param", "");
            q.make_code_push_param();
            this -> ta_codes.push_back(q);
        }
        }
         cout<<"Inside generate_quad FunctionCallNode 1: "<<endl;
        //int frame_size = 0;
       quad q1("", "-" + to_string(this->size), "", "");
        q1.make_code_shift_pointer();
        this -> ta_codes.push_back(q1);

        string arg = this -> name;
        quad q2("", arg, "call_func", "");
        q2.make_code_from_func_call();
        this -> ta_codes.push_back(q2);

        quad q3("", "+" + to_string(this->size), "", "");
        q3.make_code_shift_pointer();
        this -> ta_codes.push_back(q3);

        // return value stored
        if(type != "None") {
             quad q(this -> get_var_from_node(), "return_val","=", "");
            q.make_code_from_assignment();
            this -> ta_codes.push_back(q);
        }
        else {      // if no return value simply add a blank quad
            quad q("", "", "return_val", "");
            q.make_code_from_assignment();
            this -> ta_codes.push_back(q);
        }

         return;
    }
    ~FunctionCallNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};


class WhileStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    WhileStatementNode()
    {
        this->name = "While Statement";
        this->label = "While";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override {   
        this->next[0]->generate_quad();
        this->next[1]->generate_quad();
        int exp_size = this -> next[0] -> ta_codes.size();
        cout<<"exp_size: "<<exp_size<<endl;
        
        int stat_size = this -> next[1] -> ta_codes.size();
        cout<<"stat_size: "<<stat_size<<endl;
        this -> append_tac(this -> next[0]);

        string op = "if_false";
        string arg1 = this -> next[0] -> get_var_from_node();
        string arg2 = "J+" + to_string(stat_size + 2);
        quad q("", arg1, op, arg2);
        q.make_code_from_conditional();

        this -> ta_codes.push_back(q);

        for(int i = 0; i < this -> next[1] -> ta_codes.size(); i++){
            auto (&tac) = this -> next[1] -> ta_codes[i];
            if(tac.code == "\t\tgoto CONTINUE;\n"){
                int rel_jump = (i + this -> ta_codes.size());
                tac = quad("", "J-" + to_string(rel_jump), "goto", "");     // res, arg1, op, arg2
                tac.make_code_from_goto();
            }
            else if(tac.code == "\t\tgoto BREAK;\n") {
                int rel_jump = (this -> next[1] -> ta_codes.size() - i) + 1;
                tac = quad("", "J+" + to_string(rel_jump), "goto", "");     // res, arg1, op, arg2
                tac.make_code_from_goto();
            }
        }
        this -> append_tac(this -> next[1]);

        op = "goto";
        arg1 = "J-" + to_string(stat_size + exp_size + 1);
        quad q2("", arg1, op, "");
        q2.make_code_from_goto();

        this -> ta_codes.push_back(q2);
        return;}
    ~WhileStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class ForStatementNode : public AstNode
{
private:
    
    vector<AstNode *> next;

public:
    string id = "undefined";
    ForStatementNode(string id)
    {
        this->name = "For Statement";
        this->label = "for";
        this->id = id;
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { 
        int init_size = 0;
        int exp_size = 0;
 
        string op = "";
        string arg1 = "", arg2 = "";
        FunctionCallNode* func = dynamic_cast<FunctionCallNode*>(this->next[0]);
        Args* temp = dynamic_cast<Args*>(func->next[0]);
        temp->generate_quad();
        this->append_tac(temp);
if(temp->next.size()==1)  {
    arg1 = "0";
    quad q(this->id, arg1 , "=" , ""); //initialise
    init_size++;

    q.make_code_from_assignment();

    this->ta_codes.push_back(q);

    // temp->next[0]->generate_quad();
    // this -> append_tac(temp->next[0]);
    // if (temp->next[0]->label == "Identifier") {  
    //     IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(temp->next[0]);
    //     arg1 = temp1->value;
    // } else if (temp->next[0]->label == "float" || temp->next[0]->label == "int") {
    //     NumberNode* temp1 = dynamic_cast<NumberNode*>(temp->next[0]);
    //     arg1 = to_string(temp1->value);
    // } 
    // else if (temp->next[0]->label == "string") {
    //     StringNode* temp1 = dynamic_cast<StringNode*>(temp->next[0]);
    //     arg1 = temp1->value;
    // }
    // else if (temp->next[0]->label == "Function Call") {
    //     temp->next[0]->generate_quad();
    //     FunctionCallNode * temp1 = dynamic_cast<FunctionCallNode *>(temp->next[0]);
    //     arg1 = temp1->name;
    //     this->append_tac(temp->next[0]);
    // }
    // else if (temp->next[0]->label == "True" || temp->next[0]->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(temp->next[0]);
    //     arg1 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //     arg1 = temp->next[0]->get_var_from_node();
    // }
   
    arg1 = temp->next[0]->get_value();
    q = quad(this->next[0]->get_var_from_node(), this->id, "<", arg1);
    q.make_code_from_binary();
    init_size++;

    this -> ta_codes.push_back(q);

    this->next[1]->generate_quad();
    for(int i = 0; i < this -> next[1] -> ta_codes.size(); i++){
            auto (&tac) = this -> next[1] -> ta_codes[i];
            if(tac.code == "\t\tgoto CONTINUE;\n"){
                int rel_jump = (i + this -> ta_codes.size());
                tac = quad("", "J-" + to_string(rel_jump), "goto", "");     // res, arg1, op, arg2
                tac.make_code_from_goto();
            }
            else if(tac.code == "\t\tgoto BREAK;\n") {
                int rel_jump = (this -> next[1] -> ta_codes.size() - i) + 2;
                tac = quad("", "J+" + to_string(rel_jump), "goto", "");     // res, arg1, op, arg2
                tac.make_code_from_goto();
            }
        }
        // this -> append_tac(this -> next[1]);

    exp_size = this -> next[1] -> ta_codes.size();
    cout<<"exp_size: "<<exp_size<<endl;
         op = "if_false";
         arg1 = this -> next[0] -> get_var_from_node();
         arg2 = "J+" + to_string(this -> next[1] -> ta_codes.size() + 3);

         quad q1("", arg1, op, arg2);
         q1.make_code_from_conditional();
         this -> ta_codes.push_back(q1); 
         init_size++;

         this -> append_tac(this->next[1]);

      quad q3(this->id,this->id,"+","1");

      q3.make_code_from_binary();
      this -> ta_codes.push_back(q3);
      init_size++;

        op = "goto";
        arg1 = "J-" + to_string(exp_size + 3);
        quad q2("", arg1, op, "");
        q2.make_code_from_goto();

        this -> ta_codes.push_back(q2);
   
                // init_size = this->next[0]-> ta_codes . size();
                // this -> append_tac(this->next[0] -> ta_codes);   
                //add goto
}      

else if(temp->next.size()==2) {
        cout<<"Inside ForStatementNode : generate_quad()"<<endl;
    //   if (temp->next[0]->label == "Identifier") {  
    //     IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(temp->next[0]);
    //     arg1 = temp1->value;
    //   } else if (temp->next[0]->label == "float" || temp->next[0]->label == "int") {
    //     NumberNode* temp1 = dynamic_cast<NumberNode*>(temp->next[0]);
    //     arg1 = to_string(temp1->value);
    // } 
    // else if (temp->next[0]->label == "string") {
    //     StringNode* temp1 = dynamic_cast<StringNode*>(temp->next[0]);
    //     arg1 = temp1->value;
    // }
    // else if (temp->next[0]->label == "True" || temp->next[0]->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(temp->next[0]);
    //     arg1 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //     arg1 = temp->next[0]->get_var_from_node();
    // }
    //    temp->next[0]->generate_quad();
    // this -> append_tac(temp->next[0]);
    arg1 = temp->next[0]->get_value();
    quad q ("","","","");
    q = quad(this->id, arg1 , "=" , "");
   // cout << result << " " << arg1 << endl;
    q.make_code_from_assignment();
    ta_codes.push_back(q);
    init_size++;
//     q = quad(this->next[0]->get_var_from_node(), this->id, ">", arg1);
//     q.make_code_from_binary();
    
//    // this -> append_tac(this->right);
//    // this -> append_tac(this->left);
//     this -> ta_codes.push_back(q);

    // if (temp->next[1]->label == "Identifier") {  
    //     IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(temp->next[1]);
    //     arg1 = temp1->value;
    // } else if (temp->next[1]->label == "float" || temp->next[1]->label == "int") {
    //     NumberNode* temp1 = dynamic_cast<NumberNode*>(temp->next[1]);
    //     arg1 = to_string(temp1->value);
    // } 
    // else if (temp->next[1]->label == "string") {
    //     StringNode* temp1 = dynamic_cast<StringNode*>(temp->next[1]);
    //     arg1 = temp1->value;
    // }
    // else if (temp->next[1]->label == "True" || temp->next[1]->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(temp->next[1]);
    //     arg1 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //     arg1 = temp->next[1]->get_var_from_node();
    // }
    // temp->next[1]->generate_quad();
    // cout<<"Inside ForStatementNode : generate_quad() temp->next[1]->generate_quad();"<<endl;
    // this -> append_tac(temp->next[1]);
    arg1 = temp->next[1]->get_value();
    q = quad(this->next[0]->get_var_from_node(), this->id, "<", arg1);
    q.make_code_from_binary();
    init_size++;
   // this -> append_tac(this->right);
   // this -> append_tac(this->left);
    this -> ta_codes.push_back(q);
    cout<<"Inside ForStatementNode : generate_quad() statements generation"<<endl;
    this->next[1]->generate_quad();
    cout<<"Inside ForStatementNode : generate_quad() this->next[1]->generate_quad();"<<endl;

    for(int i = 0; i < this -> next[1] -> ta_codes.size(); i++){
            auto (&tac) = this -> next[1] -> ta_codes[i];
            if(tac.code == "\t\tgoto CONTINUE;\n"){
                int rel_jump = (i + this -> ta_codes.size());
                tac = quad("", "J-" + to_string(rel_jump), "goto", "");     // res, arg1, op, arg2
                tac.make_code_from_goto();
            }
            else if(tac.code == "\t\tgoto BREAK;\n") {
                int rel_jump = (this -> next[1] -> ta_codes.size() - i) + 2;
                tac = quad("", "J+" + to_string(rel_jump), "goto", "");     // res, arg1, op, arg2
                tac.make_code_from_goto();
            }
        }
       // this -> append_tac(this -> next[1]);

    exp_size = this -> next[1] -> ta_codes.size();
        string op = "if_false";
        string arg1 = this -> next[0] -> get_var_from_node();
        string arg2 = "J+" + to_string(this -> next[1] -> ta_codes.size() + 3);
        quad q1("", arg1, op, arg2);
        q1.make_code_from_conditional();
       this -> ta_codes.push_back(q1); 
       init_size++;
      this -> append_tac(this->next[1]);
      cout<<"Inside ForStatementNode : generate_quad() appended"<<endl;
      q = quad(this->id,this->id,"+","1");
      q.make_code_from_binary();
      this -> ta_codes.push_back(q);
      init_size++;

        op = "goto";
        arg1 = "J-" + to_string(exp_size + 3);
        quad q2("", arg1, op, "");
        q2.make_code_from_goto();

        this -> ta_codes.push_back(q2);
    
            } 
      
           
else {
    //    if (temp->next[0]->label == "Identifier") {  
    //     IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(temp->next[0]);
    //     arg1 = temp1->value;
    //   } else if (temp->next[0]->label == "float" || temp->next[0]->label == "int") {
    //     NumberNode* temp1 = dynamic_cast<NumberNode*>(temp->next[0]);
    //     arg1 = to_string(temp1->value);
    // } 
    // else if (temp->next[0]->label == "string") {
    //     StringNode* temp1 = dynamic_cast<StringNode*>(temp->next[0]);
    //     arg1 = temp1->value;
    // }
    // else if (temp->next[0]->label == "True" || temp->next[0]->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(temp->next[0]);
    //     arg1 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //     arg1 = temp->next[0]->get_var_from_node();
    // }
        temp->next[0]->generate_quad();
    this -> append_tac(temp->next[0]);
     arg1 = temp->next[0]->get_value();
    quad q ("","","","");
    q = quad(this->id, arg1 , "=" , "");
    q.make_code_from_assignment();
    init_size++;
     this -> ta_codes.push_back(q);
// if (temp->next[1]->label == "Identifier") {  
//         IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(temp->next[1]);
//         arg1 = temp1->value;
//     } else if (temp->next[1]->label == "float" || temp->next[1]->label == "int") {
//         NumberNode* temp1 = dynamic_cast<NumberNode*>(temp->next[1]);
//         arg1 = to_string(temp1->value);
//     } 
//     else if (temp->next[1]->label == "string") {
//         StringNode* temp1 = dynamic_cast<StringNode*>(temp->next[1]);
//         arg1 = temp1->value;
//     }
//     else if (temp->next[1]->label == "True" || temp->next[1]->label == "False") {
//         BoolNode* temp1 = dynamic_cast<BoolNode*>(temp->next[1]);
//         arg1 = temp1->label;
//         }
//     else {
//         // Handle error: unexpected label
//         arg1 = temp->next[1]->get_var_from_node();
//     }
    temp->next[1]->generate_quad();
    this -> append_tac(temp->next[1]);
 arg1 = temp->next[1]->get_value();
    q = quad(this->next[0]->get_var_from_node(), this->id, "<", arg1);
    q.make_code_from_binary();
    init_size++;
    this -> ta_codes.push_back(q);

    this->next[1]->generate_quad();

    for(int i = 0; i < this -> next[1] -> ta_codes.size(); i++){
            auto (&tac) = this -> next[1] -> ta_codes[i];
            if(tac.code == "\t\tgoto CONTINUE;\n"){
                int rel_jump = (i + this -> ta_codes.size());
                tac = quad("", "J-" + to_string(rel_jump), "goto", "");     // res, arg1, op, arg2
                tac.make_code_from_goto();
            }
            else if(tac.code == "\t\tgoto BREAK;\n") {
                int rel_jump = (this -> next[1] -> ta_codes.size() - i) + 2;
                tac = quad("", "J+" + to_string(rel_jump), "goto", "");     // res, arg1, op, arg2
                tac.make_code_from_goto();
            }
        }
      //  this -> append_tac(this -> next[1]);

    exp_size = this -> next[1] -> ta_codes.size();

        string op = "if_false";
        string arg1 = this -> next[0] -> get_var_from_node();
        string arg2 = "J+" + to_string(this -> next[1] -> ta_codes.size() + 3);
        quad q1("", arg1, op, arg2);
        q1.make_code_from_conditional();
       this -> ta_codes.push_back(q1); 
       init_size++;
      this -> append_tac(this->next[1]);

    
    // if (temp->next[2]->label == "Identifier") {  
    //     IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(temp->next[2]);
    //     arg1 = temp1->value;
    // } else if (temp->next[2]->label == "float" || temp->next[2]->label == "int") {
    //     NumberNode* temp1 = dynamic_cast<NumberNode*>(temp->next[2]);
    //     arg1 = to_string(temp1->value);
    // } 
    // else if (temp->next[2]->label == "string") {
    //     StringNode* temp1 = dynamic_cast<StringNode*>(temp->next[2]);
    //     arg1 = temp1->value;
    // }
    // else if (temp->next[2]->label == "True" || temp->next[2]->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(temp->next[2]);
    //     arg1 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //     arg1 = temp->next[2]->get_var_from_node();
    // }
    temp->next[2]->generate_quad();
    this -> append_tac(temp->next[2]);
     arg1 = temp->next[2]->get_value();
    
    q = quad(this->id,this->id,"+",arg1);
    q.make_code_from_binary();
     this -> ta_codes.push_back(q);
    init_size++;

        op = "goto";
        arg1 = "J-" + to_string(exp_size + 3);
        quad q2("", arg1, op, "");
        q2.make_code_from_goto();

        this -> ta_codes.push_back(q2);

            }    
   
    }
    ~ForStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class IfStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    IfStatementNode()
    {
        this->name = "if statement";
        this->label = "If";
    }

    void add(AstNode *node) override
    {   
        cout<<"In IfStatementNode.........."<< node->label<<endl;
        next.push_back(node);
    }

    void print() const override
    {   
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { 
        this -> next[0]->generate_quad();
        this -> next[1]->generate_quad();
       cout<<"In IfStatementNode generate_quad 0"<<endl; 
      if(next.size()==3)  this -> next[2]->generate_quad();
        this -> append_tac(this -> next[0]);
    cout<<"In IfStatementNode generate_quad 1"<<endl;
        string op = "if_false";
        string arg1 = this -> next[0] -> get_var_from_node();
        string arg2 = "J+" + to_string(this -> next[1] -> ta_codes.size() + 1);
        quad q("", arg1, op, arg2);
        q.make_code_from_conditional();
 cout<<"In IfStatementNode generate_quad 2"<<endl;
        this -> ta_codes.push_back(q);      // add the jump statement
        this -> append_tac(this -> next[1]);
    if(next.size()==3)    this -> append_tac(this -> next[2]);
         cout<<"In IfStatementNode generate_quad 3"<<endl;
       // return;
        }
    ~IfStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class ElIfStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    ElIfStatementNode(string nname)
    {
        this->name = nname;
        this->label = "ElIf";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { 
        //cout<< "ElIfStatementNode: "<<endl;
         this -> next[0]->generate_quad();
        this -> next[1]->generate_quad();
        this -> append_tac(this -> next[0]);

        string op = "if_false";
        string arg1 = this -> next[0] -> get_var_from_node();
        string arg2 = "J+" + to_string(this -> next[1] -> ta_codes.size() + 1);
        quad q("", arg1, op, arg2);
        q.make_code_from_conditional();

        this -> ta_codes.push_back(q);      // add the jump statement
        this -> append_tac(this -> next[1]);
      //  return;
      }
    ~ElIfStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class ElseStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    ElseStatementNode(string name)
    {
        this->name = name;
        this->label = "Else";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { 
        this->next[0]->generate_quad();
    //    string op = "goto";
    //    string  arg1 = "J+" + to_string(this -> next[0] -> ta_codes.size() + 1);
    //     quad q2("", arg1, op, "");
    //     q2.make_code_from_goto();

    //     this -> ta_codes.push_back(q2);
        this -> append_tac(this -> next[0]); 
    }
    ~ElseStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class assignmentStatement : public AstNode
{
private:
    vector<AstNode *> next;

public:
    assignmentStatement(const string &name, const string &label)
    {
        this->name = name;
        this->label = label;
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override
{
     for(auto (&child) : next)
        {
        child -> generate_quad();
        } 
    if (next.size() < 2) {
        // Handle error: not enough elements in 'next' vector
        cerr << "Error assignmentStatement: Not enough elements in 'next' vector." << endl;
        return;
    }
    cout<<"assignmentStatement: label "<<this->label<<endl;

    quad q("", "", "", "");
    string result;
    string arg1;

    // if (next[0]->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(next[0]);
    //     result = temp->value;
    // } else if (next[0]->label == "float" || next[0]->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(next[0]);
    //     result = to_string(temp->value);
    // } 
    // else if (next[0]->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(next[0]);
    //     result = temp->value;
    // }
    // else if (next[0]->label == "True" || next[0]->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(next[0]);
    //     result = temp1->label;
    // }       
    // else if(next[0]->label == "Array") {
    //     // next[0]->generate_quad();
    //     result = "*("+this->next[0]->get_var_from_node()+")";
    // }

    //  else {
    //     // Handle error: unexpected label
    //     result = this->next[0]->get_var_from_node();
    // }
     result = this->next[0]->get_value();

    if (next[1]->label == "Identifier") {  
        IdentifierNode* temp = dynamic_cast<IdentifierNode*>(next[1]);
        arg1 = temp->value;
    } else if (next[1]->label == "float" || next[1]->label == "int") {
        NumberNode* temp = dynamic_cast<NumberNode*>(next[1]);
        arg1 = to_string(temp->value);
    }
     else if (next[1]->label == "string") {
        StringNode* temp = dynamic_cast<StringNode*>(next[1]);
        arg1 = temp->value;
    } 
    else if (next[1]->label == "True" || next[1]->label == "False") {
        BoolNode* temp1 = dynamic_cast<BoolNode*>(next[1]);
        arg1 = temp1->label;
        }
    else if(next[1]->label == "Array") {
       // next[1]->generate_quad();
        arg1 = "*("+this->next[1]->get_var_from_node()+")";
    } 
    else if(next[1]->label == "List") {
       // next[1]->generate_quad();
       IdentifierNode* temp = dynamic_cast<IdentifierNode*>(next[1]);
       int size = temp->v.size()*8;

        quad q("", to_string(size), "push_param", "");
            q.make_code_push_param();
            this -> ta_codes.push_back(q);

        string arg = "alloc_mem";
        quad q2("", arg, "call_func", "");
        q2.make_code_from_func_call();
        this -> ta_codes.push_back(q2);

        result = this->get_var_from_node();

        quad q4(result, "return_value", "=" , "");
         q4.make_code_from_assignment();
        this -> ta_codes.push_back(q4);

        int offset = 0;
    for(const auto &val: temp->v){
            
            q = quad(this->next[1]->get_var_from_node(),result, "+",to_string(offset));
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);
            offset+=8;
            
            arg1 = "*("+this->next[1]->get_var_from_node()+")";
             quad q1(arg1, to_string(val), "=" , "");
   // cout << result << " " << arg1 << endl;
             q1.make_code_from_assignment();
             ta_codes.push_back(q1);

    }
      arg1 = this->get_var_from_node();
     result = this->next[0]->get_value();
     
     quad q5(result, arg1, "=", "");

    q5.make_code_from_assignment();
             ta_codes.push_back(q5);


        return;

    } 

    else {
        // Handle error: unexpected label
       // cerr << "Error assignmentStatement: Unexpected label for second element in 'next' vector." << endl;
       arg1 = this->next[1]->get_var_from_node();
    }
    if(this->label == "="){
    q = quad(result, arg1, this->label , "");
   // cout << result << " " << arg1 << endl;
    q.make_code_from_assignment();
    this -> append_tac(this->next[0]);
    this -> append_tac(this->next[1]);
    ta_codes.push_back(q);

    }
    else if(this->label == "+="){
           string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "+", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);

    }
    else if(this->label == "-="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "-", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
    else if(this->label == "/="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "/", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
    else if(this->label == "*="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "*", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
    else if(this->label == "%="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "%", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
    else if(this->label == "**="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "**", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
    else if(this->label == "&="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "&", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
    else if(this->label == "|="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "|", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
    }
    else if(this->label == "^="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "^", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
    else if(this->label == "<<="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, "<<", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
    else if(this->label == ">>="){
        string arg2 = arg1;
            arg1 = result;
            result = this -> get_var_from_node();

           // string arg1 = this -> children[0] -> get_var_from_node();
          //  this->label = op.substr(0, op.size() - 1);
    
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     this -> children[0] -> ta_codes.pop_back();
            // }
            this -> append_tac(this -> next[0]);
            this -> append_tac(this -> next[1]);

            quad q("", "", "", "");

            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "*()", "");
            //     q.make_code_from_load();
            // }
          //  else{
                // q = quad(result, arg1, "=", "");
                // q.make_code_from_assignment();
           // }
            //this -> ta_codes.push_back(q);

            q = quad(result, arg1, ">>", arg2);
            q.make_code_from_binary();
            this -> ta_codes.push_back(q);

          //  swap(result, arg1);
            // if(this -> children[0] -> name == "ArrayAccess" || this -> children[0] -> name == "FieldAccess" || this -> children[0] -> name == "#Name#"){
            //     q = quad(result, arg1, "", "");
            //     q.make_code_from_store();
            // }
            // else{
                q = quad(arg1, result, "=", "");
                q.make_code_from_assignment();
            // }
            this -> ta_codes.push_back(q);
        
    }
  
}

    ~assignmentStatement()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};




class TryStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    TryStatementNode()
    {
        this->name = "TryStatement";
        this->label = "try";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { return;}
    ~TryStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class ExceptStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    ExceptStatementNode()
    {
        this->name = "ExceptStatement";
        this->label = "except";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { return;}
    ~ExceptStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class withStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    withStatementNode()
    {
        this->name = "With Statement";
        this->label = "with";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { return;}
    ~withStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class matchStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    matchStatementNode()
    {
        this->name = "Match Statement";
        this->label = "Match";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { return;}
    ~matchStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class CaseStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    CaseStatementNode(string name)
    {
        this->name = name;
        this->label = "Case Statement";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\"" << label << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }
    void generate_quad() override { return;}
    ~CaseStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};



class ReturnStatementNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    ReturnStatementNode(AstNode *value)
    {
        this->name = "ReturnStatement";
        add(value);
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\""
             << "ReturnStatement"
             << "\"]" << endl;
        for (const auto &stmt : next)
        {
            cout << "\t" << name << " -> " << stmt->name << ";" << endl;
            stmt->print();
        }
    }

    void generate_quad() override { 
        string arg1 = "", op = "return";
        string arg2 = "";
        for(auto &child : this -> next) {
                child->generate_quad();
                this -> append_tac(child);
        }
        // if (this->next[0]->label == "Identifier") {  
        // IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(this->next[0]);
        // arg1 = temp1->value;
        // } 
        // else if (this->next[0]->label == "float" || this->next[0]->label == "int") {
        // NumberNode* temp1 = dynamic_cast<NumberNode*>(this->next[0]);
        // arg1 = to_string(temp1->value);
        // } 
        // else if (this->next[0]->label == "string") {
        // StringNode* temp1 = dynamic_cast<StringNode*>(this->next[0]);
        // arg1 = temp1->value;
        // }
        // else if (this->next[0]->label == "True" || this->next[0]->label == "False") {
        // BoolNode* temp1 = dynamic_cast<BoolNode*>(this->next[0]);
        // arg1 = temp1->label;
        // }
        // else {
        // // Handle error: unexpected label
        // arg1 = this->next[0]->get_var_from_node();
        //  }
         arg1 = this->next[0]->get_value();

        if(this -> next.size() == 1){
            quad q("", arg1, op, "");
            q.make_code_from_return();
            this -> ta_codes.push_back(q);
        }
        else{
        //     if (this->next[1]->label == "Identifier") {  
        // IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(this->next[1]);
        // arg2 = temp1->value;
        // } 
        // else if (this->next[1]->label == "float" || this->next[1]->label == "int") {
        // NumberNode* temp1 = dynamic_cast<NumberNode*>(this->next[1]);
        // arg2 = to_string(temp1->value);
        // } 
        // else if (this->next[1]->label == "string") {
        // StringNode* temp1 = dynamic_cast<StringNode*>(this->next[1]);
        // arg2 = temp1->value;
        // }
        // else if (this->next[1]->label == "True" || this->next[1]->label == "False") {
        // BoolNode* temp1 = dynamic_cast<BoolNode*>(this->next[1]);
        // arg1 = temp1->label;
        // }
        // else {
        // // Handle error: unexpected label
        // arg2 = this->next[1]->get_var_from_node();
        //  }
         arg2 = this->next[1]->get_value();
            quad q("", arg1+","+arg2, op, "");
            q.make_code_from_return();
            this -> ta_codes.push_back(q);
        }
        return;
        }
    ~ReturnStatementNode()
    {
        // delete returnValue;
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class yieldStatementNode : public AstNode
{
private:
    AstNode *yieldValue;

public:
    yieldStatementNode(AstNode *value)
        : yieldValue(value)
    {
        this->name = "yieldStatement";
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\""
             << "yieldStatement"
             << "\"]" << endl;
        cout << "\t" << name << " -> " << yieldValue->name << ";" << endl;
        yieldValue->print();
    }

    void generate_quad() override { return;}
    ~yieldStatementNode()
    {
        delete yieldValue;
    }
};

class globalStatementNode : public AstNode
{
private:
    AstNode *globalValue;

public:
    globalStatementNode(AstNode *value)
        : globalValue(value)
    {
        this->name = "GlobalStatement";
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\""
             << "GlobalStatement"
             << "\"]" << endl;
        cout << "\t" << name << " -> " << globalValue->name << ";" << endl;
        globalValue->print();
    }

    void generate_quad() override { return;}
    ~globalStatementNode()
    {
        delete globalValue;
    }
};

class nonLocalStatementNode : public AstNode
{
private:
    AstNode *nonLocalValue;

public:
    nonLocalStatementNode(AstNode *value)
        : nonLocalValue(value)
    {
        this->name = "nonLocalStatement";
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [label=\""
             << "nonLocalStatement"
             << "\"]" << endl;
        cout << "\t" << name << " -> " << nonLocalValue->name << ";" << endl;
        nonLocalValue->print();
    }

    void generate_quad() override { return;}
    ~nonLocalStatementNode()
    {
        delete nonLocalValue;
    }
};

class AdditionExpressionNode : public AstNode
{
private:
    const char operation = '+';
    AstNode *left;
    AstNode *right;

public:
    AdditionExpressionNode(AstNode *l, AstNode *r)
        : left(l), right(r) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else if (!right)
            right = node;
        else
            cerr << "Addition expression already has two children." << endl;
    }

    void print() const override
    {
        // cout << "\t"
        //      << name
        //      << " [label=\"" << operation << "\"]" << endl;
        // left->print();
        // cout << "\t"
        //      << name
        //      << " [label=\"" << operation << "\"]" << endl;
        // right->print();
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        cout << "\t" << name << " -> " << right->name << ";" << endl;
        right->print();
    }

    void generate_quad() override {
        this->left->generate_quad();
        
        this->right->generate_quad();
       
         quad q("", "", "", "");
            string result = this -> get_var_from_node();
            string arg1, arg2;

            
    // if (this->left->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->left);
    //     arg1 = temp->value;
    // } else if (this->left->label == "float" || this->left->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->left);
    //     arg1 = to_string(temp->value);
    // }
    // else if (this->left->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->left);
    //     arg1 = temp->value;
    // } 
    // else if (this->left->label == "True" || this->left->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(this->left);
    //     arg1 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //   arg1 = this->left->get_var_from_node();
    // }
     arg1 = this->left->get_value();

    // if (this->right->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->right);
    //     arg2 = temp->value;
    // } else if (this->right->label == "float" || this->right->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->right);
    //     arg2 = to_string(temp->value);
    // }
    // else if (this->right->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->right);
    //     arg2 = temp->value;
    // }
    // else if (this->right->label == "True" || this->right->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(this->right);
    //     arg2 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //     arg2 = this->right->get_var_from_node();
    // }

            arg2 = this->right->get_value();

    q = quad(result, arg1, "+", arg2);
    q.make_code_from_binary();
    
    this -> append_tac(this->right);
    this -> append_tac(this->left);
    this -> ta_codes.push_back(q);

        
        }
    ~AdditionExpressionNode()
    {
        delete left;
        delete right;
    }
};

class SubtractionExpressionNode : public AstNode
{
private:
    const char operation = '-';
    AstNode *left;
    AstNode *right;

public:
    SubtractionExpressionNode(AstNode *l, AstNode *r)
        : left(l), right(r) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else if (!right)
            right = node;
        else
            cerr << "Subtraction expression already has two children." << endl;
    }

    void print() const override
    {
        // cout << "\t"
        //      << "SubtractionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // left->print();
        // cout << "\t"
        //      << "SubtractionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // right->print();
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        cout << "\t" << name << " -> " << right->name << ";" << endl;
        right->print();
    }

    void generate_quad() override { 
         this->left->generate_quad();
        
        this->right->generate_quad();
       
         quad q("", "", "", "");
            string result = this -> get_var_from_node();
            string arg1, arg2;

            
    // if (this->left->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->left);
    //     arg1 = temp->value;
    // } else if (this->left->label == "float" || this->left->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->left);
    //     arg1 = to_string(temp->value);
    // }
    // else if (this->left->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->left);
    //     arg1 = temp->value;
    // }
    //  else if (this->left->label == "True" || this->left->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(this->left);
    //     arg1 = temp1->label;
    //     }
    //  else {
        
    //     // Handle error: unexpected label
    //   arg1 = this->left->get_var_from_node();
    // }
    
    arg1 = this->left->get_value();

    // if (this->right->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->right);
    //     arg2 = temp->value;
    // } else if (this->right->label == "float" || this->right->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->right);
    //     arg2 = to_string(temp->value);
    // }
    //  else if (this->right->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->right);
    //     arg2 = temp->value;
    // }
    //  else if (this->right->label == "True" || this->right->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(this->right);
    //     arg1 = temp1->label;
    //     }
    //  else {
    //     // Handle error: unexpected label
    //     arg2 = this->right->get_var_from_node();
    // }
            
   arg2 = this->right->get_value();
    q = quad(result, arg1, "-", arg2);
    q.make_code_from_binary();
    
    this -> append_tac(this->right);
    this -> append_tac(this->left);
    this -> ta_codes.push_back(q);

        }
    ~SubtractionExpressionNode()
    {
        delete left;
        delete right;
    }
};

class MultiplicationExpressionNode : public AstNode
{
private:
    const char operation = '*';
    AstNode *left;
    AstNode *right;

public:
    MultiplicationExpressionNode(AstNode *l, AstNode *r)
        : left(l), right(r) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else if (!right)
            right = node;
        else
            cerr << "Multiplication expression already has two children." << endl;
    }

    void print() const override
    {
        // cout << "\t"
        //      << "MultiplicationExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // left->print();
        // cout << "\t"
        //      << "MultiplicationExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // right->print();
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        cout << "\t" << name << " -> " << right->name << ";" << endl;
        right->print();
    }

    void generate_quad() override { 
         this->left->generate_quad();
        
        this->right->generate_quad();
       
         quad q("", "", "", "");
            string result = this -> get_var_from_node();
            string arg1, arg2;

            
    // if (this->left->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->left);
    //     arg1 = temp->value;
    // } else if (this->left->label == "float" || this->left->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->left);
    //     arg1 = to_string(temp->value);
    // }
    // else if (this->left->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->left);
    //     arg1 = temp->value;
    // } 
    // else if (this->left->label == "True" || this->left->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(this->left);
    //     arg1 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //   arg1 = this->left->get_var_from_node();
    // }
     arg1 = this->left->get_value();

    // if (this->right->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->right);
    //     arg2 = temp->value;
    // } else if (this->right->label == "float" || this->right->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->right);
    //     arg2 = to_string(temp->value);
    // }
    //  else if (this->right->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->right);
    //     arg2 = temp->value;
    // } 
    // else if (this->right->label == "True" || this->right->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(this->right);
    //     arg2 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //     arg2 = this->right->get_var_from_node();
    // }
     arg2 = this->right->get_value();
            

    q = quad(result, arg1, "*", arg2);
    q.make_code_from_binary();
    
    this -> append_tac(this->right);
    this -> append_tac(this->left);
    this -> ta_codes.push_back(q);

    }
    ~MultiplicationExpressionNode()
    {
        delete left;
        delete right;
    }
};

class DivisionExpressionNode : public AstNode
{
private:
    const char operation = '/';
    AstNode *left;
    AstNode *right;

public:
    DivisionExpressionNode(AstNode *l, AstNode *r)
        : left(l), right(r) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else if (!right)
            right = node;
        else
            cerr << "Division expression already has two children." << endl;
    }

    void print() const override
    {
        // cout << "\t"
        //      << "DivisionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // left->print();
        // cout << "\t"
        //      << "DivisionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // right->print();
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        cout << "\t" << name << " -> " << right->name << ";" << endl;
        right->print();
    }

    void generate_quad() override { 
         this->left->generate_quad();
        
        this->right->generate_quad();
       
         quad q("", "", "", "");
            string result = this -> get_var_from_node();
            string arg1, arg2;

            
    // if (this->left->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->left);
    //     arg1 = temp->value;
    // } else if (this->left->label == "float" || this->left->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->left);
    //     arg1 = to_string(temp->value);
    // }
    // else if (this->left->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->left);
    //     arg1 = temp->value;
    // }
    //  else if (this->left->label == "True" || this->left->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(this->left);
    //     arg1 = temp1->label;
    //     }
    //  else {
    //     // Handle error: unexpected label
    //   arg1 = this->left->get_var_from_node();
    // }
    arg1 = this->left->get_value();
    // if (this->right->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->right);
    //     arg2 = temp->value;
    // } else if (this->right->label == "float" || this->right->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->right);
    //     arg2 = to_string(temp->value);
    // }
    //  else if (this->right->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->right);
    //     arg2 = temp->value;
    // } 
    // else if (this->right->label == "True" || this->right->label == "False") {
    //     BoolNode* temp1 = dynamic_cast<BoolNode*>(this->right);
    //     arg2 = temp1->label;
    //     }
    // else {
    //     // Handle error: unexpected label
    //     arg2 = this->right->get_var_from_node();
    // }
          arg2 = this->right->get_value();   

    q = quad(result, arg1, "/", arg2);
    q.make_code_from_binary();
    
    this -> append_tac(this->right);
    this -> append_tac(this->left);
    this -> ta_codes.push_back(q);

        
    }
    ~DivisionExpressionNode()
    {
        delete left;
        delete right;
    }
};

class FloorDivisionExpressionNode : public AstNode
{
private:
    const string operation = "//";
    AstNode *left;
    AstNode *right;

public:
    FloorDivisionExpressionNode(AstNode *l, AstNode *r)
        : left(l), right(r) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else if (!right)
            right = node;
        else
            cerr << "Floor Division expression already has two children." << endl;
    }

    void print() const override
    {
        // cout << "\t"
        //      << "DivisionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // left->print();
        // cout << "\t"
        //      << "DivisionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // right->print();
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        cout << "\t" << name << " -> " << right->name << ";" << endl;
        right->print();
    }

    void generate_quad() override { 
         this->left->generate_quad();
        
        this->right->generate_quad();
       
         quad q("", "", "", "");
            string result = this -> get_var_from_node();
            string arg1, arg2;

            
    // if (this->left->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->left);
    //     arg1 = temp->value;
    // } else if (this->left->label == "float" || this->left->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->left);
    //     arg1 = to_string(temp->value);
    // }
    // else if (this->left->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->left);
    //     arg1 = temp->value;
    // } else {
    //     // Handle error: unexpected label
    //   arg1 = this->left->get_var_from_node();
    // }
     arg1 = this->left->get_value();

    // if (this->right->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->right);
    //     arg2 = temp->value;
    // } else if (this->right->label == "float" || this->right->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->right);
    //     arg2 = to_string(temp->value);
    // }
    // else if (this->right->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->right);
    //     arg2 = temp->value;
    // } else {
    //     // Handle error: unexpected label
    //     arg2 = this->right->get_var_from_node();
    // }
           arg2 = this->right->get_value();  

    q = quad(result, arg1, "//", arg2);
    q.make_code_from_binary();
    
    this -> append_tac(this->right);
    this -> append_tac(this->left);
    this -> ta_codes.push_back(q);

        
    }
    ~FloorDivisionExpressionNode()
    {
        delete left;
        delete right;
    }
};

class PowerExpressionNode : public AstNode
{
private:
    const string operation = "**";
    AstNode *left;
    AstNode *right;

public:
    PowerExpressionNode(AstNode *l, AstNode *r)
        : left(l), right(r) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else if (!right)
            right = node;
        else
            cerr << "Power expression already has two children." << endl;
    }

    void print() const override
    {
        // cout << "\t"
        //      << "DivisionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // left->print();
        // cout << "\t"
        //      << "DivisionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // right->print();
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        cout << "\t" << name << " -> " << right->name << ";" << endl;
        right->print();
    }

    void generate_quad() override { 
         this->left->generate_quad();
        
        this->right->generate_quad();
       
         quad q("", "", "", "");
            string result = this -> get_var_from_node();
            string arg1, arg2;

            
    // if (this->left->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->left);
    //     arg1 = temp->value;
    // } else if (this->left->label == "float" || this->left->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->left);
    //     arg1 = to_string(temp->value);
    // }
    // else if (this->left->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->left);
    //     arg1 = temp->value;
    // } else {
    //     // Handle error: unexpected label
    //   arg1 = this->left->get_var_from_node();
    // }
    arg1 = this->left->get_value();
    // if (this->right->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->right);
    //     arg2 = temp->value;
    // } else if (this->right->label == "float" || this->right->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->right);
    //     arg2 = to_string(temp->value);
    // }
    // else if (this->right->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->right);
    //     arg2 = temp->value;
    // } else {
    //     // Handle error: unexpected label
    //     arg2 = this->right->get_var_from_node();
    // }
     arg2 = this->right->get_value();
            

    q = quad(result, arg1, "**", arg2);
    q.make_code_from_binary();
    
    this -> append_tac(this->right);
    this -> append_tac(this->left);
    this -> ta_codes.push_back(q);

        
    }
    ~PowerExpressionNode()
    {
        delete left;
        delete right;
    }
};

class ModulusExpressionNode : public AstNode
{
private:
    const char operation = '%';
    AstNode *left;
    AstNode *right;

public:
    ModulusExpressionNode(AstNode *l, AstNode *r)
        : left(l), right(r) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else if (!right)
            right = node;
        else
            cerr << "Modulus expression already has two children." << endl;
    }

    void print() const override
    {
        // cout << "\t"
        //      << "DivisionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // left->print();
        // cout << "\t"
        //      << "DivisionExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        // right->print();
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        cout << "\t" << name << " -> " << right->name << ";" << endl;
        right->print();
    }

    void generate_quad() override { 
         this->left->generate_quad();
        
        this->right->generate_quad();
       
         quad q("", "", "", "");
            string result = this -> get_var_from_node();
            string arg1, arg2;

            
    // if (this->left->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->left);
    //     arg1 = temp->value;
    // } else if (this->left->label == "float" || this->left->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->left);
    //     arg1 = to_string(temp->value);
    // }
    // else if (this->left->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->left);
    //     arg1 = temp->value;
    // } else {
    //     // Handle error: unexpected label
    //   arg1 = this->left->get_var_from_node();
    // }
 arg1 = this->left->get_value();
    // if (this->right->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->right);
    //     arg2 = temp->value;
    // } else if (this->right->label == "float" || this->right->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->right);
    //     arg2 = to_string(temp->value);
    // } 
    // else if (this->right->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->right);
    //     arg2 = temp->value;
    // }else {
    //     // Handle error: unexpected label
    //     arg2 = this->right->get_var_from_node();
    // }
            
 arg2 = this->right->get_value();
    q = quad(result, arg1, "%", arg2);
    q.make_code_from_binary();
    
    this -> append_tac(this->right);
    this -> append_tac(this->left);
    this -> ta_codes.push_back(q);

        
    }
    ~ModulusExpressionNode()
    {
        delete left;
        delete right;
    }
};

// Composite node for representing binary expressions
class BinaryExpressionNode : public AstNode
{
private:
    string operation;
    AstNode *left;
    AstNode *right;

public:
    BinaryExpressionNode(string op, AstNode *l, AstNode *r)
        : operation(op), left(l), right(r) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else if (!right)
            right = node;
        else
            cerr << "Binary expression already has two children." << endl;
    }

    void print() const override
    {
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        // cout << "\t"
        //      << "BinaryExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << right->name << ";" << endl;
        right->print();
    }

    void generate_quad() override { 
        this->left->generate_quad();
        
        this->right->generate_quad();
        cout<<"BinaryExpressionNode:"<<endl;
         quad q("", "", "", "");
            string result = this -> get_var_from_node();
            string arg1, arg2;

            
    // if (this->left->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->left);
    //     arg1 = temp->value;
    // } else if (this->left->label == "float" || this->left->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->left);
    //     arg1 = to_string(temp->value);
    // } 
    // else if (this->left->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->left);
    //     arg1 = temp->value;
    // }
    // else if(this->left->label == "Array") {
    //    // next[1]->generate_quad();
    //     arg1 = "*("+this->left->get_var_from_node()+")";
    // }    
    // else {
    //     // Handle error: unexpected label
    //     arg1 = this->left->get_var_from_node();
    // }
     arg1 = this->left->get_value();

    // if (this->right->label == "Identifier") {  
    //     IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this->right);
    //     arg2 = temp->value;
    // } else if (this->right->label == "float" || this->right->label == "int") {
    //     NumberNode* temp = dynamic_cast<NumberNode*>(this->right);
    //     arg2 = to_string(temp->value);
    // }
    //  else if (this->right->label == "string") {
    //     StringNode* temp = dynamic_cast<StringNode*>(this->right);
    //     arg2 = temp->value;
    // }
    // else if(this->right->label == "Array") {
    //    // next[1]->generate_quad();
    //     arg2 = "*("+this->right->get_var_from_node()+")";
    // }  
    //  else {
    //     // Handle error: unexpected label
    //    arg2 = this->left->get_var_from_node();
    // }
             arg2 = this->right->get_value();

    q = quad(result, arg1, operation, arg2);
    q.make_code_from_binary();
    
    this -> append_tac(this->right);
    this -> append_tac(this->left);
    this -> ta_codes.push_back(q);
}
    ~BinaryExpressionNode()
    {
        delete left;
        delete right;
    }
};

class UnaryExpressionNode : public AstNode
{
private:
    string operation;
    AstNode *left;

public:
    UnaryExpressionNode(string op, AstNode *l)
        : operation(op), left(l) {}

    void add(AstNode *node) override
    {
        if (!left)
            left = node;
        else
            cerr << "Unary expression already has 1 child." << endl;
    }

    void print() const override
    {
        cout << "\t"
             << name
             << " [label=\"" << operation << "\"]" << endl;
        cout << "\t" << name << " -> " << left->name << ";" << endl;
        left->print();
        // cout << "\t"
        //      << "BinaryExpressionNode"
        //      << " [label=\"" << operation << "\"]" << endl;
    }

    void generate_quad() override { 
        this->left->generate_quad();
        this->append_tac(this->left);
       string result = this->get_var_from_node();
        string arg1 = this->left->get_value();

        quad q(result,operation,"("+arg1+")","");
        q.make_code_from_binary();
        this -> ta_codes.push_back(q);
        return;}
    ~UnaryExpressionNode()
    {
        delete left;
    }
};

class AST
{
private:
    AstNode *root = nullptr;

public:
    AST(AstNode *r) : root(r) {}

    void Print()
    {
        cout << "digraph G {" << endl;
        root->print();
        cout << "}" << endl;
    }

    ~AST()
    {
        if (root != nullptr)
        {
            delete root;
            root = nullptr;
        }
    }
}; 
#endif // NEW_AST_H
