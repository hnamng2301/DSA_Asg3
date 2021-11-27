#include <string>
#include <iostream>
#include <fstream>
using namespace std;

unsigned long changeToKey(string identifier, int scope)
{
  string key = "";
  for (int i = 0; i < int(identifier.size()); i++)
  {
    cout << identifier[i] - 48 << endl;
    key += to_string(identifier[i] - 48);
  }
  key = to_string(scope) + key;
  
  return stoll(key);
}

int main(){
  cout << changeToKey("ppphj", 0) << endl;
}

// LINEAR <><> / ....  -> Tao hash table 
// INSERT <><> 