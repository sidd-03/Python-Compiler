%{
#include "ast.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

void yyerror(const char *);
extern FILE *yyin;
extern int yylineno;
extern int yylex();
extern int yyparse();
AstNode* root = NULL;
int n_nodes = 0;
%}

%union {
        char *strval;
        int intval;
        float floatval;
       AstNode* astNode;
}

%error-verbose



%token ID STRING ARRAY RETURN BREAK CONTINUE GLOBAL NONLOCAL YIELD DOTTED_ID
%token INDENT DEDENT INDENTERROR NEWLINE GT LT GTE LTE EQUAL NOT NOTEQUAL
%token INT FLOAT STR BOOL LIST INT_NUMBER FLOAT_NUMBER POWER FLOORDIV

%token<astNode> IF ELSE ELIF DEF WHILE FOR IN RANGE PRINT INPUT CLASS TRY EXCEPT MATCH CASE WITH AS PASS AND OR TRUE_TOK FALSE_TOK NONE_TOK LSHIFT RSHIFT 

%type<astNode> ID STRING RETURN BREAK CONTINUE GLOBAL NONLOCAL YIELD GT LT GTE LTE EQUAL NOT NOTEQUAL INT FLOAT STR BOOL LIST
%type<astNode> INT_NUMBER FLOAT_NUMBER boolean POWER FLOORDIV

%type<astNode> prog statements statement function assignment NUMBER function_block function_stmts function_stmt_ function_sp_stmt function_compound_stmt args args_ arg simple_stmt ARRAY start function_call argsp args_p argp argsc args_c argc return_stmt global_stmt nonlocal_stmt
%type<astNode> func_while_stmt func_for_stmt func_for_block func_for_stmts func_for_stmt_ relation_stmt for_sp_stmt range_args  for_compound_stmt
%type<astNode> func_if_stmt func_if_header func_elif_else_ func_elif_else func_else_stmt func_elif_stmts func_elif_stmt func_elif_header
%type<astNode> if_stmt if_header elif_else_ elif_else else_stmt elif_stmts elif_stmt elif_header  
%type<astNode> block stmts stmt compound_stmt pass class class_block class_stmts class_stmt_ class_sp_stmt
%type<astNode> while_stmt for_stmt for_block for_stmts for_stmt_ for_if_stmt for_if_header for_elif_else_ for_elif_else for_else_stmt for_elif_stmts for_elif_stmt for_elif_header
%type<astNode> try_except_stmt except_clauses except_clause match_stmt case_statements case_statement final_case match_block
%type<astNode> and_test not_test comparison expressions xor_expression and_expression shift_expression arith_expression term factor 

%right '='
%left '(' ')'
%left '|' '&' AND OR
%left '+' '-'
%left '*' '/'
%left '^'   // Bitwise operators are left-associative
%left LSHIFT RSHIFT
%left POWER
%left FLOORDIV
%left NOTEQUAL
%left EQUAL
%left GTE
%left LTE
%left '%'
%left NOT '~'
%nonassoc UMINUS

%%
start :
prog {
        root = $$;
    YYACCEPT;
};
prog : {$$ = NULL ;}
| statements {
    $$ = $1;
}
| NEWLINE statements {$$ = $2;}
;

statements : statement {$$=new StatementsNode("root"); $$->add($1);}
            | statements statement {$1->add($2); $$ = $1;}
            | statements NEWLINE {  }
;
statement  
        : if_stmt { $$ = $1; }
        | while_stmt { $$ = $1; }
        | for_stmt { $$ = $1; }
        | function { $$ = $1; }
        | function_call { $$ = $1; }
        | assignment NEWLINE { $$ = $1; }
        | class { $$ = $1; }
        | try_except_stmt { $$ = $1; }
        | match_stmt { $$ = $1; }
        | pass NEWLINE { $$ = $1; }
;
NUMBER : INT_NUMBER { std::string nname = "int" + std::to_string(n_nodes);
                                ++n_nodes;
                                $1->name=nname;
                                $$ = $1; }
        | FLOAT_NUMBER { std::string nname = "float" + std::to_string(n_nodes);
                                ++n_nodes;
                                $1->name=nname;
                                $$ = $1; }
