#include <string>
#include <iostream>
#include <fstream>
using namespace std;

int changeToKey(string identifier, int scope)
{
  string key = "";
  for (int i = 0; i < int(identifier.size()); i++)
  {
    key += to_string(identifier[i] - 48);
  }
  key = to_string(scope) + key;
  return stoi(key);
}

int main(){
  int arr[9] = {12};
  for (int i = 0; i < 9; i++){
    cout << arr[i] << endl;
  }
  return 0;
}

// LINEAR <><> / ....  -> Tao hash table 
// INSERT <><> 