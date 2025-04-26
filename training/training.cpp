#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <random>

#include "..\global.h"
#include "..\vocab\vocab.h"
#include "..\training\generate.h"

using namespace std;

map<int, vector<float>> vectorMap;


int min(int a, int b) {
    return (a < b) ? a : b;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

float sigmod(float x) {
    return 1.0f / (1.0f + exp(-x));
}

float updateVectors(int target, int context) {
    vector<float>& vec1 = vectorMap[target];
    vector<float>& vec2 = vectorMap[context];

    if (vec1.size() != vectorSize || vec2.size() != vectorSize) {
        return 0.0f;
    }

    float loss, dotProduct = 0.0;
    for (int i = 0; i < vectorSize; ++i) {
        dotProduct += vec1[i] * vec2[i];
    }

    float prob = sigmod(dotProduct);
    float bce = 1.0 - prob;
    loss = -log(prob);

    for (int i = 0; i < vectorSize; ++i) {
        vec1[i] += learningRate * loss * vec2[i];
        vec2[i] += learningRate * loss * vec1[i];
    }

    vectorMap[target] = vec1;
    vectorMap[context] = vec2;

    return loss;
}

float Training(map<int, pair<int, int>> mergeRules, string folderPath) {
    float loss;

    vectorMap = GenerateVectors(mergeRules);
 
    vector<int> ids = TextToIDs(folderPath, mergeRules);

    for (int i = 0; i < epochs; ++i) {
        float tempLoss;

        for (int j = 0; j < sizeof(ids); ++j) {
            int start = max(j - windowSize, 0);
            int end = min(j + windowSize, sizeof(ids));

            for (int k = start; k < end; ++k) {
                if (k != j) {
                    tempLoss += updateVectors(ids[j], ids[k]);
                }
            }
        }
        cout << "Epoch " << i + 1 << " completed. Loss: " << tempLoss << endl;
        loss += tempLoss;
    }

    return loss;
}

void SaveVectors(const string& filename) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    for (const auto& entry : vectorMap) {
        int id = entry.first;
        const vector<float>& vec = entry.second;

        outFile.write(reinterpret_cast<const char*>(&id), sizeof(id));
        outFile.write(reinterpret_cast<const char*>(vec.data()), vec.size() * sizeof(float));
    }

    outFile.close();
    cout << "Vectors saved to " << filename << endl;
}



