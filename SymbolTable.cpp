#include "SymbolTable.h"

bool validID(string lineName)
{
    // Kiem tra ID hop le
    if (lineName[0] >= 'a' && lineName[0] <= 'z')   // Phai bat dau bang ky tu thuong
    { 
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
    if (action != "INSERT" && action != "ASSIGN" && action != "PRINT" && 
        action != "BEGIN" && action != "END" && action != "CALL" && action != "LOOKUP")
        return false;
    return true;
}
unsigned long long idToKey(string identifier, int scope)
{
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
        if (i >= 3 && (arg[0] == "LINEAR" || arg[0] == "DOUBLE"))
            throw InvalidInstruction(line);
        else if (i >= 4 && arg[0] == "QUADRATIC")
            throw InvalidInstruction(line);
        arg[i] = *iter++;
        i++;
    }

    if (arg[0] == "LINEAR")
    {
        if (!isNum(arg[1]) || !isNum(arg[2]))
            throw InvalidInstruction(line);
    }
    else if (arg[0] == "DOUBLE")
    {
        if (!isNum(arg[1]) || !isNum(arg[2]))
            throw InvalidInstruction(line);
    }
    else if (arg[0] == "QUADRATIC")
    {
        if (!isNum(arg[1]) || !isNum(arg[2]) || !isNum(arg[3]))
            throw InvalidInstruction(line);
    }
}
void splitLine(string line, string arg[], int numOfWords)
{
    regex reg(" +(?=(?:[^\']*\'[^\']*\')*[^\']*$)");
    regex_token_iterator<string::iterator> iter(line.begin(), line.end(), reg, -1);
    regex_token_iterator<string::iterator> end;
    int i = 0;
    while (iter != end)
    {
        if (i >= numOfWords)
            throw InvalidInstruction(line);
        arg[i] = *iter++;
        i++;
    }
}

void SymbolTable::initArray(int size)
{
    this->symbolTable = new Symbol *[size];
    for (int i = 0; i < size; i++)
    {
        symbolTable[i] = nullptr;
    }
}

unsigned long long SymbolTable::hashFunction(unsigned long long key, int size)
{
    return key % size;
}

unsigned long long SymbolTable::subHashFunction(unsigned long long key, int size)
{
    return 1 + (key % (size - 2));
}

void SymbolTable::insert(string line, string type, int firstConst, int secondConst, int size, int &scope)
{
    string word[3] = {"", "", ""};
    splitLine(line, word, 3);

    if ((line != word[0] + ' ' + word[1] + ' ' + word[2] && word[2] != "") || 
        (line != word[0] + ' ' + word[1] && word[2] == ""))
        throw InvalidInstruction(line);

    int numsteps = 0;
    string name = word[1];
    string value = word[2];
    if (!validID(name))
        throw InvalidInstruction(line);
    // If insert a function type variable in scope != 0
    if (scope != 0 && value != "")
    {
        throw InvalidDeclaration(name);
    }

    int num_of_parameters = -1;
    if (value != "")
    {
        if (isNum(value))
            num_of_parameters = stoi(value);
        else
            throw InvalidInstruction(line);
    }

    if (size == 0) throw Overflow(line);
    
    Symbol *symbol = new Symbol(name, num_of_parameters, scope);
    if (type == "LINEAR")
    {
        int index = hashFunction(symbol->key, size);
        // cout << "Symbol " << symbol->numOfParameters << '\n';
        // cout << "Index: "<< index << " " << endl;
        if (this->count == size - 1)
        {
            throw Overflow(line);
        }
        else
        {
            while (symbolTable[index] != NULL)
            {
                if (symbolTable[index] != NULL)
                    if (symbolTable[index]->identifier == symbol->identifier && symbolTable[index]->scope == symbol->scope)
                        throw Redeclared(name);
                index += firstConst;
                numsteps++;
                index = index % size;
            }

            symbolTable[index] = symbol;
            // cout << numsteps << endl;
            // this->count++;
        }
    }
    else if (type == "QUADRATIC")
    {
        int index = hashFunction(symbol->key, size);
        // cout << "Index: " << index << " " << endl;
        if (this->count == size - 1)
        {
            throw Overflow(line);
        }
        else
        {
            int i = 1;
            while (symbolTable[index] != NULL)
            {
                if (symbolTable[index] != NULL)
                    if (symbolTable[index]->identifier == symbol->identifier && symbolTable[index]->scope == symbol->scope)
                        throw Redeclared(name);
                index += firstConst * i + secondConst * i * i;
                numsteps++;
                ;
                index = index % size;
                i++;
            }
            symbolTable[index] = symbol;
            // this->count++;
        }
    }
    else if (type == "DOUBLE")
    {
        int index = hashFunction(symbol->key, size);
        // cout << symbol->key << " "  << index << endl;
        if (this->count == size - 1)
        {
            throw Overflow(line);
        }
        else
        {
            while (symbolTable[index] != NULL)
            {
                if (symbolTable[index] != NULL)
                    if (symbolTable[index]->identifier == symbol->identifier && symbolTable[index]->scope == symbol->scope)
                        throw Redeclared(name);
                index += firstConst * subHashFunction(symbol->key, size);
                numsteps++;
                // cout << firstConst << endl;
                index = index % size;
            }
            symbolTable[index] = symbol;
            // this->count++;
            // cout << symbol->key << endl;
        }
    }
    cout << numsteps << endl;
    this->count++;
}

