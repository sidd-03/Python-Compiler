%option noyywrap
%option yylineno
%{
#include "ast.hpp"
#include "make_tac.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include "parser.hpp"
#pragma warning(disable : 4005)
using namespace std;
#define MAX_SIZE 10000

int debug = 0;
void push();
void pop();
void display();
 
int top = -1;
int stack_array[MAX_SIZE];

void push(int value)
{
    if (top == MAX_SIZE - 1)
    {
        if (debug) printf("Stack is full\n");
    }
    else
    {
        top++;
        stack_array[top] = value;
    }
}
void pop()
{
    if (top == -1)
    {
        if (debug) printf("Stack is empty\n");
    }
    else
    {
        if (debug) printf("Popped element is %d\n", stack_array[top]);
        top--;
    }
}
void display()
{
    if (top == -1)
    {
        if (debug) printf("Stack is empty\n");
    }
    else
    {
        if (debug) printf("Stack elements are:\n");
        for (int i = top; i >= 0; i--)
        {
            if (debug) printf("%d\n", stack_array[i]);
        }
    }
}
vector<float> parseNumbers(const string& input) {
    vector<float> numbers;
    string strippedInput = input.substr(1, input.size() - 2); // Remove '[' and ']'
    istringstream iss(strippedInput);
    string token;

    // Split the input string by commas
    while (getline(iss, token, ',')) {
        // Convert the token to an integer and add it to the vector
        numbers.push_back(stod(token));
    }

    return numbers;
}
#define keyword_size 32
char keywords[keyword_size];

#define MAX 1000
char buffer[MAX];
char *buffer_ptr ;

int current_indentation = 0;
int spaces = 0;



%}
newLine [\n]
ws [ \t]
whiteSpaces {ws}+
digit [0-9]
letter [a-zA-Z]
id ({letter}|"_")({letter}|{digit}|"_")*
identifier  {id}("."{id})*
integer            -?{digit}+
float              -?{digit}+"."{digit}+
num               {integer}|{float}
LEFT_BRACKET    "\\["
RIGHT_BRACKET   "\\]"
keyword     "yield"|"with"|"try"|"True"|"return"|"raise"|"pass"|"or"|"and"|"not"|"nonlocal"|"None"|"lambda"|"is"|"in"|"import"|"if"|"else"|"global"|"from"|"for"|"finally"|"False"|"except"|"def"|"assert"|"break"|"class"|"continue"|"del"|"elif"|"while"
comment_1  #.*
comment_2   \'{3}[^\'{3}]*\'{3}
comment_3   \"{3}[^\"{3}]*\"{3}
comments    ({comment_1}|{comment_2}|{comment_3})
separator    [:,.;]
COMMA       ","
literal     (\"([^\"]|\\.)*\")|(\'([^\']|\\.)*\')
literal_2   ({keyword}" "|(("["|"{"|"("|",")|"="{ws}*))
three_single_quotes  \'{3}
three_double_quotes  \"{3}
array ({LEFT_BRACKET}{num}({COMMA}{num})*{RIGHT_BRACKET})
%x DOUBLE_STRING
%x SINGLE_STRING
%x COMMENT_STRING
%x COMMENT_STRING_DOUBLE
%x INDENTSTATE
%x DEDENTALLSTATE
%x DEDENTSTATE
%x NEWLINESTATE
%x LINESTATE
%%
\n[ \t]*$ {if(debug) printf("empty_line\n");}
":" {
    BEGIN LINESTATE;
    if (debug) printf("SEPARATOR\n");
     yylval.strval = strdup(yytext); return yytext[0];
}
<LINESTATE>{newLine}{whiteSpaces} {
    BEGIN INDENTSTATE;
    if(top<0) {
        push(0);
    }
    spaces = 0;
    for(int i=0 ; i< strlen(yytext); i++) {
        if(yytext[i] == ' ') {
            spaces++;
        }
        else if (yytext[i] == '\t') {
            spaces += 4;
        }
    }
    push(spaces);
    current_indentation = spaces;
    unput(' ');
    if (debug) printf("NEWLINE\n");
    return NEWLINE;
    // Add return 

}
<LINESTATE>\n[ \t]*$ {if(debug) printf("empty_line\n");}
<LINESTATE>. {BEGIN INITIAL;}

{newLine}{whiteSpaces} {
    if(top<0) {
        push(0);
    }
    spaces = 0;
    for(int i=0 ; i< strlen(yytext); i++) {
        if(yytext[i] == ' ') {
            spaces++;
        }
        else if (yytext[i] == '\t') {
            spaces += 4;
        }
    }
    current_indentation = spaces;
    unput(' ');
    BEGIN (DEDENTSTATE);
    if (debug) printf("NEWLINE\n");
    return NEWLINE;
    // Add return
}

{newLine} {
    BEGIN (DEDENTALLSTATE);
    unput(' ');
    if (debug) printf("NEWLINE\n");
    return NEWLINE;
    // Add return
}
{newLine}{comments} {}

<<EOF>>     {
BEGIN(NEWLINESTATE);
unput(' ');
if(stack_array[top] == 0){
   return 0;
}
}

{ws}  {}

\\$                     {}
@.*                     {}
{separator}             {
    if (debug) printf("SEPARATOR\n");
     yylval.strval = strdup(yytext); return yytext[0];
      }  
"["{ws}*({num}{ws}*,{ws}*)*{num}{ws}*"]"  {
if(debug) printf("ARRAY\n");
string text(yytext); 
    vector<float> numbers = parseNumbers(text);
    yylval.astNode = new IdentifierNode("ARRAY", "List", yytext, numbers, yylineno);
return ARRAY;
}        
{comments}               {  }
"="                     {
    yylval.strval = strdup(yytext);
     if (debug) printf("ASSIGN\n");
     return yytext[0];
     }
"+"|"-"|"*"|"/"|"_"|"|"|"&"|"%"|"^"|"~" {
    yylval.strval = strdup(yytext); 
    if (debug) printf("OPERATOR\n");
    return yytext[0];
    }
"//" {
    yylval.strval = strdup(yytext); 
    if (debug) printf("OPERATOR\n");
    return FLOORDIV;
}
"**" {
    yylval.strval = strdup(yytext); 
    if (debug) printf("OPERATOR\n");
    return POWER;
}
"{"|"}"|"("|")" {
    yylval.strval = strdup(yytext); 
    if (debug) printf("BRACKET\n");
    return yytext[0];
    } 
"[" {
    yylval.strval = strdup(yytext); 
    if (debug) printf("OPEN BRACKET\n");
    return yytext[0];
    } 
"]" {
    yylval.strval = strdup(yytext); 
    if (debug) printf("CLOSE BRACKET\n");
    return yytext[0];
    }     
 
"def"                   {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("DEF\n");
                            return DEF;
                        }

"if"                    {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("IF\n");
                            return IF;
}
"elif"                  {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("ELIF\n");
                            return ELIF;}

"else"                  {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("ELSE\n");
                            return ELSE;
                            
                        }

"break"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("BREAK\n");
                            return BREAK;
                        }

