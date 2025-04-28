#ifndef GEBERATE_H
#define GEBERATE_H

#include <map>
#include <vector>

using namespace std;

map<int, vector<float>> GenerateVectors();

vector<int> TextToIDs(const string&);

#endif