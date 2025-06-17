#ifndef GEBERATE_H
#define GEBERATE_H

#include <map>
#include <vector>

using namespace std;

map<int, vector<float>> GenerateVectors();

pair<vector<int>, int> TextToIDs(const vector<string> &);

#endif