void SymbolTable::assign(string line, string type, int firstConst, int secondConst, int size, int currentScope)
{
    string word[3] = {"", "", ""};
    splitLine(line, word, 3);

    if (word[0] + ' ' + word[1] + ' ' + word[2] != line)
        throw InvalidInstruction(line);
    string varToAssign = word[1];
    string valToAssign = word[2];
    if (!validID(varToAssign))
        throw InvalidInstruction(line);
    if (valToAssign == "void" || valToAssign == "number" || valToAssign == "string")
        throw InvalidInstruction(line);
  
    int numsteps = 0;
    if (type == "LINEAR")
    {
        bool isDeclared;
        int index = 0;
        for (int scope = currentScope; scope >= 0; scope--)
        {
            index = hashFunction(idToKey(varToAssign, scope), size);
            isDeclared = false;
            // cout << idToKey(varToAssign, scope) << endl;
            while (symbolTable[index] != NULL)
            {
                // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == varToAssign)
                {
                    isDeclared = true;
                    break;
                }
                index += firstConst;
                numsteps++;
                index = index % size;
            }
            if (isDeclared)
                break;
        }

        if (isDeclared && symbolTable[index]->type == "" && isNum(valToAssign))
        {
            // ASSIGN <ID> <number value>
            symbolTable[index]->type = "number";
        }
        else if (isDeclared && symbolTable[index]->type == "" && isString(valToAssign))
        {
            // ASSIGN <ID> <string value>
            symbolTable[index]->type = "string";
        }
        else if (validID(valToAssign))
        {
            // ASSIGN <ID> <ID>
            bool valDeclared = false;
            int index_val = 0;
            for (int scope_val = currentScope; scope_val >= 0; scope_val--)
            {
                valDeclared = false;
                index_val = hashFunction(idToKey(valToAssign, scope_val), size);
                while (symbolTable[index_val] != NULL)
                {
                    if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valToAssign)
                    {
                        valDeclared = true;
                        break;
                    }
                    index_val += firstConst;
                    numsteps++;
                    index_val = index_val % size;
                }
                if (valDeclared)
                    break;
            }
            // cout << valDeclared << endl;
            if (isDeclared && valDeclared)
            {
                if (symbolTable[index_val]->numOfParameters != -1)
                    throw InvalidInstruction(line);
                if (symbolTable[index_val]->type == "" && symbolTable[index]->type != "")
                    throw TypeMismatch(line); //
                else if (symbolTable[index_val]->type != "" && symbolTable[index]->type != "")
                {
                    if (symbolTable[index_val]->type != symbolTable[index]->type)
                        throw TypeMismatch(line);
                }
                else if (symbolTable[index]->numOfParameters != -1)
                    throw TypeMismatch(line);
                else if (symbolTable[index_val]->type == "" && symbolTable[index]->type == "")
                    throw TypeCannotBeInferred(line);
                else
                {
                    symbolTable[index]->type = symbolTable[index_val]->type;
                }
            }

            if (!valDeclared)
                throw Undeclared(valToAssign);
        }
        else if (valToAssign.find('(') && valToAssign.find(')'))
        {
            // ASSIGN <ID> <Function>
            // A function's type has not been defined, so that varID had to have type.

            size_t pos = valToAssign.find('(');
            string valName = valToAssign.substr(0, pos);
            // cout << valName << endl;
            int index_val = 0;

            // First, check valID and varID are declared?
            bool valDeclared = false;
            for (int val_scope = currentScope; val_scope >= 0; val_scope--)
            {
                valDeclared = false;
                index_val = hashFunction(idToKey(valName, val_scope), size);
                while (symbolTable[index_val] != NULL)
                {
                    if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valName)
                    {
                        valDeclared = true;
                        break;
                    }
                    index_val += firstConst;
                    numsteps++;
                    index_val = index_val % size;
                }
                if (valDeclared)
                    break;
            }

            // cout << "Here\n";
            if (!valDeclared)
                throw Undeclared(valName);

            string ListParam = valToAssign.substr(pos + 1, valToAssign.length() - pos - 2); // sum(5,x) -> 5,x
            size_t p = string::npos;
            int countParam = symbolTable[index_val]->numOfParameters;
            // cout << countParam << endl;
            while ((p = ListParam.find(',')) != string::npos)
            {
                string param = ListParam.substr(0, p);
                if (param != "")
                    countParam--;
                else
                    throw TypeMismatch(line);
                if (validID(param))
                {
                    int index_param = 0;
                    bool paramDeclared = false;
                    for (int param_scope = currentScope; param_scope >= 0; param_scope--)
                    {
                        paramDeclared = false;
                        index_param = hashFunction(idToKey(param, param_scope), size);
                        while (symbolTable[index_param] != NULL)
                        {
                            if (symbolTable[index_param] != NULL && symbolTable[index_param]->identifier == param)
                            {
                                paramDeclared = true;
                                break;
                            }
                            index_param += firstConst;
                            numsteps++;
                            index_param = index_param % size;
                        }
                        if (paramDeclared)
                            break;
                    }
                    if (!paramDeclared)
                        throw Undeclared(param);
                    if (symbolTable[index_param]->numOfParameters != -1)
                        throw TypeMismatch(line);
                    if (symbolTable[index_param]->type == "")
                        throw TypeCannotBeInferred(line);
                }
                ListParam.erase(0, p + 1);
            }
            if (countParam != 1)
                throw TypeMismatch(line);
            if (!isNum(ListParam) && !isString(ListParam) && !validID(ListParam))
                throw InvalidInstruction(line);
            if (validID(ListParam))
            {
                bool ListParamDeclared = false;
                int index_ListParam = 0;
                for (int ListParam_scope = currentScope; ListParam_scope >= 0; ListParam_scope--)
                {
                    index_ListParam = hashFunction(idToKey(ListParam, ListParam_scope), size);
                    ListParamDeclared = false;
                    while (symbolTable[index_ListParam] != NULL)
                    {
                        if (symbolTable[index_ListParam] != NULL && symbolTable[index_ListParam]->identifier == ListParam)
                        {
                            ListParamDeclared = true;
                            break;
                        }
                        index_ListParam += firstConst;
                        numsteps++;
                        index_ListParam = index_ListParam % size;
                    }
                    if (ListParamDeclared)
                        break;
                }

                if (!ListParamDeclared)
                    throw Undeclared(ListParam);
                if (symbolTable[index_ListParam]->numOfParameters != -1)
                    throw TypeMismatch(line);
                if (symbolTable[index_ListParam]->type == "")
                    throw TypeCannotBeInferred(line);
            }
            if (countParam != 1)
                throw TypeMismatch(line);

            if (isDeclared)
            {
                if (symbolTable[index]->type == "" && symbolTable[index]->numOfParameters == -1)
                    throw TypeCannotBeInferred(line);
                else if (symbolTable[index]->numOfParameters != -1)
                    throw TypeMismatch(line);
            }
            // cout << "Here is the: " << symbolTable[index]->numOfParameters << endl;
            if (isDeclared && valDeclared)
                symbolTable[index_val]->type = symbolTable[index]->type;
        }

        if (!isDeclared)
            throw Undeclared(varToAssign);

        cout << numsteps << endl;
    }
    else if (type == "QUADRATIC")
    {
        bool isDeclared;
        int index = 0;
        for (int var_scope = currentScope; var_scope >= 0; var_scope--)
        {
            index = hashFunction(idToKey(varToAssign, var_scope), size);
            isDeclared = false;

            int i = 1;
            while (symbolTable[index] != NULL)
            {
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == varToAssign)
                {
                    isDeclared = true;
                    break;
                }
                index += firstConst * i + secondConst * i * i;
                numsteps++;
                index = index % size;
                i++;
            }
            if (isDeclared) break;
        }

        // cout << symbolTable[index]->identifier << endl;
        // cout << varToAssign << ' ' << valToAssign << endl;
        if (isDeclared && symbolTable[index]->type == "" && isNum(valToAssign))
        {
            // ASSIGN <ID> <number value>
            symbolTable[index]->type = "number";
        }
        else if (isDeclared && symbolTable[index]->type == "" && isString(valToAssign))
        {
            // ASSIGN <ID> <string value>
            symbolTable[index]->type = "string";
        }
        else if (validID(valToAssign))
        {
            // ASSIGN <ID> <ID>
            // cout << valToAssign << endl;
            bool valDeclared = false;
            int index_val = 0;
            for (int val_scope = currentScope; val_scope >= 0; val_scope--)
            {
                index_val = hashFunction(idToKey(valToAssign, val_scope), size);
                valDeclared = false;
                int i_val = 1;
                while (symbolTable[index_val] != NULL)
                {
                    if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valToAssign)
                    {
                        valDeclared = true;
                        break;
                    }
                    index_val += firstConst * i_val + secondConst * i_val * i_val;
                    numsteps++;
                    index_val = index_val % size;
                    i_val++;
                }
                if (valDeclared)
                    break;
            }
            // cout << valDeclared << endl;
            if (isDeclared && valDeclared)
            {
                if (symbolTable[index_val]->numOfParameters != -1)
                    throw InvalidInstruction(line);
                
                if (symbolTable[index_val]->type == "" && symbolTable[index]->type != "")
                    throw TypeMismatch(line); //
                else if (symbolTable[index_val]->type != "" && symbolTable[index]->type != "")
                {
                    if (symbolTable[index_val]->type != symbolTable[index]->type)
                        throw TypeMismatch(line);
                }
                else if (symbolTable[index_val]->type == "" && symbolTable[index]->type == "")
                    throw TypeCannotBeInferred(line);
                else if (symbolTable[index]->numOfParameters != -1)
                    throw TypeMismatch(line);
                else
                {
                    symbolTable[index]->type = symbolTable[index_val]->type;
                }
            }

            if (!valDeclared)
                throw Undeclared(valToAssign);
        }
        else if (valToAssign.find('(') && valToAssign.find(')'))
        {
            // ASSIGN <ID> <Function>
            // A function's type has not been defined, so that varID had to have type.

            size_t pos = valToAssign.find('(');
            string valName = valToAssign.substr(0, pos);
            int index_val = 0; // index_var = 0;
            // First, check valID and varID are declared?
            bool valDeclared = 0;
            for (int val_scope = currentScope; val_scope >= 0; val_scope--)
            {
                int i_val = 1;
                valDeclared = false;
                index_val = hashFunction(idToKey(valName, val_scope), size);
                while (symbolTable[index_val] != NULL)
                {
                    if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valName)
                    {
                        valDeclared = true;
                        break;
                    }
                    index_val += firstConst * i_val + secondConst * i_val * i_val;
                    numsteps++;
                    index_val = index_val % size;
                    i_val++;
                }
                if (valDeclared)
                    break;
            }

            if (!valDeclared)
                throw Undeclared(valName);

            string ListParam = valToAssign.substr(pos + 1, valToAssign.length() - pos - 2); // sum(5,x) -> 5,x
            size_t p = string::npos;
            int countParam = symbolTable[index_val]->numOfParameters;
            // cout << countParam << endl;
            while ((p = ListParam.find(',')) != string::npos)
            {
                string param = ListParam.substr(0, p);
                if (param != "")
                    countParam--;
                else
                    throw TypeMismatch(line);
                if (validID(param))
                {
                    // cout << param << endl;
                    bool paramDeclared = false;
                    int index_param = 0;
                    for (int param_scope = currentScope; param_scope >= 0; param_scope--)
                    {
                        index_param = hashFunction(idToKey(param, param_scope), size);
                        paramDeclared = false;
                        int i_param = 1;
                        while (symbolTable[index_param] != NULL)
                        {
                            if (symbolTable[index_param] != NULL && symbolTable[index_param]->identifier == param)
                            {
                                paramDeclared = true;
                                break;
                            }
                            index_param += firstConst * i_param + secondConst * i_param * i_param;
                            numsteps++;
                            index_param = index_param % size;
                            i_param++;
                        }
                        if (paramDeclared)
                            break;
                    }
                    if (!paramDeclared)
                        throw Undeclared(param);
                    if (symbolTable[index_param]->numOfParameters != -1)
                        throw TypeMismatch(line);
                    if (symbolTable[index_param]->type == "")
                        throw TypeCannotBeInferred(line);
                }
                ListParam.erase(0, p + 1);
            }
            if (countParam != 1)
                throw TypeMismatch(line);
            if (!isNum(ListParam) && !isString(ListParam) && !validID(ListParam))
                throw InvalidInstruction(line);
            if (validID(ListParam))
            {
                int index_ListParam = 0;
                bool ListParamDeclared = false;
                for (int ListParam_scope = currentScope; ListParam_scope >= 0; ListParam_scope--)
                {
                    index_ListParam = hashFunction(idToKey(ListParam, ListParam_scope), size);
                    ListParamDeclared = false;
                    int i_List = 1;
                    while (symbolTable[index_ListParam] != NULL)
                    {
                        if (symbolTable[index_ListParam] != NULL && symbolTable[index_ListParam]->identifier == ListParam)
                        {
                            ListParamDeclared = true;
                            break;
                        }
                        index_ListParam += firstConst * i_List + secondConst * i_List * i_List;
                        numsteps++;
                        index_ListParam = index_ListParam % size;
                        i_List++;
                    }
                    if (ListParamDeclared)
                        break;
                }

                if (!ListParamDeclared)
                    throw Undeclared(ListParam);
                if (symbolTable[index_ListParam]->numOfParameters != -1)
                    throw TypeMismatch(line);
                if (symbolTable[index_ListParam]->type == "")
                    throw TypeCannotBeInferred(line);
            }
            if (countParam != 1)
                throw TypeMismatch(line);
            if (isDeclared && symbolTable[index]->type == "" && symbolTable[index]->numOfParameters == -1)
                throw TypeCannotBeInferred(line);
            if (isDeclared && symbolTable[index]->numOfParameters != -1)
                throw TypeMismatch(line);

            if (isDeclared && valDeclared)
                symbolTable[index_val]->type = symbolTable[index]->type;
        }

        if (!isDeclared) throw Undeclared(varToAssign);

        cout << numsteps << endl;
    }
    else if (type == "DOUBLE")
    {
        bool isDeclared = false;
        int index = 0;
        // cout << "Current scope: " << currentScope << endl;
        // cout << "Var: " << varToAssign << endl;
        // cout << "Val: " << valToAssign << endl;
        for (int var_scope = currentScope; var_scope >= 0; var_scope--)
        {
            index = hashFunction(idToKey(varToAssign, var_scope), size);
            isDeclared = false;
            while (symbolTable[index] != NULL)
            {
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == varToAssign)
                {
                    isDeclared = true;
                    break;
                }
                index += firstConst * subHashFunction(idToKey(varToAssign, var_scope), size);
                numsteps++;
                index = index % size;
            }
            if (isDeclared)
                break;
        }
        // cout << boolalpha << isDeclared << endl;
        if (isDeclared && symbolTable[index]->type == "" && isNum(valToAssign))
        {
            // ASSIGN <ID> <number value>
            symbolTable[index]->type = "number";
        }
        else if (isDeclared && symbolTable[index]->type == "" && isString(valToAssign))
        {
            // ASSIGN <ID> <string value>
            symbolTable[index]->type = "string";
        }
        else if (validID(valToAssign))
        {
            // ASSIGN <ID> <ID>
            // cout << "Current scope: " << currentScope << endl;

            bool valDeclared = false;
            int index_val = 0;
            for (int val_scope = currentScope; val_scope >= 0; val_scope--)
            {
                index_val = hashFunction(idToKey(valToAssign, val_scope), size);
                valDeclared = false;
                while (symbolTable[index_val] != NULL)
                {
                    if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valToAssign)
                    {
                        valDeclared = true;
                        break;
                    }
                    index_val += firstConst * subHashFunction(idToKey(valToAssign, val_scope), size);
                    numsteps++;
                    index_val = index_val % size;
                }
                if (valDeclared)
                    break;
            }
            if (isDeclared && valDeclared)
            {
                if (symbolTable[index_val]->numOfParameters != -1)
                    throw InvalidInstruction(line);
                    // cout << "Invalid parameter\n";
                
                else if (symbolTable[index_val]->type == "" && symbolTable[index]->type != "")
                    throw TypeMismatch(line); //
                else if (symbolTable[index_val]->type != "" && symbolTable[index]->type != "")
                {
                    if (symbolTable[index_val]->type != symbolTable[index]->type)
                        throw TypeMismatch(line);
                }
                else if (symbolTable[index]->numOfParameters != -1)
                    throw TypeMismatch(line);
                else if (symbolTable[index_val]->type == "" && symbolTable[index]->type == "")
                    throw TypeCannotBeInferred(line);
                else
                {
                    symbolTable[index]->type = symbolTable[index_val]->type;
                }
            }

            if (!valDeclared)
                throw Undeclared(valToAssign);
            // cout << boolalpha << valDeclared << endl;
        }
        else if (valToAssign.find('(') && valToAssign.find(')'))
        {
            // ASSIGN <ID> <Function>
            // A function's type has not been defined, so that varID had to have type.
            // cout << "Here\n";
            size_t pos = valToAssign.find('(');
            string valName = valToAssign.substr(0, pos);

            // First, check valID and varID are declared?
            int index_val = 0;
            bool valDeclared = false;
            for (int val_scope = currentScope; val_scope >= 0; val_scope--)
            {
                index_val = hashFunction(idToKey(valName, val_scope), size);
                // cout << idToKey(valName, val_scope) << ' ' << index_val << endl;
                valDeclared = false;
                while (symbolTable[index_val] != NULL)
                {
                    if (symbolTable[index_val] != NULL && symbolTable[index_val]->identifier == valName)
                    {
                        valDeclared = true;
                        break;
                    }
                    index_val += firstConst * subHashFunction(idToKey(valName, val_scope), size);
                    numsteps++;
                    index_val = index_val % size;
                }
                if (valDeclared)
                    break;
            }

            // cout << valName << endl;
            if (!valDeclared)
                throw Undeclared(valName);

            string ListParam = valToAssign.substr(pos + 1, valToAssign.length() - pos - 2); // sum(5,x) -> 5,x
            size_t p = string::npos;
            int countParam = symbolTable[index_val]->numOfParameters;
            // cout << countParam << endl;
            while ((p = ListParam.find(',')) != string::npos)
            {
                string param = ListParam.substr(0, p);
                if (param != "")
                    countParam--;
                else
                    throw TypeMismatch(line);
                if (validID(param))
                {
                    int index_param = 0;
                    bool paramDeclared = false;
                    for (int param_scope = currentScope; param_scope >= 0; param_scope--)
                    {
                        paramDeclared = false;
                        index_param = hashFunction(idToKey(param, param_scope), size);
                        while (symbolTable[index_param] != NULL)
                        {
                            if (symbolTable[index_param] != NULL && symbolTable[index_param]->identifier == param)
                            {
                                paramDeclared = true;
                                break;
                            }
                            index_param += firstConst * subHashFunction(idToKey(param, param_scope), size);
                            numsteps++;
                            index_param = index_param % size;
                        }
                        if (paramDeclared)
                            break;
                    }
                    if (!paramDeclared)
                        throw Undeclared(param);
                    if (symbolTable[index_param]->numOfParameters != -1)
                        throw TypeMismatch(line);
                    if (symbolTable[index_param]->type == "")
                        throw TypeCannotBeInferred(line);
                }
                ListParam.erase(0, p + 1);
            }
            if (countParam != 1)
                throw TypeMismatch(line);
            if (!isNum(ListParam) && !isString(ListParam) && !validID(ListParam))
                throw InvalidInstruction(line);
            if (validID(ListParam))
            {
                int index_ListParam = 0;
                bool ListParamDeclared = false;
                for (int ListParam_scope = currentScope; ListParam_scope >= 0; ListParam_scope--)
                {
                    index_ListParam = hashFunction(idToKey(ListParam, ListParam_scope), size);
                    ListParamDeclared = false;
                    while (symbolTable[index_ListParam] != NULL)
                    {
                        if (symbolTable[index_ListParam] != NULL && symbolTable[index_ListParam]->identifier == ListParam)
                        {
                            ListParamDeclared = true;
                            break;
                        }
                        index_ListParam += firstConst * subHashFunction(idToKey(ListParam, ListParam_scope), size);
                        numsteps++;
                        index_ListParam = index_ListParam % size;
                    }
                    if (ListParamDeclared)
                        break;
                }

                if (!ListParamDeclared)
                    throw Undeclared(ListParam);
                if (symbolTable[index_ListParam]->numOfParameters != -1)
                    throw TypeMismatch(line);
                if (symbolTable[index_ListParam]->type == "")
                    throw TypeCannotBeInferred(line);
            }

            if (countParam != 1)
                throw TypeMismatch(line);
            if (isDeclared && symbolTable[index]->type == "" && symbolTable[index]->numOfParameters == -1)
                throw TypeCannotBeInferred(line);
            if (isDeclared && symbolTable[index]->numOfParameters != -1)
                throw TypeMismatch(line);
            if (isDeclared && valDeclared)
                symbolTable[index_val]->type = symbolTable[index]->type;
        }
        // cout << "Here is the\n";

        if (!isDeclared)
            throw Undeclared(varToAssign);
        cout << numsteps << endl;
    }
}