;

boolean : TRUE_TOK {  std::string nname = "bool" + std::to_string(n_nodes);
                                ++n_nodes;
                                $1 = new BoolNode();
                                $1->name=nname;
                                $1->label = "True";
                                $$ = $1; }
        | FALSE_TOK { std::string nname = "bool" + std::to_string(n_nodes);
                                ++n_nodes;
                                $1 = new BoolNode();
                                $1->name=nname;
                                $1->label = "False";
                                $$ = $1; }
        /* | NONE_TOK { std::string nname = "bool" + std::to_string(n_nodes);
                                ++n_nodes;
                                $1 = new BoolNode();
                                $1->name=nname;
                                $1->label = "None";
                                $$ = $1; } */
;

if_stmt
        : if_header block elif_else_ {
                 std::string nname = "if" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new IfStatementNode();
                $$->name = nname;
                $$->add($1);
                $$->add($2);
                if ($3 != NULL)
                    $$->add($3);
        }
;
if_header
        : IF relation_stmt { $$ = $2; }
;
elif_else_
        :  {$$ = NULL;}
        | elif_else { $$ = $1; }
;

elif_else
        : elif_stmts else_stmt { std::string nname = "block" + std::to_string(n_nodes);
                ++n_nodes;
                $$=new BlockNode(nname);
                $$->label = "Elif..._Else";
                $$->add($1); 
                $$->add($2); }
        | elif_stmts { $$ = $1; }
        | else_stmt { $$ = $1;  }
;

else_stmt
        : ELSE block { std::string nname = "else" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ElseStatementNode(nname);
                $$->add($2); }
;

elif_stmts  
        : elif_stmt {
                std::string nname = "block" + std::to_string(n_nodes);
                ++n_nodes; 
                $$=new BlockNode(nname);
                $$->label = "Elif..."; 
                $$->add($1); 
        }
        | elif_stmts elif_stmt { $1->add($2); $$ = $1; }
;
elif_stmt   
        : elif_header block {
                std::string nname = "elif" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ElIfStatementNode(nname);
                $$->add($1);
                $$->add($2);
        }
;

elif_header 
        : ELIF relation_stmt { $$ = $2; }

;

function    
        : DEF ID '(' args ')' function_block {
        //printf("Function successfully parsed:\n"); 
                std::string name = "func" + std::to_string(n_nodes);
                ++n_nodes;
                IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
                $$ = new FunctionNode(idFunc->value);
                if ($4 != NULL)
                $$->add($4);
                $$->add($6);
        }
        |DEF ID '(' args ')' ':'function_block {
        //printf("Function successfully parsed:\n"); 
                std::string name = "func" + std::to_string(n_nodes);
                ++n_nodes;
                IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
                $$ = new FunctionNode(idFunc->value);
                if ($4 != NULL)
                    $$->add($4);
                $$->add($7);
        }
        | DEF ID '(' args ')' '-' GT data_type function_block {
            //printf("Generic Function successfully parsed:\n");
                std::string name = "func" + std::to_string(n_nodes);
                ++n_nodes;
                IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
                $$ = new FunctionNode(idFunc->value + "_Generic");
                if ($4 != NULL)
                    $$->add($4);
                $$->add($9);
        }
        | DEF ID '(' args ')' '-' GT data_type ':' function_block {
            //printf("Generic Function successfully parsed:\n");
                std::string name = "func" + std::to_string(n_nodes);
                ++n_nodes;
                IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
                $$ = new FunctionNode(idFunc->value + "_Generic");
                if ($4 != NULL)
                    $$->add($4);
                $$->add($10);
        }
;

function_block
            : NEWLINE INDENT function_stmts DEDENT { $$ = $3; }
;

function_stmts   
            : function_stmt_    { 
                                        std::string nname = "stmt" + std::to_string(n_nodes);
                                        ++n_nodes;
                                        $$ = new StatementsNode(nname); 
                                        $$->add($1); 
                                }

            | function_stmts function_stmt_ { $1->add($2); $$ = $1; }
