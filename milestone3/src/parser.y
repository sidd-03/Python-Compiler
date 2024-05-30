%{    
#include "ast.hpp"
#include "make_x86.hpp"
#include <iostream>
#include <fstream>
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
#include <map> 
using namespace std;
vector <string> glbl_string;
int scope_level = 0;
std::map<std::string, std::vector<string>> classMap;
std::map<string,string> obj_types;
vector <string> glbl_string_variable;
/* the symbol table entry */
typedef struct entry_struct {
    string identifier;
    string type;
    string kind;
    vector<pair < string,string > > parameters; // For adding functions' parameters
    struct entry_struct *next;
    int size;
    int line_no;
} entry_t;

/* the symbol table structure */
typedef struct symbol_table_struct {
    struct symbol_table_struct *parent;
    entry_t *entry_top;
} symbol_table_t;



entry_t* symbol_table_insert(symbol_table_t* symbol_table, entry_t entry)
{
    entry_t *new_entry = NULL;
    entry_t *cursor = NULL;

    new_entry = (entry_t *) malloc(sizeof(entry));
    if (new_entry == NULL) {
        return NULL;
    }
    new_entry -> type = entry.type;
    new_entry -> kind = entry.kind;
    new_entry -> identifier = entry.identifier;
    new_entry -> parameters = entry.parameters;
    new_entry -> next = NULL;
    new_entry -> size = entry.size;
    new_entry -> line_no = entry.line_no;
    cursor = symbol_table -> entry_top;

    /* if it's the first entry, insert and return */
    if (cursor == NULL) {
        symbol_table -> entry_top = new_entry;
        return new_entry;
    }

    /* go to the last entry and insert */
    while (cursor -> next != NULL) {
        cursor = cursor -> next;
    }

    cursor -> next = new_entry;
    return new_entry;
}

/**
 * Search the symbol table for the given indentifier name and return a pointer
 * to the latest entry.
 *
 * @param symbol_table_t The pointer to the symbol table to search in
 * @param string The name of the identifier
 * @return entry_t* The pointer to the symbol table entry. NULL if not found.
 */
entry_t* symbol_table_lookup(symbol_table_t* symbol_table, string name)
{
    symbol_table_t *table = NULL;
    entry_t *entry = NULL;

    if (symbol_table == NULL) {
        return NULL;
    }

    table = symbol_table;
    entry = symbol_table -> entry_top;

    while (entry != NULL) {
        if (entry -> identifier == name ) {
            return entry;
        }
        entry = entry -> next;
    }

    /* increment the scope level if searching in parent */
    scope_level++;
    return symbol_table_lookup(table -> parent, name);
}

symbol_table_t* symbol_table_enter_scope(symbol_table_t* symbol_table)
{
    symbol_table_t *new_table = NULL;

    new_table = (symbol_table_t *) malloc(sizeof(symbol_table_t));
    new_table -> parent = symbol_table;
    new_table -> entry_top = NULL;
    scope_level++;
    return new_table;
}

symbol_table_t* symbol_table_exit_scope(symbol_table_t* symbol_table)
{
    symbol_table_t *parent;

    parent = symbol_table -> parent;
    scope_level--;
    free(symbol_table);

    return parent;
}

void print_symbol_table(symbol_table_t* symbol_table)
{
    entry_t *entry = NULL;
    symbol_table_t *table = NULL;

    table = symbol_table;

    while (table != NULL) {
        entry = table -> entry_top;
        while (entry != NULL) {
std::cout << "Identifier: " << entry->identifier << ", Type: " << entry->type << ", Kind: " << entry->kind << ", Size: "<< entry -> size << ", Line_no: "<< entry -> line_no<< std::endl;
               for(const auto& pair : entry->parameters ) {
                cout << pair.first <<","<<pair.second << endl;
               }
            entry = entry -> next;
        }
        table = table -> parent;
    }
}
int entry_size (entry_t entry) {
        int size = 0;
        if(entry.kind  == "FUNCTION"){
                for(const auto&pair : entry.parameters) {
                        if(pair.second == "INT") size+= 8;
                }
        }
        return size ;
}

void yyerror(const char *);
extern FILE *yyin;
extern int yylineno;
extern int yylex();
extern int yyparse();
AstNode* root = NULL;
symbol_table_t* global_table  = symbol_table_enter_scope(NULL);
gen_asm_code* gen = new gen_asm_code();
int n_nodes = 0;
int constr = 0;
map<string, string> data_type_map;

%}

%union {
        char *strval;
        int intval;
        float floatval;
       AstNode* astNode;
}

%error-verbose

%token ID STRING RETURN BREAK CONTINUE GLOBAL NONLOCAL YIELD DOTTED_ID
%token INDENT DEDENT INDENTERROR NEWLINE GT LT GTE LTE EQUAL NOT NOTEQUAL
%token INT FLOAT STR BOOL LIST INT_NUMBER FLOAT_NUMBER POWER FLOORDIV CONSTR

%token<astNode> IF ELSE ELIF DEF WHILE FOR IN RANGE PRINT INPUT CLASS TRY EXCEPT MATCH CASE WITH AS PASS AND OR TRUE_TOK FALSE_TOK NONE_TOK LSHIFT RSHIFT ARRAY

%type<astNode> ID STRING RETURN BREAK CONTINUE GLOBAL NONLOCAL YIELD GT LT GTE LTE EQUAL NOT NOTEQUAL INT FLOAT STR BOOL LIST
%type<astNode> INT_NUMBER FLOAT_NUMBER boolean POWER FLOORDIV CONSTR 
%type<strval> data_type 
%type<astNode> prog statements statement function assignment NUMBER function_block function_stmts function_stmt_ function_sp_stmt function_compound_stmt args args_ arg simple_stmt start function_call argsp args_p argp argsc args_c argc return_stmt global_stmt nonlocal_stmt
%type<astNode> func_while_stmt func_for_stmt func_for_block func_for_stmts func_for_stmt_ relation_stmt for_sp_stmt range_args  for_compound_stmt func_declare
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
    if($1!=NULL)
    $$ = $1;
}
| NEWLINE statements {if($2!=NULL)$$ = $2;}
;

statements : statement {
        $$=new StatementsNode("root");
        $$->node_number = n_nodes;
        if($1!=NULL) $$->add($1);
        }
        | statements statement {if($2!=NULL) $1->add($2); $$ = $1;}
        | statements NEWLINE {  }
;
statement  
        : if_stmt { $$ = $1; }
        | while_stmt { $$ = $1; }
        | for_stmt { $$ = $1; }
        | function { global_table = symbol_table_enter_scope(global_table);$$ = $1; }
        | function_call { $$ = $1; }
        | assignment NEWLINE { $$ = $1; }
        | class { if($1!=NULL) $$ = $1; }
        | try_except_stmt { $$ = $1; }
        | match_stmt { $$ = $1; }
        | pass NEWLINE { $$ = $1; }
;
NUMBER : INT_NUMBER { std::string nname = "int" + std::to_string(n_nodes);
                                $1->name = nname;
                                $1->node_number = n_nodes;
                                $$ = $1;
                                ++n_nodes;
                                 }
        | FLOAT_NUMBER { std::string nname = "float" + std::to_string(n_nodes);
                                $1->name=nname;
                                $1->node_number = n_nodes;
                                $$ = $1; 
                                ++n_nodes;
                               }