void SymbolTable::call(string line, string type, int firstConst, int secondConst, int currentScope, int size)
{
    string word[2] = {"", ""};
    splitLine(line, word, 2);
    if (line != word[0] + ' ' + word[1])
        throw InvalidInstruction(line);
    string functionCaller = word[1];
    if (functionCaller.find('(') == string::npos || functionCaller.find(')') == string::npos)
        throw InvalidInstruction(line);
    // cout << word[0] << " " << word[1] << endl;

    size_t pos = functionCaller.find('(');
    string functionName = functionCaller.substr(0, pos);
    string ListArg = functionCaller.substr(pos + 1, functionCaller.length() - pos - 2);
    int numsteps = 0;
    if (type == "LINEAR")
    {
        int scope = currentScope;
        bool isDeclared = false;
        while (scope >= 0)
        {
            int index = hashFunction(idToKey(functionName, scope), size);
            // cout << idToKey(varToAssign, scope) << endl;
            while (symbolTable[index] != NULL)
            {
                // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == functionName)
                {
                    isDeclared = true;
                    break;
                }
                index += firstConst;
                numsteps++;
                index = index % size;
            }
            if (isDeclared)
            {
                if (symbolTable[index]->numOfParameters == -1)
                    throw TypeMismatch(line);
                break;
            }
            scope--;
        }
        if (!isDeclared)
            throw Undeclared(functionName);
        
        size_t p = string::npos;
        while ((p = ListArg.find(',')) != string::npos)
        {
            string arg = ListArg.substr(0, p);
            // cout << p << endl;

            if (!isNum(arg) && !isString(arg) && !validID(arg))
                throw InvalidInstruction(line);
            if (validID(arg))
            {
                scope = currentScope;
                isDeclared = false;
                while (scope >= 0)
                {
                    int index = hashFunction(idToKey(arg, scope), size);
                    // cout << idToKey(varToAssign, scope) << endl;
                    while (symbolTable[index] != NULL)
                    {
                        // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                        if (symbolTable[index] != NULL && symbolTable[index]->identifier == arg)
                        {
                            isDeclared = true;
                            break;
                        }
                        index += firstConst;
                        numsteps++;
                        index = index % size;
                    }
                    if (isDeclared)
                    {
                        if (symbolTable[index]->type == "")
                            throw TypeMismatch(line);
                        break;
                    }
                    scope--;
                }
                if (!isDeclared) throw Undeclared(arg);
            }
            ListArg.erase(0, p + 1);
        }
        // 'abc',aa -> aa -> check aa
        if (!isNum(ListArg) && !isString(ListArg) && !validID(ListArg))
            throw InvalidInstruction(line);
        if (validID(ListArg))
        {
            scope = currentScope;
            isDeclared = false;
            while (scope >= 0)
            {
                int index = hashFunction(idToKey(ListArg, scope), size);
                // cout << idToKey(varToAssign, scope) << endl;
                while (symbolTable[index] != NULL)
                {
                    // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                    if (symbolTable[index] != NULL && symbolTable[index]->identifier == ListArg)
                    {
                        isDeclared = true;
                        break;
                    }
                    index += firstConst;
                    numsteps++;
                    index = index % size;
                }
                if (isDeclared)
                {
                    if (symbolTable[index]->type == "")
                        throw TypeMismatch(line);
                    break;
                }
                scope--;
            }
            if (!isDeclared) throw Undeclared(ListArg);
        }
    }
    else if (type == "QUADRATIC")
    {
        int scope = currentScope;
        bool isDeclared = false;
        while (scope >= 0)
        {
            int index = hashFunction(idToKey(functionName, scope), size);
            int i = 1;
            // cout << idToKey(varToAssign, scope) << endl;
            while (symbolTable[index] != NULL)
            {
                // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == functionName)
                {
                    isDeclared = true;
                    break;
                }
                index += firstConst * i + secondConst * i * i;
                numsteps++;
                index = index % size;
                i++;
            }
            if (isDeclared)
            {
                if (symbolTable[index]->numOfParameters == -1)
                    throw TypeMismatch(line);
                break;
            }
            scope--;
        }
        if (!isDeclared)
            throw Undeclared(functionName);

        size_t p = string::npos;
        while ((p = ListArg.find(',')) != string::npos)
        {
            string arg = ListArg.substr(0, p);
            // cout << p << endl;

            if (!isNum(arg) && !isString(arg) && !validID(arg))
                throw InvalidInstruction(line);
            if (validID(arg))
            {
                scope = currentScope;
                isDeclared = false;
                int i = 1;
                while (scope >= 0)
                {
                    int index = hashFunction(idToKey(arg, scope), size);
                    // cout << idToKey(varToAssign, scope) << endl;
                    while (symbolTable[index] != NULL)
                    {
                        // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                        if (symbolTable[index] != NULL && symbolTable[index]->identifier == arg)
                        {
                            isDeclared = true;
                            break;
                        }
                        index += firstConst * i + secondConst * i * i;
                        numsteps++;
                        index = index % size;
                        i++;
                    }
                    if (isDeclared)
                    {
                        if (symbolTable[index]->type == "")
                            throw TypeMismatch(line);
                        break;
                    }
                    scope--;
                }
                if (!isDeclared) throw Undeclared(arg);
            }
            ListArg.erase(0, p + 1);
        }
        // 'abc',aa -> aa -> check aa
        if (!isNum(ListArg) && !isString(ListArg) && !validID(ListArg))
            throw InvalidInstruction(line);
        if (validID(ListArg))
        {
            scope = currentScope;
            isDeclared = false;
            while (scope >= 0)
            {
                int index = hashFunction(idToKey(ListArg, scope), size);
                int i = 1;
                // cout << idToKey(varToAssign, scope) << endl;
                while (symbolTable[index] != NULL)
                {
                    // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                    if (symbolTable[index] != NULL && symbolTable[index]->identifier == ListArg)
                    {
                        isDeclared = true;
                        break;
                    }
                    index += firstConst * i + secondConst * i * i;
                    numsteps++;
                    index = index % size;
                    i++;
                }
                if (isDeclared)
                {
                    if (symbolTable[index]->type == "")
                        throw TypeMismatch(line);
                    break;
                }
                scope--;
            }
            if (!isDeclared) throw Undeclared(ListArg);
        }
    }
    else if (type == "DOUBLE")
    {
        int scope = currentScope;
        bool isDeclared = false;
        while (scope >= 0)
        {
            int index = hashFunction(idToKey(functionName, scope), size);
            // cout << idToKey(varToAssign, scope) << endl;
            while (symbolTable[index] != NULL)
            {
                // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == functionName)
                {
                    isDeclared = true;
                    break;
                }
                index += firstConst * subHashFunction(idToKey(functionName, scope), size);
                numsteps++;
                index = index % size;
            }
            if (isDeclared)
            {
                if (symbolTable[index]->numOfParameters == -1)
                    throw TypeMismatch(line);
                break;
            }
            scope--;
        }
        if (!isDeclared)
            throw Undeclared(functionName);

        size_t p = string::npos;
        while ((p = ListArg.find(',')) != string::npos)
        {
            string arg = ListArg.substr(0, p);
            // cout << p << endl;

            if (!isNum(arg) && !isString(arg) && !validID(arg))
                throw InvalidInstruction(line);
            if (validID(arg))
            {
                scope = currentScope;
                isDeclared = false;
                while (scope >= 0)
                {
                    int index = hashFunction(idToKey(arg, scope), size);
                    // cout << idToKey(varToAssign, scope) << endl;
                    while (symbolTable[index] != NULL)
                    {
                        // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                        if (symbolTable[index] != NULL && symbolTable[index]->identifier == arg)
                        {
                            isDeclared = true;
                            break;
                        }
                        index += firstConst * subHashFunction(idToKey(functionName, scope), size);
                        numsteps++;
                        index = index % size;
                    }
                    if (isDeclared)
                    {
                        // cout << symbolTable[index]->identifier << endl;
                        if (symbolTable[index]->type == "")
                            throw TypeMismatch(line);
                        break;
                    }
                    scope--;
                }
                if (!isDeclared) throw Undeclared(arg);
            }
            ListArg.erase(0, p + 1);
        }
        // 'abc',aa -> aa -> check aa
        if (!isNum(ListArg) && !isString(ListArg) && !validID(ListArg))
            throw InvalidInstruction(line);
        if (validID(ListArg))
        {
            scope = currentScope;
            isDeclared = false;
            while (scope >= 0)
            {
                int index = hashFunction(idToKey(ListArg, scope), size);
                // cout << idToKey(varToAssign, scope) << endl;
                while (symbolTable[index] != NULL)
                {
                    // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                    if (symbolTable[index] != NULL && symbolTable[index]->identifier == ListArg)
                    {
                        isDeclared = true;
                        break;
                    }
                    index += firstConst * subHashFunction(idToKey(functionName, scope), size);
                    numsteps++;
                    index = index % size;
                }
                if (isDeclared)
                {
                    if (symbolTable[index]->type == "")
                        throw TypeMismatch(line);
                    break;
                }
                scope--;
            }
            if (!isDeclared) throw Undeclared(ListArg);
        }
    }
    cout << numsteps << endl;
}

