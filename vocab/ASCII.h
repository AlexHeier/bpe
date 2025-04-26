#ifndef ASCII_H
#define ASCII_H

#include <vector>
using namespace std;

string loadFile(const string& filename);

vector<int> toASCII(const string& str);

vector<int> FileToASCII(const string& filename);

vector<int> InitASCII();

#endif