;

boolean : TRUE_TOK {  std::string nname = "bool" + std::to_string(n_nodes);
                                $1 = new BoolNode();
                                $1->name=nname;
                                $1->label = "True";
                                $1->node_number = n_nodes;
                                $$ = $1; 
                                ++n_nodes;
                               }
        | FALSE_TOK { std::string nname = "bool" + std::to_string(n_nodes);
                                $1 = new BoolNode();
                                $1->name=nname;
                                $1->label = "False";
                                $1->node_number = n_nodes;
                                $$ = $1;
                                ++n_nodes;
                                 }
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
                $$ = new IfStatementNode();
                $$->name = nname;
                $$->add($1);
                $$->add($2);
                if ($3 != NULL)
                    $$->add($3);
                $$->node_number = n_nodes;    
                ++n_nodes;
               
        }
        | if_header ':' block elif_else_ {
                std::string nname = "if" + std::to_string(n_nodes);
                $$ = new IfStatementNode();
                $$->name = nname;
                $$->add($1);
                $$->add($3);
                if ($4 != NULL)
                    $$->add($4);
                $$->node_number = n_nodes;       
                ++n_nodes;
                
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
        : elif_stmts else_stmt { 
                std::string nname = "block" + std::to_string(n_nodes);
                $$=new BlockNode(nname);
                $$->label = "Elif..._Else";
                $$->add($1); 
                $$->add($2);
                $$->node_number = n_nodes;   
                ++n_nodes;
            }
        | elif_stmts { $$ = $1; }
        | else_stmt { $$ = $1;  }
;

else_stmt
        : ELSE ':' block { 
                std::string nname = "else" + std::to_string(n_nodes);
                $$ = new ElseStatementNode(nname);
                $$->add($3);
                $$->node_number = n_nodes;   
                ++n_nodes;
                }
;

elif_stmts  
        : elif_stmt {
                std::string nname = "block" + std::to_string(n_nodes);
                $$=new BlockNode(nname);
                $$->label = "Elif..."; 
                $$->add($1); 
                $$->node_number = n_nodes;   
                ++n_nodes; 
               
        }
        | elif_stmts elif_stmt { $1->add($2); $$ = $1; }
;
elif_stmt   
        : elif_header ':' block {
                std::string nname = "elif" + std::to_string(n_nodes);
                $$ = new ElIfStatementNode(nname);
                $$->add($1);
                $$->add($3);
                $$->node_number = n_nodes;   
                ++n_nodes;
               
        }
;

elif_header 
        : ELIF relation_stmt { $$ = $2; }

;
func_declare
        :  DEF ID '(' args ')' '-' GT data_type ':'  {
            //printf("Generic Function successfully parsed:\n");
                std::string name = "func" + std::to_string(n_nodes);
                IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
                entry_t* entry = symbol_table_lookup(global_table , idFunc -> value);
                 if (entry) {
                    std::cerr << "Error: Function " << idFunc->name << " already defined at " << entry->line_no <<std::endl;
                }
                else {
                         vector<pair< string, string>> params;
                        entry_t new_entry = {idFunc -> value, $8, "FUNCTION", params , NULL , 0 , idFunc -> line_no};
                        $$ = new FunctionNode(idFunc->value + "_Generic",idFunc->value);
                       if ($4 != NULL)
                   { $$->add($4); 
                   Args* arguments = dynamic_cast<Args*>($4);
                   vector<AstNode *> next = arguments -> next ;
                   for (const auto &arg : next)  {
                        string arg1 ="";
         if (arg->label == "Identifier") {  
        IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(arg);
        arg1 = temp1->value;
        } 
        else if (arg->label == "float" || arg->label == "int") {
        NumberNode* temp1 = dynamic_cast<NumberNode*>(arg);
        arg1 = to_string(temp1->value);
        } 
        else if (arg->label == "string") {
        StringNode* temp1 = dynamic_cast<StringNode*>(arg);
        arg1 = temp1->value;
        }
        else if (arg->label == "True" || arg->label == "False") {
        BoolNode* temp1 = dynamic_cast<BoolNode*>(arg);
        arg1 = temp1->label;
        }               
                      if(arg1 != "self"){
                                                
                        new_entry.parameters.push_back(make_pair(arg1, data_type_map.at(arg-> name) ));
               }
               else{
                        new_entry.parameters.push_back(make_pair(arg1, "SELF" ));}
                   }
                   }
                new_entry.size = entry_size(new_entry);
                // $$->add($10);
                $$->node_number = n_nodes;
                ++n_nodes; 
                 symbol_table_insert(global_table, new_entry);
                }
                
               
        }

function    
        : DEF ID '(' args ')' function_block {
        //printf("Function successfully parsed:\n"); 
                std::string name = "func" + std::to_string(n_nodes);
                  IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
                entry_t* entry = symbol_table_lookup(global_table, idFunc->value);
                 if (entry) {
                    std::cerr << "Error: Function " << idFunc->value << " already defined at "<<entry-> line_no << std::endl;
                }
                else {
                $$ = new FunctionNode(idFunc->value + "_Generic",idFunc->value);
                if ($4 != NULL)
                $$->add($4);
                $$->add($6);
                $$->node_number = n_nodes;
                ++n_nodes;
                }
        }
        | DEF ID '(' args ')' ':' function_block {
        //printf("Function successfully parsed:\n"); 
                std::string name = "func" + std::to_string(n_nodes);
                  IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
                $$ =new FunctionNode(idFunc->value + "_Generic",idFunc->value);
                if ($4 != NULL)
                    $$->add($4);
                $$->add($7);
                $$->node_number = n_nodes;
                entry_t* entry = symbol_table_lookup(global_table, idFunc->value);
                if (entry) {
                    std::cerr << "At line no " << idFunc->line_no <<" Error: Function " << idFunc->value << " already defined at " <<entry-> line_no <<  std::endl;
                }
                else {
                         vector<pair< string, string>> params;
                        entry_t new_entry = {idFunc -> value, "VOID", "FUNCTION", params , NULL , 0 , idFunc -> line_no};
                        symbol_table_insert(global_table, new_entry);
                }
                ++n_nodes;
              
        }
        | DEF ID '(' args ')' '-' GT data_type function_block {
            //printf("Generic Function successfully parsed:\n");
                std::string name = "func" + std::to_string(n_nodes);
                 IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
                $$ = new FunctionNode(idFunc->value + "_Generic",idFunc->value);
                if ($4 != NULL)
                    $$->add($4);
                $$->add($9);
                $$->node_number = n_nodes;
                ++n_nodes;
               
        }
        // | DEF ID '(' args ')' '-' GT data_type ':' function_block {
        //     //printf("Generic Function successfully parsed:\n");
        //         std::string name = "func" + std::to_string(n_nodes);
        //         IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($2);
        //         entry_t* entry = symbol_table_lookup(global_table , idFunc -> value);
        //          if (entry) {
        //             std::cerr << "Error: Function " << idFunc->name << " already defined at " << entry->line_no <<std::endl;
        //         }
        //         else {
        //                  vector<pair< string, string>> params;
        //                 entry_t new_entry = {idFunc -> value, $8, "FUNCTION", params , NULL , 0 , idFunc -> line_no};
        //                 $$ = new FunctionNode(idFunc->value + "_Generic");
        //                if ($4 != NULL)
        //            { $$->add($4); 
        //            Args* arguments = dynamic_cast<Args*>($4);
        //            vector<AstNode *> next = arguments -> next ;
        //            for (const auto &arg : next)  {
        //                 string arg1 ="";
        //  if (arg->label == "Identifier") {  
        // IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(arg);
        // arg1 = temp1->value;
        // } 
        // else if (arg->label == "float" || arg->label == "int") {
        // NumberNode* temp1 = dynamic_cast<NumberNode*>(arg);
        // arg1 = to_string(temp1->value);
        // } 
        // else if (arg->label == "string") {
        // StringNode* temp1 = dynamic_cast<StringNode*>(arg);
        // arg1 = temp1->value;
        // }
        // else if (arg->label == "True" || arg->label == "False") {
        // BoolNode* temp1 = dynamic_cast<BoolNode*>(arg);
        // arg1 = temp1->label;
        // }
        //                 new_entry.parameters.push_back(make_pair(arg1, data_type_map.at(arg-> name) ));
        //            }
        //            }
        //         new_entry.size = entry_size(new_entry);
        //         $$->add($10);
        //         $$->node_number = n_nodes;
        //         ++n_nodes; 
        //          symbol_table_insert(global_table, new_entry);
        //         }
                
               
        // }
        | func_declare function_block{
                //printf("Function successfully parsed:\n"); 
                        std::string name = "func" + std::to_string(n_nodes);
                        $$ = $1;
                        $$->add($2);
                        $$->node_number = n_nodes;
                        ++n_nodes;
        }
        | DEF CONSTR '(' args ')' ':' function_block {
                 std::string name = "constr" + std::to_string(n_nodes);
                  ConstructorNode* c = dynamic_cast<ConstructorNode*>($2);
                $$ = new ConstructorNode ("Constructor"+ name);
                if($4!=NULL)
                     $$->add($4);
                $$->add($7);
                if($4!=NULL){
                         Args* arguments = dynamic_cast<Args*>($4);
                         int offset = 0;

                        for(auto (&child) : arguments->next){

                        }

                }
                $$->node_number = n_nodes;
                ++n_nodes;
               
        }
;

function_block
            : NEWLINE INDENT function_stmts DEDENT { global_table = symbol_table_enter_scope(global_table) ;
            $$ = $3;
            }
;

function_stmts   
            : function_stmt_    { 
                                        std::string nname = "stmt" + std::to_string(n_nodes);
                                        $$ = new StatementsNode(nname); 
                                        $$->add($1);
                                        $$->node_number = n_nodes; 
                                        ++n_nodes;
                                      
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
        :  WHILE relation_stmt ':' func_for_block { 
            //printf("while statement successfully parsed:\n");
                std::string name = "while" + std::to_string(n_nodes);
                $$ = new WhileStatementNode();
                $$->name = name;
                $$->add($2);
                $$->add($4);
                $$->node_number = n_nodes;
                ++n_nodes;
               
        }

;

func_for_stmt    
        : FOR ID IN RANGE '(' range_args ')' ':' func_for_block { 
            //printf("for statement successfully parsed:\n");
                std::string name = "for" + std::to_string(n_nodes);
                 IdentifierNode* temp = dynamic_cast<IdentifierNode*>($2);
                $$ = new ForStatementNode(temp->value);
                $$->name = name;
                $2->node_number = n_nodes;
                ++n_nodes;
                $$->node_number = n_nodes;
                ++n_nodes;
                std::string nname = "range" + std::to_string(n_nodes);
                $4 = new FunctionCallNode(nname,"range");
                $4->add($6);
                $4->node_number = n_nodes;
                ++n_nodes;
                $$->add($4);
                $$->add($9);
                
        }
        | FOR ID IN ID ':' func_for_block { 
            //printf("for statement successfully parsed:\n"); 
        }
;

func_for_block
        : NEWLINE INDENT func_for_stmts DEDENT {
                global_table = symbol_table_enter_scope(global_table);
                 $$ = $3; }
;

func_for_stmts   
        : func_for_stmt_ {
                std::string nname = "stmt" + std::to_string(n_nodes);
                $$ = new StatementsNode(nname); 
                $$->add($1); 
                $$->node_number = n_nodes;
                ++n_nodes;
                
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
        : func_if_header ':' function_block func_elif_else_  { 
                //printf("if statement successfully parsed:\n");
                std::string nname = "if" + std::to_string(n_nodes);
                $$ = new IfStatementNode();
                $$->name = nname;
                $$->add($1);
                $$->add($3);
                if ($4 != NULL)
                    $$->add($4);
                $$->node_number = n_nodes;
                ++n_nodes;
               
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
                $$=new BlockNode(nname);
                $$->label = "Elif..._Else";
                $$->add($1); 
                $$->add($2); 
                $$->node_number = n_nodes;
                ++n_nodes;
               
        }
        | func_elif_stmts { $$ = $1; }
        | func_else_stmt { $$ = $1; }
;

func_else_stmt   
        : ELSE ':' function_block {
                std::string nname = "else" + std::to_string(n_nodes);
                $$ = new ElseStatementNode(nname);
                $$->add($3);
                $$->node_number = n_nodes;
                ++n_nodes;
                
        }
;

func_elif_stmts  
        : func_elif_stmt {
                std::string nname = "block" + std::to_string(n_nodes);
                $$=new BlockNode(nname);
                $$->label = "Elif..."; 
                $$->add($1); 
                $$->node_number = n_nodes;
                ++n_nodes; 
               
        }
        | func_elif_stmts func_elif_stmt { $1->add($2); $$ = $1; }
;

func_elif_stmt   
        :  func_elif_header ':' function_block {
                std::string nname = "elif" + std::to_string(n_nodes);
                $$ = new ElIfStatementNode(nname);
                $$->add($1);
                $$->add($3);
                $$->node_number = n_nodes;
                ++n_nodes;
             
        }
;

func_elif_header 
        : ELIF relation_stmt { $$ = $2; }

;  /////start here for node number update
args    
    : /* empty params */ { $$ = NULL; }
    | args_  { $$ = $1; }
;


args_ 
    : arg{ 
                std::string nname = "arg" + std::to_string(n_nodes);
                 $$ = new Args(nname); $$->add($1); 
                 $$->node_number = n_nodes;
                ++n_nodes;
                cout <<"I am here"<<endl;
               
        }
    | args_ ',' arg { $1->add($3); $$ = $1; }
;




arg   
    : ID { 
        std::string nname = "iden" + std::to_string(n_nodes);
        ++n_nodes;
       $1->name=nname;
    }

    | ID ':' data_type { 
        std::string nname = "iden" + std::to_string(n_nodes);
        cout << " Chacking data type " <<$3<<endl;
        data_type_map[nname] = $3;
        ++n_nodes;
        $1->name=nname;
        IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>($1);
         cout<<"parser.y: arg rule: id value: "<<temp1->value<<endl;
        vector<pair< string, string>> params1;
          entry_t temp = { temp1 -> value , $3, "FUNCTION_PARAMETER" , params1 , NULL , 8 , temp1 -> line_no}; 
                        symbol_table_insert(global_table, temp);
        $$ = $1;
         if($3 == "STR" ){
                string variable = temp1-> value;
                cout<<"Finally!!!"<<variable<<endl;
                glbl_string_variable.push_back(variable);
        }
    }
    | ID ':' LIST '[' data_type ']' { 
        std::string nname = "iden" + std::to_string(n_nodes);
        cout << " Chacking data type " <<$3<<endl;
        data_type_map[nname] = "ARRAY";
        IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>($1);
        cout<<"parser.y: arg rule: id value: "<<temp1->value<<endl;
        vector<pair< string, string>> params1;
          entry_t temp = { temp1 -> value , $5, "FUNCTION_PARAMETER" , params1 , NULL , 8 , temp1 -> line_no}; 
        symbol_table_insert(global_table, temp);
        ++n_nodes;
        $1->name=nname;
        $$ = $1;
    }
; //node number doubt here.

//! args for print function (AST)
argsp  
    : /* empty params */ { $$ = NULL; }
    | args_p { $$ = $1; }
;


args_p
    : argp {
                std::string nname = "arg" + std::to_string(n_nodes);
                $$ = new Args(nname); $$->add($1); 
                $$->node_number = n_nodes;
                ++n_nodes;
              
        }

    | args_p  argp { $1->add($2); $$ = $1; }
;



argp  
    :  relation_stmt {$$ = $1;}
;

//! args for function call (AST)
argsc  
    : /* empty params */ { $$ = NULL; }
    | args_c { $$ = $1; }
;


args_c
    : argc {
                std::string nname = "arg" + std::to_string(n_nodes);
                $$ = new Args(nname); $$->add($1); 
                $$->node_number = n_nodes;
                ++n_nodes;
                
        }

    | args_c ',' argc { $1->add($3); $$ = $1; }
;

argc  
    :  relation_stmt {$$ = $1;}

;

while_stmt
        :  WHILE relation_stmt ':' for_block { 
            //printf("while statement successfully parsed:\n");
                std::string name = "while" + std::to_string(n_nodes);
                $$ = new WhileStatementNode();
                $$->name = name;
                $$->add($2);
                $$->add($4);
                $$->node_number = n_nodes;
                ++n_nodes;
               
        }
;
range_args   
        : arith_expression {
                std::string nname = "arg" + std::to_string(n_nodes);
                $$ = new Args(nname); 
                $$->add($1); 
                $$->node_number = n_nodes;
                ++n_nodes;
                
        }

        | arith_expression ','arith_expression {
                std::string nname = "arg" + std::to_string(n_nodes);
                $$ = new Args(nname); 
                $$->add($1); 
                $$->add($3); 
                $$->node_number = n_nodes;
                ++n_nodes;
                
        }

        | arith_expression ',' arith_expression ','arith_expression {
                std::string nname = "arg" + std::to_string(n_nodes);
                $$ = new Args(nname); 
                $$->add($1); 
                $$->add($3); 
                $$->add($5); 
                $$->node_number = n_nodes; 
                ++n_nodes;
                
        }
;

for_stmt    
        : FOR ID IN RANGE '(' range_args ')' ':' for_block { 
            //printf("for statement successfully parsed:\n");
                std::string name = "for" + std::to_string(n_nodes);
                IdentifierNode* temp = dynamic_cast<IdentifierNode*>($2);
                $$ = new ForStatementNode(temp->value);
                ForStatementNode* temp1 = dynamic_cast<ForStatementNode*>($$);
                cout<<"ForStatementNode id: "<<temp1->id<<endl;
                $$->name = name;
                $2->node_number = n_nodes;
                ++n_nodes;
                $$->node_number = n_nodes;
                ++n_nodes;
                std::string nname = "range" + std::to_string(n_nodes);
                $4 = new FunctionCallNode(nname,"range");
                $4->add($6);
                $4->node_number = n_nodes;
                ++n_nodes;
                $$->add($4);
                $$->add($9);
                entry_t* entry = symbol_table_lookup(global_table, temp->value);
                if(entry == NULL && temp -> is_dotted == 0) {
                        std::cerr << "At Line No : " << temp -> line_no<<" Error: Variable " << temp->value << " not defined" << std::endl;
                }
             //   FunctionNode* temp2 = dynamic_cast<FU
                
        }

        | FOR ID IN ID ':' for_block { 
            printf("for statement successfully parsed:\n"); 
        }
;

for_block
        : NEWLINE INDENT for_stmts DEDENT { $$ = $3; }
;

for_stmts   
        : for_stmt_ {
                std::string nname = "stmt" + std::to_string(n_nodes);
                $$ = new StatementsNode(nname); 
                $$->add($1);
                $$->node_number = n_nodes;
                ++n_nodes;
               
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
                $$ = new BreakStatementNode(); 
                $$->name = nname;
                $$->node_number = n_nodes;
                ++n_nodes;
               
        }

        | CONTINUE {
                std::string nname = "continue" + std::to_string(n_nodes);
                $$ = new ContinueStatementNode(); 
                $$->name = nname;
                $$->node_number = n_nodes;
                ++n_nodes;
               
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
        : for_if_header ':' for_block for_elif_else_  { 
                //printf("if statement successfully parsed:\n");
                std::string nname = "if" + std::to_string(n_nodes);
                $$ = new IfStatementNode();
                $$->name = nname;
                $$->add($1); 
                $$->add($3); 
                if ($4 != NULL)
                    $$->add($4);
                $$->node_number = n_nodes;
                ++n_nodes;
               
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
                $$=new BlockNode(nname);
                $$->label = "Elif..._Else";
                $$->add($1); 
                $$->add($2); 
                $$->node_number = n_nodes;
                ++n_nodes;
               
        }
        | for_elif_stmts { $$ = $1; }
        | for_else_stmt { $$ = $1; }
;

for_else_stmt   
        : ELSE ':' for_block {
                std::string nname = "else" + std::to_string(n_nodes);
                $$ = new ElseStatementNode(nname);
                $$->add($3);
                $$->node_number = n_nodes;
                ++n_nodes;
               
        }
;

for_elif_stmts  
        : for_elif_stmt {
                std::string nname = "block" + std::to_string(n_nodes);
                $$=new BlockNode(nname);
                $$->label = "Elif..."; 
                $$->add($1);
                $$->node_number = n_nodes;
                ++n_nodes; 
                
        }
        | for_elif_stmts for_elif_stmt { $1->add($2); $$ = $1; }
;

for_elif_stmt   
        : for_elif_header ':' for_block {
                std::string nname = "elif" + std::to_string(n_nodes);
                $$ = new ElIfStatementNode(nname);
                $$->add($1);
                $$->add($3);
                $$->node_number = n_nodes;
                ++n_nodes;
                
        }
;

for_elif_header 
        : ELIF relation_stmt { $$ = $2; }
 
;

function_call     
            : ID '(' argsc ')' {
                std::string name = "_func_call" + std::to_string(n_nodes);
                IdentifierNode* idFunc = dynamic_cast<IdentifierNode*>($1);
                if(idFunc -> is_dotted == 1) {
                    vector<pair<string, string>> params;
                    entry_t new_entry = {idFunc -> value, "VOID", "FUNCTION", params , NULL , 0 , idFunc -> line_no};
                    symbol_table_insert(global_table, new_entry); 
                }
                cout<<"Calculating FuncCall size"<<endl;
                entry_t* entry = symbol_table_lookup(global_table, idFunc->value);
                if (entry == NULL && idFunc -> is_dotted == 0) {
                    std::cerr << "At Line No : " << idFunc -> line_no<<" Error: Function " << idFunc->value << " not defined" << std::endl;
                }
                string func_type = "";
               if(entry!= NULL) func_type = entry->type;
                int size = 0;
                if(entry!= NULL){
                for( const auto& pair : entry -> parameters){
                       cout<<entry -> parameters.size()<<endl;
                        if(pair.second == "ARRAY"){
                                
                                entry_t* temp = symbol_table_lookup(global_table , pair.first);
                                 cout<<"I came here !!!"<<endl;
                                 if(temp!=NULL){
                                        cout<<"I came here !"<<endl;
                                size += temp->size;}
                                cout<<"I came here !!!!!!"<<endl;
                        }
                        else{ size += 8;  }
                }
                //Checking arguments are properly passed
                Args* temp = dynamic_cast<Args*>($3);
                if($3!= NULL){
                        
                       if(temp ->next[0] -> label == "string"){
                        StringNode* temp1 = dynamic_cast<StringNode*>(temp->next[0]);
                        size = (temp1->value).size();
                        cout<<"Checking string size "<<size<<endl;
                       }
                       if(temp-> next[0] -> label == "Identifier"){
                        IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(temp->next[0]);
                        entry_t* entry = symbol_table_lookup(global_table, temp1->value);
                        if(entry == NULL) {
                                std::cerr << "At Line No : " << temp1 -> line_no<<" Error: Variable " << temp1->value << " not defined" << std::endl;
                                }
                        
                       }
                       entry_t* entry = symbol_table_lookup(global_table, idFunc->value);
                       if(temp -> next.size() != entry -> parameters.size() && entry -> kind == "FUNCTION" && idFunc -> is_dotted == 0) {
                                std::cerr <<temp-> next.size() << " "<<entry -> parameters.size()<< " At Line No : " << idFunc -> line_no<<" in " << idFunc->value << " incorrect number of arguments passed" << std::endl;

                       }
                }
                
            }
             
            $$ = new FunctionCallNode(idFunc->value + name , size,func_type,idFunc->value);
                
                if ($3 != NULL)
                    $$->add($3);
                    ///////////
        $$->add($1); //new_change  
        ///////////////////  
                $$->node_number = n_nodes;    
                ++n_nodes;
             
            }

            | PRINT '(' argsp ')' {
                std::string name = "print" + std::to_string(n_nodes);
                int size = 8;
                Args* temp = dynamic_cast<Args*>($3);
                if($3!= NULL){
                       if(temp ->next[0] -> label == "string"){
                        StringNode* temp1 = dynamic_cast<StringNode*>(temp->next[0]);
                        size = (temp1->value).size();
                        cout<<"Checking string size "<<size<<endl;
                       }
                       if(temp-> next[0] -> label == "Identifier"){
                        IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(temp->next[0]);
                        entry_t* entry = symbol_table_lookup(global_table, temp1->value);
                        if(entry == NULL && temp1 -> is_dotted == 0) {
                                std::cerr << "At Line No : " << temp1 -> line_no<<" Error: Variable " << temp1->value << " not defined" << std::endl;
                                }
                        
                       }
                }
                $$ = new FunctionCallNode(name,size,"print");
                ++n_nodes;
                if($3!=NULL){
                $$->add($3);
                }
               
            }

            | INPUT '(' STRING ')' {
                std::string name = "input" + std::to_string(n_nodes);
                $$ = new FunctionCallNode(name,"input");
                $$->node_number = n_nodes;
                ++n_nodes;
                std::string nname = "string" + std::to_string(n_nodes);
                $3->node_number = n_nodes;
                ++n_nodes;
                $3->name=nname;
                $$->add($3);
            }
;

class
    :  CLASS ID ':' class_block { 
        //printf("class statement successfully parsed:\n");
        std::string name = "class" + std::to_string(n_nodes);
        ++n_nodes;
        IdentifierNode* idclass = dynamic_cast<IdentifierNode*>($2);
        $$ = new ClassNode(idclass->value);
        entry_t* entry = symbol_table_lookup(global_table, idclass->value);
        if (entry ) {
            std::cerr << "Error: Class " << idclass->name << " already defined" << std::endl;
        }
        else {
            vector<pair< string, string>> params;
            entry_t new_entry = {idclass -> value, "CLASS", "PARENT_CLASS", params , NULL , 0 , idclass -> line_no};
            symbol_table_insert(global_table, new_entry);
            new_entry = {idclass -> value + "__init__", "CONSTRUCTOR", idclass->value, params , NULL , 0 , idclass -> line_no};
            symbol_table_insert(global_table, new_entry);
        }
        StatementsNode * class_statements = dynamic_cast<StatementsNode*>($4);
        ConstructorNode * constr = dynamic_cast<ConstructorNode*>(class_statements->next[0]);
        constr->value = idclass->value;
        Args * class_variables = dynamic_cast<Args*>(constr->next[0]);
        vector<string> temp;
        for(auto &item : class_variables->next){
                IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(item);
                temp.push_back(temp1->value);
                temp1->classtype = idclass->value;

        }

        classMap[idclass->value] = temp;

             StatementsNode * constr_statements = dynamic_cast<StatementsNode*>(constr->next[1]);
         for(auto &item : constr_statements->next){
                if(item->label == "="){
                        assignmentStatement * a = dynamic_cast<assignmentStatement *>(item);
                        IdentifierNode* a1 = dynamic_cast<IdentifierNode*>(a->next[0]);
                        if(a1->is_dotted){
                                a1->classtype = idclass->value;
                        }
                }
         }


        $$->add($4);
    }
    | CLASS ID '(' ID ')' ':' class_block { 
        //printf("class statement successfully parsed:\n");
        std::string name = "class" + std::to_string(n_nodes);
        ++n_nodes;
        IdentifierNode* idclass = dynamic_cast<IdentifierNode*>($2);
        IdentifierNode* idclass_parent = dynamic_cast<IdentifierNode*>($4);

        StatementsNode * class_statements = dynamic_cast<StatementsNode*>($7);
        ConstructorNode * constr = dynamic_cast<ConstructorNode*>(class_statements->next[0]);
        constr->value = idclass->value;
        
        ClassNode* childClass = new ClassNode(idclass->value);
      //  cout<<idclass->value<<" "<<idclass_parent->value<<endl;
        childClass->add($7);
       
        ClassNode* parentClass = childClass->findParentClassNode(idclass_parent->value);
      
     if (parentClass) {
        
        parentClass->add(childClass);
    
        $$ = NULL;
    } else {
        
        std::cerr << "Error: Parent class not found for " << idclass->value << std::endl;
        delete childClass;
        $$ = nullptr;    
    }
     entry_t* entry = symbol_table_lookup(global_table, idclass->value);
        if (entry) {
            std::cerr << "Error: Class " << idclass->name << " already defined" << std::endl;
        }
        else {
            vector<pair< string, string>> params;
            entry_t new_entry = {idclass -> value, idclass_parent -> value , "Inherited Class", params , NULL , 0 , idclass -> line_no};
            symbol_table_insert(global_table, new_entry);
            new_entry = {idclass -> value + "__init__", "CONSTRUCTOR", idclass->value, params , NULL , 0 , idclass -> line_no};
            symbol_table_insert(global_table, new_entry);
        }
        Args * class_variables = dynamic_cast<Args*>(constr->next[0]);
        vector<string> temp;
        for(auto &item : class_variables->next){
                IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>(item);
                temp.push_back(temp1->value);
                temp1->classtype = idclass->value;

        }
        classMap[idclass->value] = temp;
             StatementsNode * constr_statements = dynamic_cast<StatementsNode*>(constr->next[1]);
         for(auto &item : constr_statements->next){
                if(item->label == "="){
                        assignmentStatement * a = dynamic_cast<assignmentStatement *>(item);
                        IdentifierNode* a1 = dynamic_cast<IdentifierNode*>(a->next[0]);
                        if(a1->is_dotted){
                                a1->classtype = idclass->value;
                        }
                }
         }
    }
;

class_block
        : NEWLINE INDENT class_stmts DEDENT { $$ = $3; }
;

class_stmts   
        : class_stmt_ {
            std::string nname = "stmt" + std::to_string(n_nodes);
            $$ = new StatementsNode(nname); 
            $$->add($1); 
            $$->node_number = n_nodes;
            ++n_nodes;
           
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
            :  TRY ':' block except_clauses { 
                //printf("try_except statement successfully parsed:\n");
                std::string name = "try" + std::to_string(n_nodes);
                $$ = new TryStatementNode();
                $$->name = name;
                $$->add($3);
                $$->add($4);
                $$->node_number = n_nodes;
                ++n_nodes;
                
            }
;

except_clauses    
            : except_clause {
                std::string nname = "block" + std::to_string(n_nodes);
                $$=new BlockNode(nname);
                $$->label = "Except..."; 
                $$->add($1); 
                $$->node_number = n_nodes;
                ++n_nodes; 
                
            }
            | except_clauses except_clause { $1->add($2); $$ = $1; }
;

except_clause 
            :  EXCEPT ':' block {
                std::string nname = "except" + std::to_string(n_nodes);
                $$ = new ExceptStatementNode();
                $$->name = nname;
                $$->add($3);
                $$->node_number = n_nodes;
                ++n_nodes;
               
            }
;
match_stmt   
        : MATCH ID ':' match_block { 
            //printf("match statement successfully parsed:\n");
            std::string name = "match" + std::to_string(n_nodes);
            $$ = new matchStatementNode();
            $$->name = name;
            $$->node_number = n_nodes;
            ++n_nodes;
            std::string nname = "iden" + std::to_string(n_nodes);
            $2 -> node_number = n_nodes;
            ++n_nodes;
            $2->name=nname;
            $$->add($2);
            $$->add($4);
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
            : CASE argp ':' block {
                std::string nname = "case" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new CaseStatementNode(nname);
                $$->add($2);
                $$->add($4);
            }
;

final_case  
        : CASE '_' block {
            std::string nname = "case" + std::to_string(n_nodes);
            $$ = new CaseStatementNode(nname);
            $$->label = "Default Statement";
            $$->add($3);
            $$->node_number = n_nodes;
            ++n_nodes;
            
        }
;

match_block 
        : NEWLINE INDENT case_statements final_case DEDENT {
            std::string nname = "block" + std::to_string(n_nodes);
            $$=new BlockNode(nname);
            $$->label = "Match Block"; 
            $$->node_number = n_nodes;
            $$->add($3);
            $$->add($4);
            ++n_nodes; 
           
        }
;

block
        :NEWLINE INDENT stmts DEDENT { $$ = $3; 
        global_table = symbol_table_enter_scope(global_table);
        }
;
stmts 
    : stmt {
            std::string nname = "stmt" + std::to_string(n_nodes);
            $$ = new StatementsNode(nname); 
            $$->add($1); 
            $$->node_number = n_nodes;
            ++n_nodes;
            
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
        : ID ':' data_type '=' relation_stmt  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name, "=");
                $$->add($1);
                $$->add($5);
               entry_t* entry = symbol_table_lookup(global_table, $1->name);
                if (entry) {
                    std::cerr << "Error: Variable " << $1->name << " already defined" << std::endl;
                }
                else {
                    vector<pair< string, string>> params;
                    IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                    entry_t new_entry = {id->value, $3,"VARIABLE", params , NULL , 0 , $1->line_no};
                    symbol_table_insert(global_table, new_entry);
                }
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                obj_types[id->value] = $3; //new_changes
                if(!($3 == "INT" || $3 == "FLOAT" || $3 == "BOOL" || $3 == "STR" || $3 =="NONE")){
                        cout <<"Finally in cons_call !!!" << $3 <<endl;
                        id->label = "Constructor Call";
                }
                 if(id->is_dotted){
                        id->label = "Class Store";
                       
                 }
                  if($3 == "STR") {
                        string variable = id -> value;
                        glbl_string_variable.push_back(variable);
                 }
              //  cout<<"here"<<endl;
        }
        | ID ':' data_type '=' NONE_TOK  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name, "=");
                $$->add($1);
                $$->add($5);
                entry_t* entry = symbol_table_lookup(global_table, $1->name);
                if (entry) {
                    std::cerr << "Error: Variable " << $1->name << " already defined" << std::endl;
                }
                else {
                    vector<pair< string, string>> params;
                    IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                    cout<<"Checking data type "<<$3<<endl;
                    entry_t new_entry = {id->value, $3,"VARIABLE", params , NULL , 0 , $1->line_no};
                    symbol_table_insert(global_table, new_entry);
                }
              //  cout<<"here"<<endl;
        }
        | ID '=' relation_stmt  {
                std::string name = "assignment" + std::to_string(n_nodes);
                $$ = new assignmentStatement(name, "="); 
                $$->node_number = n_nodes;
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                $1->node_number = n_nodes;
                ++n_nodes;
                $1->name=nname;
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($3);
                
        }

        | ID '+' '=' relation_stmt  {
                std::string name = "plusequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"+=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
                
               
        }

        | ID '-' '=' relation_stmt  {
                std::string name = "minusequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"-=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }

        | ID '*' '=' relation_stmt  {
                std::string name = "multiequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"*=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }

        | ID '/' '=' relation_stmt  {
                std::string name = "divequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"/=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
              
                $$->add($4);
        }
        | ID '%' '=' relation_stmt  {
                std::string name = "modequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"%=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }
        | ID POWER '=' relation_stmt  {
                std::string name = "powerequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"**=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }
        | ID '&' '=' relation_stmt  {
                std::string name = "andequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"&=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }
        | ID '|' '=' relation_stmt  {
                std::string name = "orequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"|=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }
        | ID '^' '=' relation_stmt  {
                std::string name = "xorequal" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"^=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }
        | ID LSHIFT '=' relation_stmt  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"<<=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }
        | ID RSHIFT '=' relation_stmt  {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,">>=");
                IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                 if(id->is_dotted){
                        id->label = "Class Store";
                 }
                $$->add($1);
                $$->add($4);
        }
        | ID '[' relation_stmt ']' '=' relation_stmt   {
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                $1->node_number = n_nodes;
                ++n_nodes;
                $3->node_number = n_nodes;
                ++n_nodes;
                $1->name=nname;
                $1->label="Array Store";
                $$ = new assignmentStatement(name,"=");
                $1->add($3);
                $$->add($1);
                $$->add($6);
                
        }
        |  ID ':' LIST '[' data_type ']' '=' ARRAY  {
                cout<<"NEW array rule matched" << endl;
                std::string name = "assignment" + std::to_string(n_nodes);
                ++n_nodes;
                std::string nname = "iden" + std::to_string(n_nodes);
                ++n_nodes;
                $1->name=nname;
                $$ = new assignmentStatement(name,"=");
                $8->node_number=n_nodes;
                n_nodes++;
                $$->add($1);
                $$->add($8);
                entry_t* entry = symbol_table_lookup(global_table, $1->name);
                if (entry) {
                    std::cerr << "Error: array " << $1->name << " already defined" << std::endl;
                }
                else {
                    vector<pair< string, string>> params;
                    IdentifierNode* id = dynamic_cast<IdentifierNode*>($1);
                    IdentifierNode* temp = dynamic_cast<IdentifierNode*>($8);
                    entry_t new_entry = {id->value, $5,"array", params , NULL , (int)(temp -> v.size()*8) , $1->line_no};
                    symbol_table_insert(global_table, new_entry);
                }
        }
;

return_stmt
        : RETURN arith_expression {
                std::string nname = "return" + std::to_string(n_nodes);
                $$ = new ReturnStatementNode($2);
                $$->node_number=n_nodes;
                $$->name = nname;
                 ++n_nodes;
        }
        
        
        | RETURN arith_expression ',' arith_expression {
                std::string nname = "return" + std::to_string(n_nodes);
                $$ = new ReturnStatementNode($2);
                $$->add($4);
                $$->node_number=n_nodes;
                $$->name = nname;
                ++n_nodes;
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
                $$ = new BinaryExpressionNode("OR", $1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                std::string name = "OR" + std::to_string(n_nodes);
                $1->name=name;
                $1->node_number = n_nodes;
                ++n_nodes;

}
| and_test
;

and_test : and_test AND not_test {
         std::string nname = "and" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode("AND", $1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                std::string name = "AND" + std::to_string(n_nodes);
                $1->name=name;
                $1->node_number = n_nodes;
                ++n_nodes;
               
               
}
| not_test
;
not_test : NOT not_test {
               std::string nname = "not" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new UnaryExpressionNode("NOT", $2);
                $$->name=nname;
}
| comparison {$$ = $1;}
;
 
comparison
        : comparison GT expressions    {
                std::string nname = "gt" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode(">", $1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                // std::string name = "iden" + std::to_string(n_nodes);
                // ++n_nodes;
                // $1->name=name;
         
        }

        | comparison LT expressions    {
                std::string nname = "lt" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode("<", $1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                // std::string name = "iden" + std::to_string(n_nodes);
                // ++n_nodes;
                // $1->name=name;
               
        }

        | comparison GTE expressions   {
                std::string nname = "gte" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode(">=", $1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                // std::string name = "iden" + std::to_string(n_nodes);
                // ++n_nodes;
                // $1->name=name;
          
        }

        | comparison LTE expressions   {
                std::string nname = "lte" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode("<=", $1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                // std::string name = "iden" + std::to_string(n_nodes);
                // ++n_nodes;
                // $1->name=name;
                
        }

        | comparison EQUAL expressions {
                std::string nname = "equal" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode("==", $1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                // std::string name = "iden" + std::to_string(n_nodes);
                // ++n_nodes;
                // $1->name=name;
                
        }

        | comparison NOTEQUAL expressions {
                std::string nname = "notequal" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode("!=", $1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                // std::string name = "iden" + std::to_string(n_nodes);
                // ++n_nodes;
                // $1->name=name;
                
        }

        | expressions {
                $$ = $1;
        }
;
expressions : expressions '|' xor_expression { 
         std::string nname = "or" + std::to_string(n_nodes);
          $$ = new BinaryExpressionNode("|", $1, $3);
                 $$->node_number = n_nodes;
                 $$->name = nname;
                ++n_nodes;
               
          }
| xor_expression { $$ = $1; }
;

xor_expression : xor_expression '^' and_expression {
                std::string nname = "xor" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode("^", $1, $3);
                 $$->node_number = n_nodes;
                 $$->name = nname;
                ++n_nodes;
              
}
| and_expression {$$ = $1;}
;

and_expression : and_expression '&' shift_expression {
                std::string nname = "and" + std::to_string(n_nodes);
                $$ = new BinaryExpressionNode("&", $1, $3);
                 $$->node_number = n_nodes;
                 $$->name = nname;
                ++n_nodes;
                
}
| shift_expression { $$ = $1; }
;

shift_expression : shift_expression LSHIFT arith_expression {
          std::string nname = "leftshift" + std::to_string(n_nodes);
          $$ = new BinaryExpressionNode("<<", $1, $3);
                 $$->node_number = n_nodes;
                 $$->name = nname;
                ++n_nodes;
                
}
| shift_expression RSHIFT arith_expression {
          std::string nname = "rightshift" + std::to_string(n_nodes);
             $$ = new BinaryExpressionNode(">>", $1, $3);
                 $$->node_number = n_nodes;
                 $$->name = nname;
                ++n_nodes;
             
}
| arith_expression {$$ = $1;}

;

arith_expression : arith_expression '+' term {
           std::string nname = "add" + std::to_string(n_nodes);
                $$ = new AdditionExpressionNode($1, $3);
                $$->node_number = n_nodes;
                 ++n_nodes;
                $$->name=nname;
}
| arith_expression '-' term {
         std::string nname = "sub" + std::to_string(n_nodes);
          $$ = new SubtractionExpressionNode($1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
               
}
| term {$$ = $1;}
;
term : term '*' factor {
         std::string nname = "multi" + std::to_string(n_nodes);
         $$ = new MultiplicationExpressionNode($1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                
}
| term '/' factor {
        std::string nname = "divide" + std::to_string(n_nodes);
         $$ = new DivisionExpressionNode($1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
               
}
| term '%' factor {
         std::string nname = "modulus" + std::to_string(n_nodes);
          $$ = new ModulusExpressionNode($1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
               
}
| term FLOORDIV factor {
          std::string nname = "floordiv" + std::to_string(n_nodes);
          $$ = new FloorDivisionExpressionNode($1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
                
}
| term POWER factor {
        std::string nname = "power" + std::to_string(n_nodes);
          $$ = new PowerExpressionNode($1, $3);
                $$->name=nname;
                $$->node_number = n_nodes;
                ++n_nodes;
              

}
| factor {$$ = $1;}
;

factor : '(' relation_stmt ')'           { $$ = $2; }
        | ID                            { 
                                                std::string nname = "iden" + std::to_string(n_nodes);
                                                $1->node_number = n_nodes;
                                                ++n_nodes;
                                                $1->name=nname;
                                               IdentifierNode* temp1 = dynamic_cast<IdentifierNode*>($1);
                                               cout<<"IdentifierNode* temp1 "<<temp1->value<<" "<<temp1->name<<endl;
                                                $$ = $1;
                                        }
        | ID '[' arith_expression ']'       { 
                                                std::string nname = "array" + std::to_string(n_nodes);
                                                $1->node_number = n_nodes;
                                                ++n_nodes;
                                                $1->name=nname;
                                                $1->label = "Array";
                                                cout<<"ID arrray rule"<<endl;
                                                $3->node_number = n_nodes;
                                                ++n_nodes;
                                                std::string nname1 = "array_exp" + std::to_string(n_nodes);
                                                $3->name = nname1;
                                                $$->add($3);
                                              // $$->add($1);
                                                
                                        }   
        | NUMBER                        { $$ = $1; }

        | boolean                       { $$ = $1; }

        | STRING                        {
                                                std::string nname = "string" + std::to_string(n_nodes);
                                                $1->node_number = n_nodes;
                                                ++n_nodes;
                                                $1->name=nname;
                                                $$ = $1; 
                                                StringNode* temp1 = dynamic_cast<StringNode*>($1);
                                                string s = temp1->value.c_str();
                                                glbl_string.push_back(s);
                                        }
        | function_call {
                $$ = $1;
        }  
        | '~' factor    {
                std::string nname = "not" + std::to_string(n_nodes);
                ++n_nodes;
                $$ = new UnaryExpressionNode("~", $2);
                $$->name=nname;
        }          
; 

data_type   
        : INT { $$ = "INT";}
        | FLOAT {$$ = "FLOAT";}
        | BOOL {$$ = "BOOL";}
        | STR {$$ = "STR";}
        | NONE_TOK {$$ = "NONE";}
        | ID {
                IdentifierNode* temp = dynamic_cast<IdentifierNode*>($1);
                $$ = strdup(temp->value.c_str());
        
        }

;

%%

int main(int argc, char **argv)
{
        int input_flag =0, output_flag = false, tac_output_flag = false, symtab_output_flag = false, asm_output_flag = false;
        ofstream output_file;// Declare an output file stream
        ofstream tac_file;
        ofstream symtab_file;
        string asm_file;
        ofstream dump_file = ofstream("dump.txt");
        streambuf *coutbuf = cout.rdbuf();
        streambuf *tacbuf = tac_file.rdbuf();
        streambuf *symtabbuf = symtab_file.rdbuf();
        for (int i=0; i< argc; i++)
        {

                if (strcmp(argv[i], "-help") == 0)
                {
                        cerr<<"Execution: ./m2.out [-input <filename>] [-output <filename>] [-output3AC <filename>] [-ouputSymTab <filename>]\n";
                        cerr<<"Sample: ./a.out -input input.txt -output output.dot -output3AC output3AC.txt -outputSymTab outputSymTab.csv -outputASM asm.s\n";
                        cerr<<"The default input file is input.py, the default digraph output file is output.dot, the default 3AC output file is 3AC.txt, the default symbol table output file is symtab.csv and the default asm file is asm.s.\n";
                        cerr<<"The -verbose tag would give the detailed parser output \n";
                        cerr<<"To visualise AST: dot -Tpdf <outputfilename.dot> -o graphfilename.pdf\n";
                        cerr<<"Sample: dot -Tpdf output.dot -o output.pdf\n";
                        //cerr<<"A dump of the symbol table of each function as a CSV file on correct execution will be available in <>\n";
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

                else if (strcmp(argv[i], "-output3AC") == 0)
                {
                        tac_file.open(argv[i + 1]);
                        tac_output_flag = 1;
                }

                else if (strcmp(argv[i], "-outputSymTab") == 0)
                {
                        symtab_file.open(argv[i + 1]);
                        symtab_output_flag = 1;
                }

                else if(strcmp(argv[i], "-outputASM") == 0)
                {
                        asm_file = argv[i + 1];
                        asm_output_flag =1;
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

        if(tac_output_flag == 0)
        {
                cerr<< "Output file for 3AC is not specified, check -help\n";
                cerr<< "Using default 3AC output file 3AC.txt\n";
                tac_file.open("3AC.txt");
        }

        if(symtab_output_flag == 0)
        {
                cerr<< "Output file for symbol table is not specified, check -help\n";
                cerr<< "Using default symbol table output file symtab.csv\n";
                symtab_file.open("symtab.csv");
        }

        if(asm_output_flag == 0)
        {
                cerr<< "Output file for assembly code is not specified, check -help\n";
                cerr<< "Using default assembly output file asm.s\n";
                asm_file = "asm.s";
        }
        
        // else
        // {
        //         yyin=stdin;
        // }       
        vector<pair< string, string>> params_temp;
        entry_t self_entry = {"self", "POINTER", "FUNCTION", params_temp, NULL, 0, 0};
        params_temp.push_back(make_pair("arr", "ARRAY"));
        entry_t len_entry = {"len", "INT", "FUNCTION", params_temp, NULL, 0, 0};
        entry_t* temp;
        temp = symbol_table_insert(global_table, self_entry);
        temp = symbol_table_insert(global_table, len_entry);

        

        // if (root != NULL) 
        // {
        //         AST ast(root);
        //         ast.Print();
        // }
          // Redirect cout to the output file
        streambuf *backup = cout.rdbuf(dump_file.rdbuf());
       yyparse();
        root->generate_quad();
        root->update_jump();
        cout.rdbuf(backup); // Restore the original cout buffer
        streambuf *backup1 = cout.rdbuf(tac_file.rdbuf()); // Redirect cout to the output file
        root->print_tac();
        cout.rdbuf(backup1); // Restore the original cout buffer
        //streambuf *backup3 = cout.rdbuf(asm_file.rdbuf()); // Redirect cout to the output file
        gen->first_gen();
        // if(root==NULL){
        //         cout<<"Root is NULL"<<endl;
        // }
        gen->second_gen(root);
        gen->code_print(asm_file); 
        //cout.rdbuf(backup3); // Restore the original cout buffer
        if (root != NULL)
        {
                streambuf *backup = cout.rdbuf(output_file.rdbuf()); // Redirect cout to the output file
                AST ast(root);
                ast.Print(); // Print AST to the redirected cout
                cout.rdbuf(backup); // Restore the original cout buffer
        }
        //print_symbol_table(global_table);
        streambuf *backup2 = cout.rdbuf(symtab_file.rdbuf()); // Redirect cout to the output file
        print_symbol_table(global_table);
        cout.rdbuf(backup2); // Restore the original cout buffer
        return 0; 


//         if (argc > 1){
//                 for(int i=0;i<argc;i++)
//                 printf("value of argv[%d] = %s\n\n",i,argv[i]);

//                 yyin=fopen(argv[1],"r");
//         }
//         else
//                 yyin=stdin;
// vector<pair< string, string>> params_temp;
// entry_t main_entry = {"main", "INT", "FUNCTION", params_temp, NULL, 0, 0};
// entry_t len_entry = {"len", "INT", "FUNCTION", params_temp, NULL, 0, 0};
// entry_t* temp;
// temp = symbol_table_insert(global_table, main_entry);
// temp = symbol_table_insert(global_table, len_entry);

//         yyparse();
//        // root->print();
//         // AST is constructed, you can print it now
//         root->generate_quad();
//         root->update_jump();
//         root->print_tac();
//          /* if (root != NULL) {
//                  AST ast(root);
//                  ast.Print();
//          }
//          */
//         gen->first_gen();
//         // if(root==NULL){
//         //         cout<<"Root is NULL"<<endl;
//         // }
//         gen->second_gen(root);
//         gen->code_print("asm4.s"); 
//        print_symbol_table(global_table);
//        for (const auto& pair : classMap) {
//         std::cout << "Key: " << pair.first << ", Values: ";
//         for (string value : pair.second) {
//             std::cout << value << " ";
//         }
//         std::cout << std::endl;
//     }
//         return 0;
}

void yyerror(const char *msg) {
        printf(" %s at line %d\n", msg,yylineno);
}