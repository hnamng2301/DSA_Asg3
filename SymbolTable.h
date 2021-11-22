#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Symbol{
    string identifier;
    int key;
    int numOfParameters;
    int scope;
public:
    Symbol(){}
    Symbol(string identifier, int value, int scope){
        this->identifier = identifier;
        this->numOfParameters = value;
        this->scope = scope;
        string key = "";
        for(int i = 0; i < int(identifier.size()); i++){
            key += to_string(identifier[i] - 48);
        }
        key = to_string(scope) + key;
        this->key = stoi(key);
    }
    friend class SymbolTable;
};

class SymbolTable
{
    Symbol** symbolTable;
    int count;
public:
    SymbolTable() {
        this->symbolTable = nullptr;
        this->count = 0;
    }
    void initArray(int size);
    unsigned long hashFunction(int, int);
    unsigned long subHashFunction(int, int);
    void insert(string, string, int, int, int, int&);
    void run(string filename);
};

bool checkAction(string action);                    // Check first code is INSERT, ASSIGN, ...
void splitFirstLine(string line, string arg[]);     // Split line / tokenize string
void checkLineValid(string, string, string arg[]);  // Check line is valid
bool validID(string lineName);                      // Check variable's is valid
bool isNum(string lineValue);                       // Check value to assign is valid number
bool isString(string lineValue);                    // Check value to assign is valid string

#endif