void SymbolTable::exitScope(int &scope, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (symbolTable[i] != nullptr && symbolTable[i]->scope == scope)
        {
            delete symbolTable[i];
            symbolTable[i] = nullptr;
        }
    }
}

void SymbolTable::look_up(string line, string type, int firstConst, int secondConst, int scope, int size)
{
    int numsteps = 0;
    bool isDeclared = false;
    int posOfName = 0;
    string word[2] = {"", ""};
    splitLine(line, word, 2);
    string name = word[1];

    for (int i = scope; i >= 0; i--)
    {
        int index = hashFunction(idToKey(name, i), size);
        // cout << idToKey(name, i) << endl;
        // cout << scope << endl;
        if (type == "LINEAR")
        {
            while (symbolTable[index] != NULL)
            {
                // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == name)
                {
                    isDeclared = true;
                    posOfName = index;
                    break;
                }
                index += firstConst;
                numsteps++;
                index = index % size;
            }
        }
        else if (type == "QUADRATIC")
        {
            int j = 1;
            while (symbolTable[index] != NULL)
            {
                // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == name)
                {
                    isDeclared = true;
                    posOfName = index;
                    break;
                }
                index += firstConst * j + secondConst * j * j;
                numsteps++;
                index = index % size;
                j++;
            }
        }
        else if (type == "DOUBLE")
        {
            while (symbolTable[index] != NULL)
            {
                // cout << symbolTable[index]->identifier << " " << varToAssign << endl;
                if (symbolTable[index] != NULL && symbolTable[index]->identifier == name)
                {
                    isDeclared = true;
                    posOfName = index;
                    break;
                }
                index += firstConst * subHashFunction(idToKey(name, i), size);
                numsteps++;
                index = index % size;
            }
        }

        if (isDeclared)
            break;
    }
    if (!isDeclared)
        throw Undeclared(name);
    cout << posOfName << endl;
}