"continue"              {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("CONTINUE\n");
                            return CONTINUE;
                        }

"global"                {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("GLOBAL\n");
                            return GLOBAL;
                        }

"nonlocal"              {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("NONLOCAL\n");
                            return NONLOCAL;
                        }


"return"                {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("RETURN\n");
                            return RETURN;
                        }

"<"                     {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("LESS THAN\n");
                            return LT;
                        }

">"                     {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("GREATER THAN\n");
                            return GT;
                        }
">>"                     {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("RIGHT SHIFT\n");
                            return RSHIFT;
                        }
"<<"                     {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("LEFT SHIFT\n");
                            return LSHIFT;
                        }
"<="                    {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("LESS THAN EQUAL\n");
                            return LTE;
                        }

">="                    {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("GREATER THAN EQUAL\n");
                            return GTE;
                        }

"=="                    {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("EQUAL\n");
                            return EQUAL;
                        }
 "!="                   {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("NOT EQUAL\n");
                            return NOTEQUAL;
                        }   

"while"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("WHILE\n");
                            return WHILE;
                        }

"for"                   {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("FOR\n");
                            return FOR;
                        }

"in"                    {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("IN\n");
                            return IN;
                        }

"range"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("RANGE\n");
                            return RANGE;
                        }

"print"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Print statement found: %s\n", yytext);
                             return PRINT;
                        }