;

function_stmt_   
            : simple_stmt NEWLINE { 
                $$ = $1;         
            }

            | function_compound_stmt { 
                $$ = $1;        
            }

            | function_sp_stmt NEWLINE { 
                $$ = $1;            
            }
;

function_sp_stmt 
            : global_stmt{ $$ = $1; }
            | nonlocal_stmt{ $$ = $1; }
            
;

function_compound_stmt
        : func_if_stmt{ $$ = $1; }
        | func_while_stmt{ $$ = $1; }
        | func_for_stmt{ $$ = $1; }
        | try_except_stmt { $$ = $1; }
        | match_stmt { $$ = $1; }
        | function{ $$ = $1; }
;

func_while_stmt
        : WHILE relation_stmt func_for_block { 
            //printf("while statement successfully parsed:\n");
                std::string name = "while" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new WhileStatementNode();
                $$->name = name;
                $$->add($2);
                $$->add($3);
        }

;

func_for_stmt    
        : FOR ID IN RANGE '(' range_args ')' func_for_block { 
            //printf("for statement successfully parsed:\n");
                std::string name = "for" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "range" + std::to_string(n_nodes);
                ++n_nodes;
                $4 = new FunctionCallNode(nname);
                $4->add($6);
                $$ = new ForStatementNode();
                $$->name = name;
                $$->add($4);
                $$->add($8);
        }
        | FOR ID IN ID ARRAY func_for_block { 
            //printf("for statement successfully parsed:\n"); 
        }
;

func_for_block
        : NEWLINE INDENT func_for_stmts DEDENT { $$ = $3; }
;

func_for_stmts   
        : func_for_stmt_ {
                std::string nname = "stmt" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new StatementsNode(nname); 
                $$->add($1); 
        }

        | func_for_stmts func_for_stmt_ { $1->add($2); $$ = $1; }
;

func_for_stmt_   
        : simple_stmt NEWLINE { $$ = $1; }
        | for_compound_stmt { $$ = $1; }
        | for_sp_stmt NEWLINE { $$ = $1; }
        | function_sp_stmt NEWLINE { $$ = $1; }
;

func_if_stmt     
        : func_if_header function_block func_elif_else_  { 
                //printf("if statement successfully parsed:\n");
                std::string nname = "if" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new IfStatementNode();
                $$->name = nname;
                $$->add($1);
                $$->add($2);
                if ($3 != NULL)
                    $$->add($3);
            }                     
;

func_if_header
        : IF relation_stmt { $$ = $2; }

;

func_elif_else_  
        :     { $$ = NULL; }
        | func_elif_else { $$ = $1; }
;

func_elif_else   
        : func_elif_stmts func_else_stmt {
                std::string nname = "block" + std::to_string(n_nodes);
                ++n_nodes;
                $$=new BlockNode(nname);
                $$->label = "Elif..._Else";
                $$->add($1); 
                $$->add($2); 
        }
        | func_elif_stmts { $$ = $1; }
        | func_else_stmt { $$ = $1; }
;

func_else_stmt   
        : ELSE  function_block {
                std::string nname = "else" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ElseStatementNode(nname);
                $$->add($2);
        }
;

func_elif_stmts  
        : func_elif_stmt {
                std::string nname = "block" + std::to_string(n_nodes);
                ++n_nodes; 
                $$=new BlockNode(nname);
                $$->label = "Elif..."; 
                $$->add($1); 
        }
        | func_elif_stmts func_elif_stmt { $1->add($2); $$ = $1; }
;

func_elif_stmt   
        : func_elif_header function_block {
                std::string nname = "elif" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ElIfStatementNode(nname);
                $$->add($1);
                $$->add($2);
        }
;

func_elif_header 
        : ELIF relation_stmt { $$ = $2; }

;
args    
    : /* empty params */ { $$ = NULL; }
    | args_  { $$ = $1; }
;


