#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <sstream>
#include <filesystem>
#include <string>
#include <fstream>

#include "..\global.h"
#include "..\vocab\vocab.h"

using namespace std;

namespace fs = std::filesystem;

vector<float> createVector() {
    vector<float> vec;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution dis(0.0, 1.0);

    for (int i = 0; i < vectorSize; ++i) {
        vec.push_back(dis(gen));
    }
    return vec;
}

map<int, vector<float>> GenerateVectors(map<int, pair<int, int>> mergeRules){
    map<int, vector<float>> vectorMap;

    for (int i = 0; i < 256; ++i) {
        vectorMap[i] = createVector();
    }

    for (int i = 0; i < mergeRules.size(); ++i) {
        vectorMap[i] = createVector();
    }

    return vectorMap;
}

string readFolder(const string& folderPath) {
    stringstream result;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            ifstream file(entry.path());
            if (file) {
                result << file.rdbuf(); // Read the whole file
            }
        }
    }

    return result.str();
}

vector<int> TextToIDs(const string& folderPath, const map<int, pair<int, int>>& mergeRules) {
    string text = readFolder(folderPath);
    vector<int> ids = Encode(text, mergeRules);

    if (ids.empty()) {
        cerr << "Error: No IDs generated from the text." << endl;
        return vector<int>();
    }

    return ids;
}