string SymbolTable::result(int size)
{
    string res = "";
    for (int i = 0; i < size; i++)
    {
        if (symbolTable[i] != nullptr)
        {
            res += to_string(i) + " ";
            res += symbolTable[i]->identifier;
            res += "//";
            res += to_string(symbolTable[i]->scope);
            res += ';';
        }
    }
    return res;
}
void SymbolTable::printTable(string res)
{
    for (int i = 0; i < int(res.size()) - 1; i++)
    {
        cout << res[i];
    }
    if (res != "")
        cout << endl;
}

void SymbolTable::run(string filename)
{
    ifstream file;
    file.open(filename);
    string firstLine;
    getline(file, firstLine);
    string hashType[4] = {"", "", "", ""};
    splitFirstLine(firstLine, hashType);

    // Check first line
    int firstConst = -1, secondConst = -1;
    if (hashType[0] != "LINEAR" && hashType[0] != "QUADRATIC" && hashType[0] != "DOUBLE")
        throw InvalidInstruction(firstLine);
    if (hashType[2] != "")
    {
        if (isNum(hashType[2])){
            firstConst = stoi(hashType[2]);
            if (firstConst < 0) throw InvalidInstruction(firstLine);
        }
        else
            throw InvalidInstruction(firstLine);
    }
    if (hashType[3] != "")
    {
        if (isNum(hashType[3])){
            secondConst = stoi(hashType[3]);
            if (secondConst < 0) throw InvalidInstruction(firstLine);
        }
        else
            throw InvalidInstruction(firstLine);
    }
    if (hashType[1] != "")
    {
        if (!isNum(hashType[1]))
            throw InvalidInstruction(firstLine);
    }

    const int hashSize = stoi(hashType[1]);
    if (hashSize < 0) throw InvalidInstruction(firstLine);
    initArray(hashSize);
    this->size = hashSize;
    int scope = 0;
    int beginCount = 0, endCount = 0;

    while (!file.eof())
    {
        string line = "";
        getline(file, line);
        string code = line.substr(0, line.find(' '));
        if (!checkAction(code))
            throw InvalidInstruction(line);

        if (code == "INSERT")
        {
            this->insert(line, hashType[0], firstConst, secondConst, hashSize, scope);
        }
        else if (code == "ASSIGN")
        {
            this->assign(line, hashType[0], firstConst, secondConst, hashSize, scope);
        }
        else if (code == "BEGIN")
        {
            beginCount++;
            scope++;
        }
        else if (code == "END")
        {
            beginCount--;
            endCount++;
            if (beginCount == 0)
                endCount = 0;
            if (beginCount < 0)
                throw UnknownBlock(); // End when there is no Begin
            this->exitScope(scope, hashSize);
            scope--;
        }
        else if (code == "LOOKUP")
        {
            this->look_up(line, hashType[0], firstConst, secondConst, scope, hashSize);
        }
        else if (code == "CALL")
        {
            this->call(line, hashType[0], firstConst, secondConst, scope, hashSize);
        }
        else if (code == "PRINT")
        {
            this->printTable(this->result(hashSize));
        }
        else
        {
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

    return;
}