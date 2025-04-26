#include <iostream>
#include <fstream>
#include <vector>

#include "ASCII.h"
#include "saveDecodeCommon.h"
using namespace std;

const int LOOPS = 100000;

map<int, pair<int, int>> RulesFromTraining(string filename){
    
    vector<int> asciiValues = InitASCII();

    vector<int> asciiText = FileToASCII(filename);

    map<int, pair<int, int>> mergeRules;

    for (int i = 0; i < LOOPS; ++i)
    {
        pair<int, int> mostCommonPair = FindMostCommon(asciiText);
        
        if (mostCommonPair.first == 0 && mostCommonPair.second == 0)
        {
            break;
        }

        int newID = asciiValues.size();
        asciiValues.push_back(newID);

        mergeRules[newID] = mostCommonPair;

        vector<int> newText;
        for (int j = 0; j < asciiText.size();)
        {
            if (j < asciiText.size() - 1 && asciiText[j] == mostCommonPair.first && asciiText[j + 1] == mostCommonPair.second)
            {
                newText.push_back(newID);
                j += 2;
            }
            else
            {
                newText.push_back(asciiText[j]);
                j += 1;
            }
        }

        asciiText = newText;
    }

    SaveMergeRules(mergeRules, "merge_rules.txt");

    return mergeRules;
}

void DecodeID(int id, const map<int, pair<int, int>>& mergeRules) {
    if (id < 256) {
        cout << static_cast<char>(id);
    } else {
        auto it = mergeRules.find(id);
        if (it != mergeRules.end()) {
            DecodeID(it->second.first, mergeRules);
            DecodeID(it->second.second, mergeRules);
        } else {
            cerr << "Unknown ID during decoding: " << id << endl;
        }
    }
}

map<int, pair<int, int>> RulesFromFile(string filename){
    map<int, pair<int, int>> mergeRules;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error opening file for reading: " << filename << endl;
        return mergeRules;
    }

    int id, first, second;
    while (file >> id >> first >> second) {
        mergeRules[id] = {first, second};
    }

    file.close();
    return mergeRules;
}