"input"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Input statement found: %s\n", yytext);
                             return INPUT;
                        }

"class"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Class keyword found: %s\n", yytext);
                             return CLASS;
                        }

"try"                   {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Try keyword found: %s\n", yytext);
                             return TRY;
                        }

"except"                {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Except keyword found: %s\n", yytext);
                             return EXCEPT;
                        }

"match"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Match keyword found: %s\n", yytext);
                             return MATCH;
                        }

"case"                  {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Case keyword found: %s\n", yytext);
                             return CASE;
                        }

"as"                    {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("As keyword found: %s\n", yytext);
                             return AS;
                        }

"int"                   {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Int keyword found: %s\n", yytext);
                             return INT;
                        }

"float"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Float keyword found: %s\n", yytext);
                             return FLOAT;
                        }

"str"                   {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Str keyword found: %s\n", yytext);
                             return STR;
                        }

"bool"                  {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Bool keyword found: %s\n", yytext);
                             return BOOL;
                        }

"list"                  {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("List keyword found: %s\n", yytext);
                             return LIST;
                        }

"pass"                  {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Pass keyword found: %s\n", yytext);
                             return PASS;
                        }

"or"                    {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Or keyword found: %s\n", yytext);
                             return OR;
                        }

"and"                   {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("And keyword found: %s\n", yytext);
                             return AND;
                        }
"not"                   {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("Not keyword found: %s\n", yytext);
                             return NOT;
                        }

"True"                  {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("True keyword found: %s\n", yytext);
                             return TRUE_TOK;
                        }

"False"                 {
                            yylval.strval = strdup(yytext);
                            if (debug) printf("False keyword found: %s\n", yytext);
                             return FALSE_TOK;
                        }
"None"                  {   
                            yylval.astNode = new IdentifierNode("NONE", "None", yytext, yylineno);
                           // yylval.strval = strdup(yytext);
                            if (debug) printf("None keyword found: %s\n", yytext);
                             return NONE_TOK;
}

"__init__" {
                          yylval.astNode = new IdentifierNode("CONSTRUCTOR", "Constructor", yytext, yylineno);
                            if (debug) printf("Constructor found: %s\n", yytext);
                             return CONSTR;
                   }

{identifier}            { char* dotPosition = strchr(yytext, '.');
                            int is_dotted = 0;
                            if (dotPosition != NULL) {
                                is_dotted = 1;
                                memmove(dotPosition, dotPosition + 1, strlen(dotPosition) + 1);
                            }

                           yylval.astNode = new IdentifierNode("IDENTIFIER", "Identifier", yytext, yylineno , is_dotted);
                            if (debug) printf("ID: %s\n", yytext);
                             return ID;
                        }             

{integer}       {
                            yylval.astNode = new NumberNode("INT", "int", atoi(yytext));
                            if (debug) printf("INTEGER: %s\n", yytext);
                             return INT_NUMBER;
                        }
{float}         {
                            yylval.astNode = new NumberNode("FLOAT", "float", atoi(yytext));
                            if (debug) printf("FLOAT: %s\n", yytext);
                             return FLOAT_NUMBER;
                        }
<INDENTSTATE>[ ] {
        BEGIN(INITIAL);
        if (debug) printf("INDENT\n");
        return INDENT;
    }

