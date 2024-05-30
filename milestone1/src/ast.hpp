#ifndef NEW_AST_H
#define NEW_AST_H

#include <iostream>
#include <vector>
using namespace std;

class AstNode
{
public:
    string name = "undefined";
    string label = "undefined";
    virtual void add(AstNode *node) = 0;
    virtual void print() const = 0;
    virtual ~AstNode() {}
};

class FunctionNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
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
    vector<AstNode *> next;

public:
    ClassNode(const string &name)
    {
        this->name = name;
        this->label = "Declare Class";
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

    ~ClassNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class IdentifierNode : public AstNode
{

public:
    string value = "undefined";
    IdentifierNode(string name, string label, string value)
    {
        this->name = name;
        this->label = label;
        this->value = value;
    }
    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << ": " << value << "\"]" << endl;
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
private:
    vector<AstNode *> next;

public:
    Args(const string &name)
    {
        this->name = name;
        this->label = "Arguments";
    }

    void add(AstNode *node) override
    {
        next.push_back(node);
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
    ~ControlFlowBlockNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

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
    ~ControlFlowBlockContentNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

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
    ~LoopBlockNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

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
    ~BlockNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

class StatementsNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
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

    ~StatementsNode()
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
    ~PassStatementNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
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
    ~BoolNode()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
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
    ForStatementNode()
    {
        this->name = "For Statement";
        this->label = "for";
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
    assignmentStatement(const string &name)
    {
        this->name = name;
        this->label = "assignment";
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

    ~assignmentStatement()
    {
        for (const auto &stmt : next)
        {
            delete stmt;
        }
    }
};

class StringNode : public AstNode
{
private:
    string value;

public:
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

    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << ": " << value << "\"]" << endl;
    }
};

class FunctionCallNode : public AstNode
{
private:
    vector<AstNode *> next;

public:
    FunctionCallNode(const string &name)
    {
        this->name = name;
        this->label = "Function Call";
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

    ~FunctionCallNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
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
    ~CaseStatementNode()
    {
        for (const auto &arg : next)
        {
            delete arg;
        }
    }
};

class NumberNode : public AstNode
{
private:
    int value;

public:
    NumberNode(string name, string label, int value)
    {
        this->name = name;
        this->label = label;
        this->value = value;
    }

    void add(AstNode * /*node*/) override
    {
        cerr << "Cannot add a child to a leaf node." << endl;
    }

    void print() const override
    {
        cout << "\t" << name << " [shape=box,label=\"" << label << ": " << value << "\"]" << endl;
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
    // void add(AstNode * /*node*/) override
    // {
    //     cerr << "Cannot add a child to a leaf node." << endl;
    // }

    // void print() const override
    // {
    //     cout << "\t" << name << " [label=\""
    //          << "ReturnStatement"
    //          << "\"]" << endl;
    //     cout << "\t" << name << " -> " << returnValue->name << ";" << endl;
    //     returnValue->print();
    // }

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
