#include "ast.hpp"
#include "make_tac.hpp"
#include<iostream>
#include<vector>
using namespace std;
string AstNode::get_value(){
      string arg = "";
    if (this->label == "Identifier") {  
        IdentifierNode* temp = dynamic_cast<IdentifierNode*>(this);
        if(temp->is_dotted){
            arg = this->get_var_from_node();
        }
        else arg = temp->value;
    } else if (this->label == "float" || this->label == "int") {
        NumberNode* temp = dynamic_cast<NumberNode*>(this);
        arg = to_string(temp->value);
    }
     else if (this->label == "string") {
        StringNode* temp = dynamic_cast<StringNode*>(this);
        arg = temp->value;
    }
    // else if(this->label == "Array") {
    //    // next[1]->generate_quad();
    //     arg = "*("+this->get_var_from_node()+")";
    // } 
    else if (this->label == "Function Call") {
        
        FunctionCallNode * temp = dynamic_cast<FunctionCallNode *>(this);
        arg = get_var_from_node();
        
    }
     else if (this->label == "True" || this->label == "False") {
        BoolNode* temp1 = dynamic_cast<BoolNode*>(this);
        if(temp1->label == "True")  arg = "1";
        else  arg = "0";
        
    
        }
     else {
        // Handle error: unexpected label
       arg = this->get_var_from_node();
    }
    return arg;
}
NumberNode::NumberNode(string name, string label, int value)
    {
        this->name = name;
        this->label = label;
        this->value = value;
    }

void NumberNode::add(AstNode * /*node*/) 
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

void NumberNode::print() const 
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << ": " << value << "\"]" << endl;
    }
vector<ClassNode*> ClassNode::allClasses;
ClassNode::ClassNode(const string &name)
    {
        this->name = name;
        this->label = "Declare Class";
        allClasses.push_back(this);
    }

void ClassNode::add(AstNode *node)  {
        next.push_back(node);
    }
void ClassNode::generate_quad() { 

        for(const auto &child: next){
        child->generate_quad();
        this->append_tac(child);
       

        }
        // for (size_t i = 1; i < next.size(); ++i)
        // {
        //     next[i]->generate_quad();
        //    this->append_tac(next[i]);
        // }
        
        }

void ClassNode::print() const {
        // cout << "\t" << name << " [label=\"" << label << " : " << name << "\"]" << endl;
        // vector<AstNode *>::iterator it;
        // for (const auto &item : next)
        // {
        //     cout << "\t" << name << " -> " << item->name << ";" << endl;
        //     item->print();
        // }

        cout << "\t" << name << " [label=\"" << label << " : " << name << "\"]" << endl;

        if (next.size() > 0)
        {
            AstNode *parentClass = next[0];
            cout << "\t" << name << " -> " << parentClass->name << " [label=\"\"];" << endl;
            parentClass->print();
        }

        for (size_t i = 1; i < next.size(); ++i)
        {
            cout << "\t" << name << " -> " << next[i]->name << ";" << endl;
            next[i]->print();
        }

    }
   // Function to find the parent class node by name within the next vector
 ClassNode* ClassNode::findParentClassNode(const std::string& parentClassName)
{ 
   // cout<<"in findParentClassNode for child "<<this->name<<" for parent "<<parentClassName<<endl;
    for (const auto& classNode : allClasses)
        {
         //   cout<<"in loop "<<classNode->name<<endl;
            if (classNode->name == parentClassName)
            { 
                //cout<<"found parent class"<<endl;
                return classNode;
            }
        }
        return nullptr;  // Parent class node not found in the allClasses vector
    

}


ClassNode::~ClassNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }