#include "SymbolTable.h"

bool validID(string lineName)
{
    // Kiem tra ID hop le
    if (lineName[0] >= 'a' && lineName[0] <= 'z')
    { // Phai bat dau bang ky tu thuong
        for (unsigned int i = 0; i < lineName.size(); i++)
        {
            if ((lineName[i] >= 'A' && lineName[i] <= 'Z') || (lineName[i] >= 'a' && lineName[i] <= 'z') 
                || (lineName[i] >= '0' && lineName[i] <= '9') || lineName[i] == '_')
                continue;
            else
                return false;
        }
    }
    else
        return false;
    return true;
}
bool isNum(string lineValue)
{
    regex numConst("\\d+"); // Kiem tra const number (string chi gom cac chu so)
    if (lineValue[0] == '\'' && lineValue[lineValue.size() - 1] == '\'')
        return false;
    if (regex_match(lineValue, numConst))
        return true;
    else
        return false;
    return true;
}
bool isString(string lineValue)
{
    regex strConst("^'([a-zA-Z0-9\\s]+)'$"); // Kiem tra const string

    if (lineValue[0] == '\'' && lineValue[lineValue.size() - 1] == '\'')
    { // Kiem tra value co la string khong
        if (regex_match(lineValue, strConst))
            return true;
        else
            return false;
    }
    else
        return false;
    return true;
}
bool checkAction(string action)
{
    if (action != "LINEAR" && action != "INSERT" && action != "ASSIGN" && action != "PRINT"
        && action != "BEGIN" && action != "END" && action != "CALL" && action != "LOOKUP")
            return false;
    return true;
}

void splitFirstLine(string line, string arg[])
{
    regex reg(" +(?=(?:[^\']*\'[^\']*\')*[^\']*$)");
    regex_token_iterator<string::iterator> iter(line.begin(), line.end(), reg, -1);
    regex_token_iterator<string::iterator> end;
    int i = 0;
    while (iter != end)
    {
        if(i >= 3 && (arg[0] == "LINEAR" || arg[0] == "DOUBLE"))
            throw InvalidInstruction(line);
        else if(i >= 4 && arg[0] == "QUADRATIC")
            throw InvalidInstruction(line);
        arg[i] = *iter++;
        i++;
    }

    if (arg[0] == "LINEAR"){
        if (!isNum(arg[1]) || !isNum(arg[2]))
            throw InvalidInstruction(line);
    }
    else if(arg[0] == "QUADRATIC"){
        if (!isNum(arg[1]) || !isNum(arg[2]))
            throw InvalidInstruction(line);
    }
    else if(arg[0] == "DOUBLE"){
        if (!isNum(arg[1]) || !isNum(arg[2]) || !isNum(arg[3]))
            throw InvalidInstruction(line);
    }
}

void SymbolTable::initArray(int size){
    this->symbolTable = new Symbol*[size];
    for (int i = 0; i < size; i++){
        symbolTable[i] = nullptr;
    }
}

unsigned long SymbolTable::hashFunction(int key, int size) {
    return key % size;
}

unsigned long SymbolTable::subHashFunction(int key, int size) {
    return 1 + (key % (size - 2));
}

void SymbolTable::insert(string line, string type, int firstConst, int secondConst, int size , int& scope){
    string word[3] = {"", "", ""};
    regex reg(" +(?=(?:[^\']*\'[^\']*\')*[^\']*$)");
    regex_token_iterator<string::iterator> iter(line.begin(), line.end(), reg, -1);
    regex_token_iterator<string::iterator> end;
    int i = 0;
    while (iter != end)
    {
        if(i >= 3)
            throw InvalidInstruction(line);
        word[i] = *iter++;
        i++;
    }

    int numsteps = 0;
    string name = word[1];
    string value = word[2];
    if (!validID(name))
        throw InvalidInstruction(line);
    // If insert a function type variable in scope != 0
    if (scope != 0 && value != ""){
        throw InvalidInstruction(line);
    }

    int num_of_parameters = -1;
    if (value != ""){
        if (isNum(value))
            num_of_parameters = stoi(value);
        else throw InvalidInstruction(line);
    }

    Symbol* symbol = new Symbol(name, num_of_parameters, scope);
    if (type == "LINEAR"){
        int index = hashFunction(symbol->key, size);

        if (this->count == size - 1){
            throw Overflow(line);
        }
        else{
            while(symbolTable[index] != NULL){
                if (symbolTable[index] != NULL)
                    if(symbolTable[index]->key == symbol->key && symbolTable[index]->scope == symbol->scope)
                        throw Redeclared(line);
                index += firstConst;
                numsteps += firstConst;
                index = index % size;
            }

            symbolTable[index] = symbol;
            cout << numsteps << endl;
            this->count++;
        }
    }
    else if (type == "QUADRATIC"){
        int index = hashFunction(symbol->key, size);
        
        if (this->count == size - 1){
            throw Overflow(line);
        }
        else{
            int i = 1;
            while(symbolTable[index] != NULL){
                if (symbolTable[index] != NULL)
                    if(symbolTable[index]->key == symbol->key && symbolTable[index]->scope == symbol->scope)
                        throw Redeclared(line);
                index += firstConst * i + secondConst * i * i;
                numsteps += firstConst;
                index = index % size;
                i++;
            }
            symbolTable[index] = symbol;
            this->count++;
        }
    }
    else if (type == "DOUBLE"){
        int index = hashFunction(symbol->key, size);
        
        if (this->count == size - 1){
            throw Overflow(line);
        }
        else{
            while(symbolTable[index] != NULL){
                if (symbolTable[index] != NULL)
                    if(symbolTable[index]->key == symbol->key && symbolTable[index]->scope == symbol->scope)
                        throw Redeclared(line);
                index += firstConst * subHashFunction(symbol->key, size);
                numsteps += firstConst;
                index = index % size;
            }
            symbolTable[index] = symbol;
            this->count++;
        }      
    }
}

void SymbolTable::run(string filename)
{
    ifstream file;
    file.open(filename);
    string firstLine;
    getline(file, firstLine);
    string hashType[4] = {"", "", "" , ""};
    splitFirstLine(firstLine, hashType);
    const int hashSize = stoi(hashType[1]);
    initArray(hashSize);

    // Check first line
    int firstConst = -1, secondConst = -1;
    if (hashType[2] != ""){
        if (isNum(hashType[2]))
            firstConst = stoi(hashType[2]);
        else throw InvalidInstruction(firstLine);
    } 
    if (hashType[3] != "") {
        if (isNum(hashType[3]))
            secondConst = stoi(hashType[3]);
        else throw InvalidInstruction(firstLine);
    }
    
    int scope = 0;

    while (!file.eof()){
        string line = "";
        getline(file, line);
        string code = line.substr(0, line.find(' '));
        if (!checkAction(code)) throw InvalidInstruction(line);

        if (code == "INSERT"){
            this->insert(line, hashType[0], firstConst, secondConst, hashSize, scope);
        }
        else if (code == "ASSIGN"){}
        else if (code == "BEGIN"){}
        else if (code == "END"){}
        else if (code == "LOOKUP"){}
        else if (code == "CALL"){}
        else if (code == "PRINT"){}
        else {
            throw InvalidInstruction(line);
        }
    }
    file.close();
}