<DEDENTSTATE>[ ] {
       int flag = 1;
       for(int i= 0 ; i<=top; i++) {
           if( current_indentation == stack_array[i]) {
               flag = 0;
           }
       }
       if(flag == 1) {
           if (debug) printf("INDENTERROR\n");
            return INDENTERROR;
       }
        {
            int end = 1;
            while(current_indentation < stack_array[top]) {
                unput(' ');
                BEGIN(DEDENTSTATE);
                end = 0;
                pop();
                if (debug) printf("DEDENT\n");
                return DEDENT;
            }
            if(end == 1) {
                BEGIN(INITIAL);
            }
           
       }
    }
<DEDENTALLSTATE>[ ]     {
        if(top < 0 ) push(0);
        if(stack_array[top] > 0) {
            while(top > 0) {
                unput(' ');
                BEGIN(DEDENTALLSTATE);
                pop();
               if (debug) printf("DEDENT\n");
                return DEDENT;
            }
        }
            else {
                BEGIN(INITIAL);
            }
        
    }

<NEWLINESTATE>[ ]     {
        BEGIN(DEDENTALLSTATE);
        unput(' ');
        if (debug) printf("NEWLINE\n");
        return NEWLINE;
    }
\"  {
    buffer_ptr = buffer;
    BEGIN(DOUBLE_STRING);
    }

<DOUBLE_STRING>\" {
    {*buffer_ptr = '\0'; BEGIN(INITIAL);
    yylval.astNode = new StringNode("STRING", "string", buffer);
      if (debug) printf("STRING\n");
      return STRING;} 
}

<DOUBLE_STRING>\\\n {
    // This rule is empty, as it's just for line continuation, and it doesn't perform any action.
}

<DOUBLE_STRING>\n {
    if (debug) printf("error in line %d\n", yylineno);
    BEGIN(INITIAL);
}

<DOUBLE_STRING>\\n {
    *buffer_ptr++ = '\n';
}

<DOUBLE_STRING>\\t {
    *buffer_ptr++ = '\t';
}

<DOUBLE_STRING>\\r {
    *buffer_ptr++ = '\r';
}

<DOUBLE_STRING>\\b {
    *buffer_ptr++ = '\b';
}

<DOUBLE_STRING>\\f {
    *buffer_ptr++ = '\f';
}

<DOUBLE_STRING>\\\" {
    *buffer_ptr++ = '\"';
}

<DOUBLE_STRING>[^\\\n\"]+ {
    char *yptr = yytext;
    while (*yptr)
        *buffer_ptr++ = *yptr++;
}

\' {
    buffer_ptr = buffer;
    BEGIN(SINGLE_STRING);
    }

<SINGLE_STRING>\' {
    *buffer_ptr = '\0';
    BEGIN(INITIAL);
    yylval.astNode = new StringNode("STRING", "string", buffer);
    if (debug) printf("STRING\n");
    return STRING;
}

<SINGLE_STRING>\\\n {
    // This rule is empty, as it's just for line continuation, and it doesn't perform any action.
}

<SINGLE_STRING>\n {
    if (debug) printf("line%d, ERROR: new line in string\n", yylineno);
    BEGIN(INITIAL);
}

<SINGLE_STRING>\\n {
    *buffer_ptr++ = '\n';
}

<SINGLE_STRING>\\t {
    *buffer_ptr++ = '\t';
}

<SINGLE_STRING>\\r {
    *buffer_ptr++ = '\r';
}

<SINGLE_STRING>\\b {
    *buffer_ptr++ = '\b';
}

<SINGLE_STRING>\\f {
    *buffer_ptr++ = '\f';
}

