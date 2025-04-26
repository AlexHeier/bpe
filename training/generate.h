#ifndef GEBERATE_H
#define GEBERATE_H

#include <map>
#include <vector>

using namespace std;

map<int, vector<float>> GenerateVectors(map<int, pair<int, int>>);

vector<int> TextToIDs(const string&, const map<int, pair<int, int>>&);

#endif