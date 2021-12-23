#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Symbol{
    string identifier;
    unsigned long long int key;
    int numOfParameters;
    int scope;
    string type;
public:
    Symbol(){}
    Symbol(string identifier, int value, int scope){
        this->identifier = identifier;
        this->numOfParameters = value;
        this->scope = scope;
        this->type = "";
        string res = "";
        for(int i = 0; i < int(identifier.size()); i++){
            res += to_string(identifier[i] - 48);
        }
        res = to_string(scope) + res;
        this->key = stoll(res);
    }
    friend class SymbolTable;
};

class SymbolTable
{
    Symbol** symbolTable;
    int count;
    int size;
public:
    SymbolTable() 
    {
        this->symbolTable = nullptr;
        this->count = 0;
        this->size = 0;
    }
    ~SymbolTable()
    {
        for (int i = 0; i < this->size; i++)
        {
            delete symbolTable[i];
            symbolTable[i] = nullptr;
        }
        delete[] symbolTable;
        symbolTable = nullptr;
        this->count = 0;
        this->size = 0;
    }
    void initArray(int size);
    unsigned long long hashFunction(unsigned long long int, int);    // main hash function
    unsigned long long subHashFunction(unsigned long long, int);     // sub hash function for DOUBLE type
    void insert(string, string, int, int, int, int&);           // INSERT
    void assign(string, string, int, int, int, int);            // ASSIGN
    void call(string, string, int, int, int, int);              // CALL
    void exitScope(int&, int);                                  // END
    void look_up(string, string, int, int, int, int);           // LOOKUP
    string result(int);                                         // Take all existing ID in this scope
    void printTable(string);                                    // PRINT
    void run(string filename);                                  // main function
};

bool checkAction(string action);                            // Check first code is INSERT, ASSIGN, ...
void splitFirstLine(string line, string arg[]);             // Split first line
void splitLine(string line, string arg[], int numOfWords);  // Split line / tokenize string
bool validID(string lineName);                              // Check variable's is valid
bool isNum(string lineValue);                               // Check value to assign is valid number
bool isString(string lineValue);                            // Check value to assign is valid string
unsigned long long idToKey(string identifier, int scope);   // Change identifier to key

#endif
