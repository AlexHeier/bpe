#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>
using namespace std;

pair<int, int> FindMostCommon(const vector<int>& input) {
    map<pair<int, int>, int> pairs;
    int maxCount = 0;
    pair<int, int> mostCommonPair = {-1, -1};

    for (int i = 0; i < input.size() - 1; ++i) {
        pairs[{input[i], input[i + 1]}]++;
    }

    for (const auto& p : pairs) {
        if (p.second > maxCount) {
            maxCount = p.second;
            mostCommonPair = p.first;
        }
    }

    if (maxCount == 1) {
        return {-1, -1};
    }

    return mostCommonPair;
}

void SaveMergeRules(const map<int, pair<int, int>>& mergeRules, const string& filename) {
    ofstream outFile(filename);
    
    if (!outFile.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    for (const auto& entry : mergeRules) {
        int id = entry.first;
        const auto& pair = entry.second;
        outFile << id << " " << pair.first << " " << pair.second << "\n";
    }

    outFile.close();
    cout << endl << "Number of merge rules: " << mergeRules.size()+255 << endl << "Merge rules saved to " << filename << endl;
}