args_ 
    : arg{ 
                std::string nname = "arg" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new Args(nname); $$->add($1); 
        }
    | args_ ',' arg { $1->add($3); $$ = $1; }
;



arg   
    : ID { 
        std::string nname = "iden" + std::to_string(n_nodes);
        ++n_nodes;
        $1->name=nname;
        $$ = $1;
    }

    | ID ':' data_type { 
        std::string nname = "iden" + std::to_string(n_nodes);
        ++n_nodes;
        $1->name=nname;
        $$ = $1;
    }
;

//! args for print function (AST)
argsp  
    : /* empty params */ { $$ = NULL; }
    | args_p { $$ = $1; }
;


args_p
    : argp {
                std::string nname = "arg" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new Args(nname); $$->add($1); 
        }

    | args_p  argp { $1->add($2); $$ = $1; }
;



argp  
    :  arith_expression {$$ = $1;}
;

//! args for function call (AST)
argsc  
    : /* empty params */ { $$ = NULL; }
    | args_c { $$ = $1; }
;


args_c
    : argc {
                std::string nname = "arg" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new Args(nname); $$->add($1); 
        }

    | args_c ',' argc { $1->add($3); $$ = $1; }
;

argc  
    :  arith_expression {$$ = $1;}

;

while_stmt
        : WHILE relation_stmt for_block { 
            //printf("while statement successfully parsed:\n");
                std::string name = "while" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new WhileStatementNode();
                $$->name = name;
                $$->add($2);
                $$->add($3);
        }
;
range_args   
        : arith_expression {
                std::string nname = "arg" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new Args(nname); 
                $$->add($1); 
        }

        | arith_expression ','arith_expression {
                std::string nname = "arg" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new Args(nname); 
                $$->add($1); 
                $$->add($3); 
        }

        | arith_expression ',' arith_expression ','arith_expression {
                std::string nname = "arg" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new Args(nname); 
                $$->add($1); 
                $$->add($3); 
                $$->add($5); 
        }
;

for_stmt    
        : FOR ID IN RANGE '(' range_args ')' for_block { 
            //printf("for statement successfully parsed:\n");
                std::string name = "for" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "range" + std::to_string(n_nodes);
                ++n_nodes;
                $4 = new FunctionCallNode(nname);
                $4->add($6);
                $$ = new ForStatementNode();
                $$->name = name;
                $$->add($4);
                $$->add($8);
        }

        | FOR ID IN ID ARRAY for_block { 
            printf("for statement successfully parsed:\n"); 
        }
;

for_block
        : NEWLINE INDENT for_stmts DEDENT { $$ = $3; }
;

for_stmts   
        : for_stmt_ {
                std::string nname = "stmt" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new StatementsNode(nname); 
                $$->add($1);
        }

        | for_stmts for_stmt_ { $1->add($2); $$ = $1; }
;

for_stmt_   
        : simple_stmt NEWLINE {  $$ = $1; }
        | for_compound_stmt {  $$ = $1; }
        | for_sp_stmt NEWLINE {  $$ = $1; }
;

for_sp_stmt 
        : BREAK {
                std::string nname = "break" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new BreakStatementNode(); 
                $$->name = nname;
        }

        | CONTINUE {
                std::string nname = "continue" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ContinueStatementNode(); 
                $$->name = nname;
        }
;

for_compound_stmt
        : for_if_stmt {  $$ = $1; }
        | while_stmt {  $$ = $1; }
        | for_stmt {  $$ = $1; }
        | try_except_stmt {  $$ = $1; }
        | match_stmt { $$ = $1; }
;

for_if_stmt     
        : for_if_header for_block for_elif_else_  { 
                //printf("if statement successfully parsed:\n");
                std::string nname = "if" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new IfStatementNode();
                $$->name = nname;
                $$->add($1);
                $$->add($2);
                if ($3 != NULL)
                    $$->add($3);
            }                     
;

for_if_header   
        : IF relation_stmt { $$ = $2; }

;

for_elif_else_  
        :     { $$ = NULL; }
        | for_elif_else { $$ = $1; }
