#ifndef TRAINING_H
#define TRAINING_H

#include <map>
#include <vector>
#include <string>

using namespace std;

map<int, vector<float>> Training(string);

void SaveVectors(const string&);

#endif