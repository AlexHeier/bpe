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

int min(int a, int b)
{
    return (a < b) ? a : b;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

float sigmod(float x)
{
    return 1.0f / (1.0f + exp(-x));
}

float updateVectors(int target, int context, int label)
{
    vector<float> tar = vectorMap[target];
    vector<float> con = vectorMap[context];

    if (tar.size() != vectorSize || con.size() != vectorSize)
    {
        return 0.0f;
    }

    float loss, dotProduct = 0.0;
    for (int i = 0; i < vectorSize; ++i)
    {
        dotProduct += tar[i] * con[i];
    }

    float prob = sigmod(dotProduct);
    prob = min(max(prob, 1e-7f), 1.0f - 1e-7f);

    float bce = 1.0 - prob;
    
    loss = -(label * log(prob) + (1 - label) * log(1.0f - prob));

    vector<float> original_tar = tar;
    vector<float> original_con = con;

    for (int i = 0; i < vectorSize; ++i)
    {
        tar[i] += learningRate * bce * original_con[i];
        con[i] += learningRate * bce * original_tar[i];
    }

    vectorMap[target] = tar;
    vectorMap[context] = con;

    return loss;
}

float Training(map<int, pair<int, int>> mergeRules, string folderPath)
{

    vectorMap = GenerateVectors(mergeRules);

    vector<int> ids = TextToIDs(folderPath, mergeRules);

    for (int i = 0; i < epochs; ++i)
    {
        float tempLoss = 0.0;

        for (int j = 0; j < ids.size(); ++j)
        {
            int start = max(j - windowSize, 0);
            int end = min(j + windowSize, ids.size());

            for (int k = start; k < end; ++k)
            {
                if (k != j)
                {
                    tempLoss += updateVectors(ids[j], ids[k], 1);

                    for (int n = 0; n < negativeSamples; ++n) {
                        int randomIndex = rand() % ids.size();
                        int randomWord = ids[randomIndex];
                        tempLoss += updateVectors(ids[j], randomWord, 0);
                    }
                }
            }
        }
        cout << "Epoch " << i + 1 << " completed. Loss: " << tempLoss << endl;
    }

    return 0.0f;
}

void SaveVectors(const string &filename)
{
    ofstream outFile(filename, ios::binary);
    if (!outFile)
    {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    for (const auto &entry : vectorMap)
    {
        int id = entry.first;
        const vector<float> &vec = entry.second;

        outFile.write(reinterpret_cast<const char *>(&id), sizeof(id));
        outFile.write(reinterpret_cast<const char *>(vec.data()), vec.size() * sizeof(float));
    }

    outFile.close();
    cout << "Vectors saved to " << filename << endl;
}