;

for_elif_else   
        : for_elif_stmts for_else_stmt {
                std::string nname = "block" + std::to_string(n_nodes);
                ++n_nodes;
                $$=new BlockNode(nname);
                $$->label = "Elif..._Else";
                $$->add($1); 
                $$->add($2); 
        }
        | for_elif_stmts { $$ = $1; }
        | for_else_stmt { $$ = $1; }
;

for_else_stmt   
        : ELSE  for_block {
                std::string nname = "else" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ElseStatementNode(nname);
                $$->add($2);
        }
;

for_elif_stmts  
        : for_elif_stmt {
                std::string nname = "block" + std::to_string(n_nodes);
                ++n_nodes; 
                $$=new BlockNode(nname);
                $$->label = "Elif..."; 
                $$->add($1); 
        }
        | for_elif_stmts for_elif_stmt { $1->add($2); $$ = $1; }
;

for_elif_stmt   
        : for_elif_header for_block {
                std::string nname = "elif" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ElIfStatementNode(nname);
                $$->add($1);
                $$->add($2);
        }
;

for_elif_header 
        : ELIF relation_stmt { $$ = $2; }
 
;

function_call     
            : ID '(' argsc ')' {
                std::string name = "_func_call" + std::to_string(n_nodes);
                ++n_nodes;
                IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($1);
                $$ = new FunctionCallNode(idFunc->value + name);
                if ($3 != NULL)
                    $$->add($3);
            }

            | PRINT '(' argsp ')' {
                std::string name = "print" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new FunctionCallNode(name);
                if ($3 != NULL)
                    $$->add($3);
            }

            | INPUT '(' STRING ')' {
                std::string name = "input" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new FunctionCallNode(name);
                std::string nname = "string" + std::to_string(n_nodes);
                ++n_nodes;
                $3->name=nname;
                $$->add($3);
            }
;

class
    : CLASS ID class_block { 
        //printf("class statement successfully parsed:\n");
        std::string name = "class" + std::to_string(n_nodes);
        ++n_nodes;
        IdentifierNode* idclass = dynamic_cast<IdentifierNode*>($2);
        $$ = new ClassNode(idclass->value);
        $$->add($3);
    }
    | CLASS ID '(' ID ')' class_block { 
        //printf("class statement successfully parsed:\n");
        std::string name = "class" + std::to_string(n_nodes);
        ++n_nodes;
        IdentifierNode* idclass = dynamic_cast<IdentifierNode*>($2);
        $$ = new ClassNode(idclass->value);
        $$->add($6);
    }
;

class_block
        : NEWLINE INDENT class_stmts DEDENT { $$ = $3; }
;

class_stmts   
        : class_stmt_ {
            std::string nname = "stmt" + std::to_string(n_nodes);
            ++n_nodes;
            $$ = new StatementsNode(nname); 
            $$->add($1); 
        }
        | class_stmts class_stmt_ { $1->add($2); $$ = $1; }
;

class_stmt_   
        : simple_stmt NEWLINE { $$ = $1; }
        | compound_stmt { $$ = $1; }
        | class_sp_stmt NEWLINE { $$ = $1; }
        | function { $$ = $1; }
;

class_sp_stmt 
            : global_stmt { $$ = $1; }
;  

try_except_stmt 
            : TRY block except_clauses { 
                //printf("try_except statement successfully parsed:\n");
                std::string name = "try" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new TryStatementNode();
                $$->name = name;
                $$->add($2);
                $$->add($3);
            }
;

except_clauses    
            : except_clause {
                std::string nname = "block" + std::to_string(n_nodes);
                ++n_nodes; 
                $$=new BlockNode(nname);
                $$->label = "Except..."; 
                $$->add($1); 
            }
            | except_clauses except_clause { $1->add($2); $$ = $1; }
;

except_clause 
            : EXCEPT block {
                std::string nname = "except" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ExceptStatementNode();
                $$->name = nname;
                $$->add($2);
            }
