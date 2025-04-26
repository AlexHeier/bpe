#ifndef saveDecodeCommon_H
#define saveDecodeCommon_H

#include <vector>
#include <map>
#include <utility>
#include <string>
using namespace std;

pair<int, int> FindMostCommon(const vector<int>& input);

void SaveMergeRules(const map<int, pair<int, int>>& mergeRules, const string& filename);

#endif