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
unsigned long long idToKey(string identifier, int scope){
    string res = "";
    for (int i = 0; i < int(identifier.size()); i++)
    {
        res += to_string(identifier[i] - 48);
    }
    res = to_string(scope) + res;
    return stoll(res);
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
void splitLine(string line, string arg[], int numOfWords){
    regex reg(" +(?=(?:[^\']*\'[^\']*\')*[^\']*$)");
    regex_token_iterator<string::iterator> iter(line.begin(), line.end(), reg, -1);
    regex_token_iterator<string::iterator> end;
    int i = 0;
    while (iter != end)
    {
        if(i >= numOfWords)
            throw InvalidInstruction(line);
        arg[i] = *iter++;
        i++;
    }
}

void SymbolTable::initArray(int size){
    this->symbolTable = new Symbol*[size];
    for (int i = 0; i < size; i++){
        symbolTable[i] = nullptr;
    }
}

unsigned long SymbolTable::hashFunction(unsigned long long key, int size) {
    return key % size;
}

unsigned long SymbolTable::subHashFunction(unsigned long long key, int size) {
    return 1 + (key % (size - 2));
}

void SymbolTable::insert(string line, string type, int firstConst, int secondConst, int size , int& scope){
    string word[3] = {"", "", ""};
    splitLine(line, word, 3);

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
        // cout << "Symbol " << symbol->key << " ";
        // cout << "Index: "<< index << " " << endl;
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

void SymbolTable::assign(string line, string type, int firstConst, int secondConst, int size , int& scope){
    string word[3] = {"", "", ""};
    splitLine(line, word, 3);

    if (word[0] + ' ' + word[1] + ' ' + word[2] != line) throw InvalidInstruction(line);
    string varToAssign = word[1];
    string valToAssign = word[2];
    if (!validID(varToAssign)) throw InvalidInstruction(line);

    int numsteps = 0;
    if (type == "LINEAR"){
        int index = hashFunction(idToKey(varToAssign, scope), size);

        bool isDeclared = false;
        while (symbolTable[index] != NULL)
        {
            if (symbolTable[index] != NULL && symbolTable[index]->identifier == varToAssign)
            {
                isDeclared = true;
                break;
            }
            index += firstConst;
            numsteps += firstConst;
            index = index % size;
        }
        if (!isDeclared)
            throw Undeclared(line);

        if (symbolTable[index]->type == "" && isNum(valToAssign)){
            // ASSIGN <ID> <number value>
            symbolTable[index]->type = "number";
        }
        else if(symbolTable[index]->type == "" && isString(valToAssign)){
            // ASSIGN <ID> <string value>
            symbolTable[index]->type = "string";
        }
        else if(validID(valToAssign)){
            // ASSIGN <ID> <ID>
            int index_val = hashFunction(idToKey(valToAssign, scope), size);
            bool valDeclared = false;
            while (symbolTable[index_val] != NULL)
            {
                if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valToAssign)
                {
                    valDeclared = true;
                    break;
                }
                index_val += firstConst;
                numsteps += firstConst;
                index_val = index_val % size;
            }
            if (!valDeclared)
                throw Undeclared(line);
            if (symbolTable[index_val]->type == "")
                throw TypeCannotBeInfered(line);
            else if (symbolTable[index_val]->type != "" && symbolTable[index]->type != ""){
                if (symbolTable[index_val]->type != symbolTable[index]->type)
                    throw TypeMismatch(line);
            }
            else{
                symbolTable[index]->type = symbolTable[index_val]->type;
            }
        }
        else if (valToAssign.find('(') && valToAssign.find(')')){
            // ASSIGN <ID> <Function>
            // A function's type has not been defined, so that varID had to have type.

            size_t pos = valToAssign.find('(');
            string valName = valToAssign.substr(0, pos);

            // First, check valID and varID are declared?
            int index_val = hashFunction(idToKey(valName, scope), size);
            int index_var = hashFunction(idToKey(varToAssign, scope), size);
            bool valDeclared = false;
            bool varDeclared = false;
            while (symbolTable[index_val] != NULL)
            {
                if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valName)
                {
                    valDeclared = true;
                    break;
                }
                index_val += firstConst;
                numsteps += firstConst;
                index_val = index_val % size;
            }

            while (symbolTable[index_var] != NULL)
            {
                if (symbolTable[index_var] != NULL && symbolTable[index_var]->identifier == varToAssign)
                {
                    varDeclared = true;
                    break;
                }
                index_var += firstConst;
                numsteps += firstConst;
                index_var = index_var % size;
            }

            if (!valDeclared)
                throw Undeclared(line);

            // If varID's type has not been defined
            // if (varDeclared && symbolTable[index_var]->type == "")
            //     throw TypeCannotBeInfered(line);
            
            string listParam = valToAssign.substr(pos + 1, valToAssign.length() - pos - 2);  // sum(5,x) -> 5,x
            size_t p = string::npos;
            int countParam = symbolTable[index_val]->numOfParameters;
            // cout << countParam << endl;
            while(p = listParam.find(',') != string::npos){
                string param = listParam.substr(0,p);
                if (param != "")
                    countParam--;
                else throw TypeMismatch(line);
                if (validID(param)){
                    int index_param = hashFunction(idToKey(param, scope), size);
                    bool paramDeclared = false;
                    while (symbolTable[index_param] != NULL)
                    {
                        if (symbolTable[index_param] != NULL && symbolTable[index_param]->identifier == param)
                        {
                            paramDeclared = true;
                            break;
                        }
                        index_param += firstConst;
                        numsteps += firstConst;
                        index_param = index_param % size;
                    }
                    if (!paramDeclared) throw Undeclared(line);
                    if (symbolTable[index_param]->numOfParameters != -1)
                        throw TypeMismatch(line);
                    if (symbolTable[index_param]->type == "")
                        throw TypeCannotBeInfered(line);
                }
                listParam.erase(0, p + 1);
            }
            if (countParam != 1) throw TypeMismatch(line);
            if (varDeclared && symbolTable[index_var]->type == "")
                throw TypeCannotBeInfered(line);

            symbolTable[index_val]->type = symbolTable[index_var]->type;
        }
        else throw InvalidInstruction(line);
        cout << numsteps << endl;
    }
    else if (type == "QUADRATIC"){
        int index = hashFunction(idToKey(varToAssign, scope), size);

        bool isDeclared = false;
        int i = 1;
        while (symbolTable[index] != NULL)
        {
            if (symbolTable[index] != NULL && symbolTable[index]->identifier == varToAssign)
            {
                isDeclared = true;
                break;
            }
                index += firstConst * i + secondConst * i * i;
                numsteps += firstConst;
                index = index % size;
                i++;
        }
        if (!isDeclared)
            throw Undeclared(line);

        if (symbolTable[index]->type == "" && isNum(valToAssign)){
            // ASSIGN <ID> <number value>
            symbolTable[index]->type = "number";
        }
        else if(symbolTable[index]->type == "" && isString(valToAssign)){
            // ASSIGN <ID> <string value>
            symbolTable[index]->type = "string";
        }
        else if(validID(valToAssign)){
            // ASSIGN <ID> <ID>
            int index_val = hashFunction(idToKey(valToAssign, scope), size);
            bool valDeclared = false;
            int i_val = 1;
            while (symbolTable[index_val] != NULL)
            {
                if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valToAssign)
                {
                    valDeclared = true;
                    break;
                }
                index_val += firstConst * i_val + secondConst * i_val * i_val;
                numsteps += firstConst;
                index_val = index_val % size;
                i_val++;
            }
            if (!valDeclared)
                throw Undeclared(line);
            if (symbolTable[index_val]->type == "")
                throw TypeCannotBeInfered(line);
            else if (symbolTable[index_val]->type != "" && symbolTable[index]->type != ""){
                if (symbolTable[index_val]->type != symbolTable[index]->type)
                    throw TypeMismatch(line);
            }
            else{
                symbolTable[index]->type = symbolTable[index_val]->type;
            }
        }
        else if (valToAssign.find('(') && valToAssign.find(')')){
            // ASSIGN <ID> <Function>
            // A function's type has not been defined, so that varID had to have type.

            size_t pos = valToAssign.find('(');
            string valName = valToAssign.substr(0, pos);

            // First, check valID and varID are declared?
            int index_val = hashFunction(idToKey(valName, scope), size);
            int index_var = hashFunction(idToKey(varToAssign, scope), size);
            bool valDeclared = false;
            bool varDeclared = false;
            int i_val = 1;
            while (symbolTable[index_val] != NULL)
            {
                if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valName)
                {
                    valDeclared = true;
                    break;
                }
                index_val += firstConst * i_val + secondConst * i_val * i_val;
                numsteps += firstConst;
                index_val = index_val % size;
                i_val++;
            }

            int i_var = 1;
            while (symbolTable[index_var] != NULL)
            {
                if (symbolTable[index_var] != NULL && symbolTable[index_var]->identifier == varToAssign)
                {
                    varDeclared = true;
                    break;
                }
                index_var += firstConst * i_var + secondConst * i_var * i_var;
                numsteps += firstConst;
                index_var = index_var % size;
                i_var++;
            }

            if (!valDeclared)
                throw Undeclared(line);

            // If varID's type has not been defined
            // if (varDeclared && symbolTable[index_var]->type == "")
            //     throw TypeCannotBeInfered(line);
            
            string listParam = valToAssign.substr(pos + 1, valToAssign.length() - pos - 2);  // sum(5,x) -> 5,x
            size_t p = string::npos;
            int countParam = symbolTable[index_val]->numOfParameters;
            // cout << countParam << endl;
            while(p = listParam.find(',') != string::npos){
                string param = listParam.substr(0,p);
                if (param != "")
                    countParam--;
                else throw TypeMismatch(line);
                if (validID(param)){
                    int index_param = hashFunction(idToKey(param, scope), size);
                    bool paramDeclared = false;
                    int i_param = 1;
                    while (symbolTable[index_param] != NULL)
                    {
                        if (symbolTable[index_param] != NULL && symbolTable[index_param]->identifier == param)
                        {
                            paramDeclared = true;
                            break;
                        }
                        index_param += firstConst * i_param + secondConst * i_param * i_param;
                        numsteps += firstConst;
                        index_param = index_param % size;
                        i_param++;
                    }
                    if (!paramDeclared) throw Undeclared(line);
                    if (symbolTable[index_param]->numOfParameters != -1)
                        throw TypeMismatch(line);
                    if (symbolTable[index_param]->type == "")
                        throw TypeCannotBeInfered(line);
                }
                listParam.erase(0, p + 1);
            }
            if (countParam != 1) throw TypeMismatch(line);
            if (varDeclared && symbolTable[index_var]->type == "")
                throw TypeCannotBeInfered(line);

            symbolTable[index_val]->type = symbolTable[index_var]->type;
        }
        else throw InvalidInstruction(line);
        cout << numsteps << endl;
    }
    else if (type == "DOUBLE"){
        // int index = hashFunction(idToKey(varToAssign, scope), size);
        
        // if (this->count == size - 1){
        //     throw Overflow(line);
        // }
        // else{
        //     while(symbolTable[index] != NULL){
        //         if (symbolTable[index] != NULL)
        //             if(symbolTable[index]->key == symbol->key && symbolTable[index]->scope == symbol->scope)
        //                 throw Redeclared(line);
        //         index += firstConst * subHashFunction(symbol->key, size);
        //         numsteps += firstConst;
        //         index = index % size;
        //     }
        //     symbolTable[index] = symbol;
        //     this->count++;
        // }    
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

    // Check first line
    int firstConst = -1, secondConst = -1;
    if (hashType[0] != "LINEAR" && hashType[1] != "QUADRIC" && hashType[2] != "DOUBLE")
        throw InvalidInstruction(firstLine);
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
    if (hashType[1] != "") {
        if (!isNum(hashType[1]))
            throw InvalidInstruction(firstLine);
    }
    const int hashSize = stoi(hashType[1]);
    initArray(hashSize);
    
    int scope = 0;
    int beginCount = 0, endCount = 0;

    while (!file.eof()){
        string line = "";
        getline(file, line);
        string code = line.substr(0, line.find(' '));
        if (!checkAction(code)) throw InvalidInstruction(line);

        if (code == "INSERT"){
            this->insert(line, hashType[0], firstConst, secondConst, hashSize, scope);
        }
        else if (code == "ASSIGN"){
            this->assign(line, hashType[0], firstConst, secondConst, hashSize, scope);
        }
        else if (code == "BEGIN"){
            beginCount++;
            scope++;
        }
        else if (code == "END"){
            beginCount--;
            endCount++;
            if (beginCount == 0) endCount = 0;
            if (beginCount < 0) throw UnknownBlock(); // End when there is no Begin
            scope--;
        }
        else if (code == "LOOKUP"){}
        else if (code == "CALL"){}
        else if (code == "PRINT"){}
        else {
            throw InvalidInstruction(line);
        }
    }
    file.close();

    if (beginCount == 0)
    {
        return;
    }
    else if (beginCount > 0)
        throw UnclosedBlock(beginCount);
    if (beginCount < endCount)
    {
        throw UnknownBlock();
    }
    else if (beginCount >= endCount)
    {
        throw UnclosedBlock(beginCount);
    }
    else
    {
        return;
    }
    return;
}