;
match_stmt   
        : MATCH ID match_block { 
            //printf("match statement successfully parsed:\n");
            std::string name = "match" + std::to_string(n_nodes);
            ++n_nodes;
            std::string nname = "iden" + std::to_string(n_nodes);
            ++n_nodes;
            $2->name=nname;
            $$ = new matchStatementNode();
            $$->name = name;
            $$->add($2);
            $$->add($3);
        }
;

case_statements   
            :
            case_statement {
                std::string nname = "block" + std::to_string(n_nodes);
                ++n_nodes; 
                $$=new BlockNode(nname);
                $$->label = "Case..."; 
                $$->add($1); 
            }
            | case_statements case_statement { $1->add($2); $$ = $1; }
;

case_statement    
            : CASE argp block {
                std::string nname = "case" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new CaseStatementNode(nname);
                $$->add($2);
                $$->add($3);
            }
;

final_case  
        : CASE '_' block {
            std::string nname = "case" + std::to_string(n_nodes);
            ++n_nodes;
            $$ = new CaseStatementNode(nname);
            $$->label = "Default Statement";
            $$->add($3);
        }
;

match_block 
        : NEWLINE INDENT case_statements final_case DEDENT {
            std::string nname = "block" + std::to_string(n_nodes);
            ++n_nodes; 
            $$=new BlockNode(nname);
            $$->label = "Match Block"; 
            $$->add($3);
            $$->add($4);
        }
;

block
        :NEWLINE INDENT stmts DEDENT { $$ = $3; }
;
stmts 
    : stmt {
            std::string nname = "stmt" + std::to_string(n_nodes);
            ++n_nodes;
            $$ = new StatementsNode(nname); 
            $$->add($1); 
    }
    | stmts stmt { $1->add($2); $$ = $1; }
;

stmt  
    : simple_stmt NEWLINE { $$ = $1; }
    | compound_stmt { $$ = $1; }
;
simple_stmt 
        : assignment { $$ = $1; }
        | function_call { $$ = $1; }
        | pass { $$ = $1; }
        | return_stmt {$$ = $1;}
//////// Doubtin return statement comment
;
compound_stmt
        : if_stmt    { $$ = $1; }
        | while_stmt { $$ = $1; }
        | for_stmt   { $$ = $1; }
        | try_except_stmt { $$ = $1; }
        | match_stmt { $$ = $1; }
