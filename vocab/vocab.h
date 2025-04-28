#ifndef VOCAB_H
#define VOCAB_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

map<int, pair<int, int>> RulesFromTraining(string);

map<int, pair<int, int>> RulesFromFile(string);

void Decode(int id, const map<int, pair<int, int>>& mergeRules);

vector<int> Encode(string);

#endif