<SINGLE_STRING>\\\' {
    *buffer_ptr++ = '\'';
}
<SINGLE_STRING>[^\\\n\']+ {
    char *yptr = yytext;
    while (*yptr)
        *buffer_ptr++ = *yptr++;
}
{literal_2}{three_single_quotes} {
    int i = 0;
    strcpy(keywords, " ");

    while(yytext[i] != ' '){
        if(yytext[i]== '\'') break;

        keywords[i] = yytext[i];
        i++;
    }

    buffer_ptr = buffer; BEGIN(COMMENT_STRING);

    if(strcmp(keywords, "=")==0) {
        if(debug) cout<<yytext[0]<<endl; 
        return yytext[0];}
    else if (strcmp(keywords, "(")==0 || strcmp(keywords,"{")==0 || strcmp(keywords, "[")==0){
        if(debug) cout<<yytext[0]<<endl;
        return yytext[0];
    }
    else {
        if( strcmp(keywords, "return") == 0) {
            if(debug) cout<<"RETURN\n";  
               return RETURN;}
    }
}
<COMMENT_STRING>"'''" {
    *buffer_ptr = '\0';
    BEGIN(INITIAL);
    yylval.astNode = new StringNode("STRING", "string", buffer);
    if(debug) cout<<"STRING"<<endl;
    return STRING;
}

<COMMENT_STRING>\\\n {
    // This rule is empty, as it's just for line continuation, and it doesn't perform any action.
}

<COMMENT_STRING>\n {
    *buffer_ptr++ = '\n';
}

<COMMENT_STRING>\\n {
    *buffer_ptr++ = '\n';
}

<COMMENT_STRING>\\t {
    *buffer_ptr++ = '\t';
}

<COMMENT_STRING>\\r {
    *buffer_ptr++ = '\r';
}

<COMMENT_STRING>\\b {
    *buffer_ptr++ = '\b';
}

<COMMENT_STRING>\\f {
    *buffer_ptr++ = '\f';
}

<COMMENT_STRING>[^\\\n"''']+ {
    char *yptr = yytext;
    while (*yptr)
        *buffer_ptr++ = *yptr++;
}

{literal_2}{three_double_quotes} {
    int i=0;
    strcpy(keywords, " ");

    while(yytext[i] != ' '){
        if(yytext[i]== '\"') break;

        keywords[i] = yytext[i];
        i++;
    }

    buffer_ptr = buffer; BEGIN(COMMENT_STRING_DOUBLE);

    if(strcmp(keywords, "=")==0) {
        if(debug) cout<<yytext[0]<<endl;
    return yytext[0];}
    else if (strcmp(keywords, "(")==0 || strcmp(keywords,"{")==0 || strcmp(keywords, "[")==0){
        if(debug) cout<<yytext[0]<<endl;
        return yytext[0];
    }
    else {
        if( strcmp(keywords, "return") == 0) 
        if(debug) cout<<"RETURN\n";
        return RETURN;
    }
}
<COMMENT_STRING_DOUBLE>\"\"\" {
    *buffer_ptr = '\0';
    BEGIN(INITIAL);
   yylval.astNode = new StringNode("STRING", "string", buffer);
   if(debug) cout<<"STRING"<<endl;
    return STRING;
}

<COMMENT_STRING_DOUBLE>\\\n {
    // This rule is empty, as it's just for line continuation, and it doesn't perform any action.
}

<COMMENT_STRING_DOUBLE>\n {
    *buffer_ptr++ = '\n';
}

<COMMENT_STRING_DOUBLE>\\n {
    *buffer_ptr++ = '\n';
}

<COMMENT_STRING_DOUBLE>\\t {
    *buffer_ptr++ = '\t';
}

<COMMENT_STRING_DOUBLE>\\r {
   *buffer_ptr++ = '\r';
}

<COMMENT_STRING_DOUBLE>\\b {
    *buffer_ptr++ = '\b';
}

<COMMENT_STRING_DOUBLE>\\f {
    *buffer_ptr++ = '\f';
}

<COMMENT_STRING_DOUBLE>[^\\\n\\"{3}]+ {
    char *yptr = yytext;
    while (*yptr)
        *buffer_ptr++ = *yptr++;
}
. {
    if (debug) printf("error in %d is %s",yylineno,yytext);
    }
%%