;
assignment
        : ID ':' data_type '=' arith_expression  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($5);
               
              //  cout<<"here"<<endl;
        }
        | ID ':' data_type '=' NONE_TOK  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($5);
              //  cout<<"here"<<endl;
        }
        | ID '=' arith_expression  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);       
                $$->add($1);
                $$->add($3);
        }

        | ID '+' '=' arith_expression  {
                std::string name = "plusequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($4);
                $$->add($1);
                
               
        }

        | ID '-' '=' arith_expression  {
                std::string name = "minusequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }

        | ID '*' '=' arith_expression  {
                std::string name = "multiequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }

        | ID '/' '=' arith_expression  {
                std::string name = "divequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
              
                $$->add($4);
        }
        | ID '%' '=' arith_expression  {
                std::string name = "modequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }
        | ID POWER '=' arith_expression  {
                std::string name = "powerequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }
        | ID '&' '=' arith_expression  {
                std::string name = "andequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }
        | ID '|' '=' arith_expression  {
                std::string name = "orequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }
        | ID '^' '=' arith_expression  {
                std::string name = "xorequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }
        | ID LSHIFT '=' arith_expression  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }
        | ID RSHIFT '=' arith_expression  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
        }
        /* | ID ARRAY '=' ID ARRAY      {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                
                $$->add($4);
                $$->add($1);
        } */
        | ID ARRAY '=' arith_expression   {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($4);
                
        }
        | ID ':' data_type '=' ARRAY   {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name);
                $$->add($1);
                $$->add($5);
        }
;

return_stmt
        : RETURN arith_expression {
                std::string nname = "return" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ReturnStatementNode($2);
                $$->name = nname;
        }
        
        
        | RETURN arith_expression ',' arith_expression {
                std::string nname = "return" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ReturnStatementNode($2);
                $$->add($4);
                $$->name = nname;
        }

        
;


global_stmt
        : GLOBAL ID {
                std::string nname = "global" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $2->name=name;
                $$ = new globalStatementNode($2);
                $$->name = nname;
        }
;

nonlocal_stmt
        : NONLOCAL ID {
                std::string nname = "nonlocal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $2->name=name;
                $$ = new nonLocalStatementNode($2);
                $$->name = nname;
        }
;

pass
        : PASS {
                std::string nname = "pass" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new PassStatementNode();
                $$->name = nname;
        }
;
relation_stmt : relation_stmt OR and_test {
                std::string nname = "or" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "OR" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=name;
                $$ = new BinaryExpressionNode("OR", $1, $3);
                $$->name=nname;
}
| and_test
;

and_test : and_test AND not_test {
         std::string nname = "and" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "AND" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=name;
                $$ = new BinaryExpressionNode("AND", $1, $3);
                $$->name=nname;
}
| not_test
;
not_test : NOT not_test {
               std::string nname = "not" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new UnaryExpressionNode("NOT", $2);
                $$->name=nname;
}
| comparison {$$ = $1}
;
 
comparison
        : comparison GT expressions    {
                std::string nname = "gt" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=name;
                $$ = new BinaryExpressionNode(">", $1, $3);
                $$->name=nname;
        }

        | comparison LT expressions    {
                std::string nname = "lt" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=name;
                $$ = new BinaryExpressionNode("<", $1, $3);
                $$->name=nname;
        }

        | comparison GTE expressions   {
                std::string nname = "gte" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=name;
                $$ = new BinaryExpressionNode(">=", $1, $3);
                $$->name=nname;
        }

        | comparison LTE expressions   {
                std::string nname = "lte" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=name;
                $$ = new BinaryExpressionNode("<=", $1, $3);
                $$->name=nname;
        }

        | comparison EQUAL expressions {
                std::string nname = "equal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=name;
                $$ = new BinaryExpressionNode("==", $1, $3);
                $$->name=nname;
        }

        | comparison NOTEQUAL expressions {
                std::string nname = "notequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string name = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=name;
                $$ = new BinaryExpressionNode("!=", $1, $3);
                $$->name=nname;
        }

        | expressions {
                $$ = $1;
        }
;
expressions : expressions '|' xor_expression { 
         std::string nname = "or" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new BinaryExpressionNode("OR", $1, $3);
                $$->name=nname;
          }
| xor_expression { $$ = $1; }
;

xor_expression : xor_expression '^' and_expression {
                std::string nname = "xor" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new BinaryExpressionNode("XOR", $1, $3);
                $$->name=nname;
}
| and_expression {$$ = $1;}
;

and_expression : and_expression '&' shift_expression {
                std::string nname = "and" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new BinaryExpressionNode("AND", $1, $3);
                $$->name=nname;
}
| shift_expression { $$ = $1; }
;

shift_expression : shift_expression LSHIFT arith_expression {
          std::string nname = "leftshift" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new BinaryExpressionNode("LSHIFT", $1, $3);
                $$->name=nname;
}
| shift_expression RSHIFT arith_expression {
          std::string nname = "rightshift" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new BinaryExpressionNode("RSHIFT", $1, $3);
                $$->name=nname;
}
| arith_expression {$$ = $1;}

;

arith_expression : arith_expression '+' term {
           std::string nname = "add" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new AdditionExpressionNode($1, $3);
                $$->name=nname;
}
| arith_expression '-' term {
         std::string nname = "sub" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new SubtractionExpressionNode($1, $3);
                $$->name=nname;
}
| term {$$ = $1;}
;
term : term '*' factor {
         std::string nname = "multi" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new MultiplicationExpressionNode($1, $3);
                $$->name=nname;
}
| term '/' factor {
        std::string nname = "divide" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new DivisionExpressionNode($1, $3);
                $$->name=nname;
}
| term '%' factor {
         std::string nname = "modulus" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new ModulusExpressionNode($1, $3);
                $$->name=nname;
}
| term FLOORDIV factor {
          std::string nname = "power" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new FloorDivisionExpressionNode($1, $3);
                $$->name=nname;
}
| term POWER factor {
        std::string nname = "power" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new PowerExpressionNode($1, $3);
                $$->name=nname;

}
| factor {$$ = $1;}
;

factor : '(' relation_stmt ')'           { $$ = $2; }
        | ID                            { 
                                                std::string nname = "iden" + std::to_string(n_nodes);
                                                ++n_nodes;
                                                $1->name=nname;
                                                $$ = $1;
                                        }
        | ID ARRAY        { 
                                                std::string nname = "array" + std::to_string(n_nodes);
                                                ++n_nodes;
                                                $1->name=nname;
                                                $$ = $1;
                                        }   
        | NUMBER                        { $$ = $1; }

        | boolean                       { $$ = $1; }

        | STRING                        {
                                                std::string nname = "string" + std::to_string(n_nodes);
                                                ++n_nodes;
                                                $1->name=nname;
                                                $$ = $1; 
                                        }
        | function_call {
                $$ = $1;
        }  
        | '~' factor    {
                std::string nname = "not" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new UnaryExpressionNode("NOT", $2);
                $$->name=nname;
        }          
;

data_type   
        : INT
        | FLOAT
        | BOOL
        | STR
        | NONE_TOK
        | LIST ARRAY
        | ID

;

%%

int main(int argc, char **argv)
{
        int input_flag =0, output_flag = false;
        ofstream output_file; // Declare an output file stream
        streambuf *coutbuf = cout.rdbuf();
        for (int i=0; i< argc; i++)
        {

                if (strcmp(argv[i], "-help") == 0)
                {
                        cerr<<"Execution: ./a.out [-input <filename>] [-output <filename>]\n";
                        cerr<<"Sample: ./a.out -input input.txt -output output.dot\n";
                        cerr<<"The default input file is input.py and the default output file is output.dot\n";
                        cerr<<"The -verbose tag would give the detailed parser output \n";
                        cerr<<"To visualise AST: dot -Tpdf <outputfilename.dot> -o graphfilename.pdf\n";
                        cerr<<"Sample: dot -Tpdf output.dot -o output.pdf\n";
                }

                else if (strcmp(argv[i], "-input") == 0)
                {
                        yyin = fopen(argv[i+1], "r");
                        input_flag = 1;
                }

                else if (strcmp(argv[i], "-output") == 0)
                {
                        output_file.open(argv[i + 1]);
                        output_flag = 1;
                }

                else if (strcmp(argv[i], "-verbose") == 0)
                {
                        cerr<<"verbose-output.pdf showcases the detailed output\n";
                        ifstream parser_output("new_parser.output");
                        ofstream verbose_file("verbose-output.pdf"); 
                        verbose_file << parser_output.rdbuf();
                        //return 0;
                }
        }

        if (input_flag == 0)
        {
                cerr<< "Input file not specified, check -help\n";
                cerr<< "Using default input file input.py\n";
                yyin = fopen("input.py", "r");
                //return 0;
        }

        if (output_flag == 0)
        {
                cerr<< "Output file not specified, check -help\n";
                cerr<< "Using default output file output.dot\n";
                output_file.open("output.dot");
                //return 0;
        }
        
        // else
        // {
        //         yyin=stdin;
        // }       

        yyparse();

        // if (root != NULL) 
        // {
        //         AST ast(root);
        //         ast.Print();
        // }

        if (root != NULL)
        {
                streambuf *backup = cout.rdbuf(output_file.rdbuf()); // Redirect cout to the output file
                AST ast(root);
                ast.Print(); // Print AST to the redirected cout
                cout.rdbuf(backup); // Restore the original cout buffer
        }
        return 0;
}

void yyerror(const char *msg) {
        printf(" %s at line %d\n", msg,yylineno);
}