#!/bin/bash

./m3.out -input ../tests/test1.py -output AST1.dot -output3AC 3AC1.txt -outputSymTab symtab1.csv -outputASM asm1.s
dot -Tpdf AST1.dot -o AST1.pdf
./m3.out -input ../tests/test2.py -output AST2.dot -output3AC 3AC2.txt -outputSymTab symtab2.csv -outputASM asm2.s
dot -Tpdf AST2.dot -o AST2.pdf
./m3.out -input ../tests/test3.py -output AST3.dot -output3AC 3AC3.txt -outputSymTab symtab3.csv -outputASM asm3.s
dot -Tpdf AST3.dot -o AST3.pdf
./m3.out -input ../tests/test4.py -output AST4.dot -output3AC 3AC4.txt -outputSymTab symtab4.csv -outputASM asm4.s
dot -Tpdf AST4.dot -o AST4.pdf
./m3.out -input ../tests/test5.py -output AST5.dot -output3AC 3AC5.txt -outputSymTab symtab5.csv -outputASM asm5.s
dot -Tpdf AST5.dot -o AST5.pdf
./m3.out -input ../tests/test6.py -output AST6.dot -output3AC 3AC6.txt -outputSymTab symtab6.csv -outputASM asm6.s
dot -Tpdf AST6.dot -o AST6.pdf
./m3.out -input ../tests/test7.py -output AST7.dot -output3AC 3AC7.txt -outputSymTab symtab7.csv -outputASM asm7.s
dot -Tpdf AST7.dot -o AST7.pdf
./m3.out -input ../tests/test8.py -output AST8.dot -output3AC 3AC8.txt -outputSymTab symtab8.csv -outputASM asm8.s
dot -Tpdf AST8.dot -o AST8.pdf
# ./m3.out -input ../tests/test9.py -output AST9.dot -output3AC 3AC9.txt -outputSymTab symtab9.csv -outputASM asm9.s
# dot -Tpdf AST9.dot -o AST9.pdf
# ./m3.out -input ../tests/test10.py -output AST10.dot -output3AC 3AC10.txt -outputSymTab symtab10.csv -outputASM asm10.s
# dot -Tpdf AST10.dot -o AST10.pdf

mkdir -p ../outputDOT
mv AST*.dot ../outputDOT

mkdir -p ../outputAST
mv AST*.pdf ../outputAST

mkdir -p ../output3AC
mv 3AC*.txt ../output3AC

mkdir -p ../outputSymTab
mv symtab*.csv ../outputSymTab

mkdir -p ../outputASM
mv asm*.s ../outputASM

