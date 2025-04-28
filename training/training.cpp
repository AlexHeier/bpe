#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <thread>
#include <set>
#include <tuple>

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

void AverageThreadResults(
    const vector<map<int, vector<float>>> &threadVectorMaps,
    const vector<map<int, int>> &threadWordCountMaps,
    int vectorSize
)
{
    set<int> all_ids;

    // Collect all unique ids
    for (const auto &wordCountMap : threadWordCountMaps) {
        for (const auto &[id, count] : wordCountMap) {
            all_ids.insert(id);
        }
    }

    // For each id, compute weighted average
    for (int id : all_ids)
    {
        vector<float> avgVector(vectorSize, 0.0f);
        int totalCount = 0;

        for (size_t t = 0; t < threadVectorMaps.size(); ++t)
        {
            const auto &localMap = threadVectorMaps[t];
            const auto &wordCountMap = threadWordCountMaps[t];

            auto vecIt = localMap.find(id);
            auto countIt = wordCountMap.find(id);

            if (vecIt != localMap.end() && countIt != wordCountMap.end())
            {
                int count = countIt->second;
                totalCount += count;

                for (int i = 0; i < vectorSize; ++i)
                {
                    avgVector[i] += vecIt->second[i] * count;
                }
            }
        }

        if (totalCount > 0)
        {
            for (int i = 0; i < vectorSize; ++i)
            {
                avgVector[i] /= totalCount;
            }
        }

        // Now update the global vectorMap directly
        vectorMap[id] = avgVector;
    }
}


tuple<vector<float>, vector<float>, float> updateVectors(vector<float> tar, vector<float> con, int label)
{
    if (tar.size() != vectorSize || con.size() != vectorSize)
    {
        return {tar, con, 0.0f};
    }

    float dotProduct = 0.0f;
    for (int i = 0; i < vectorSize; ++i)
    {
        dotProduct += tar[i] * con[i];
    }

    float prob = sigmod(dotProduct);
    prob = min(max(prob, 1e-7f), 1.0f - 1e-7f); // to avoid NaNs

    float loss = -(label * log(prob) + (1 - label) * log(1.0f - prob));

    float grad = (prob - label);

    vector<float> original_tar = tar;
    vector<float> original_con = con;

    for (int i = 0; i < vectorSize; ++i)
    {
        tar[i] -= learningRate * grad * original_con[i];
        con[i] -= learningRate * grad * original_tar[i];
    }

    return {tar, con, loss};
}

map<int, vector<float>> Training(string folderPath)
{
    cout << "Training started..." << endl;

    vectorMap = GenerateVectors();
    cout << "Base vector map generated." << endl;

    cout << "The folderpath: " << folderPath << endl;
    vector<int> ids = TextToIDs(folderPath);
    cout << "Text converted to IDs." << endl;

    int threadCount = threads;
    int totalSize = ids.size();
    int chunkSize = (totalSize + threadCount - 1) / threadCount;

    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        vector<map<int, vector<float>>> threadVectorMaps(threadCount);
        vector<map<int, int>> threadWordCountMaps(threadCount);
        vector<thread> threadPool;
        vector<float> threadLosses(threadCount, 0.0f);  // Store losses for each thread

        for (int t = 0; t < threadCount; ++t)
        {
            threadPool.emplace_back([&, t]() {
                map<int, vector<float>> localVectorMap = vectorMap;
                map<int, int> localWordCountMap;

                int startIdx = t * chunkSize;
                int endIdx = min(startIdx + chunkSize, totalSize);

                float tloss = 0.0f;

                for (int j = startIdx; j < endIdx; ++j)
                {
                    int windowStart = max(j - windowSize, 0);
                    int windowEnd = min(j + windowSize, totalSize);

                    for (int k = windowStart; k < windowEnd; ++k)
                    {
                        if (k != j)
                        {
                            auto [tar, con, l] = updateVectors(localVectorMap[ids[j]], localVectorMap[ids[k]], 1);
                            localVectorMap[ids[j]] = tar;
                            localVectorMap[ids[k]] = con;
                            tloss += l;
                            localWordCountMap[ids[j]]++;
                            localWordCountMap[ids[k]]++;

                            for (int n = 0; n < negativeSamples; ++n)
                            {
                                int randomIndex = rand() % totalSize;
                                int randomWord = ids[randomIndex];
                                auto [tar, con, l] = updateVectors(localVectorMap[ids[j]], localVectorMap[randomWord], 0);
                                localVectorMap[ids[j]] = tar;
                                localVectorMap[randomWord] = con;
                                localWordCountMap[ids[j]]++;
                                localWordCountMap[randomWord]++;
                            }
                        }
                    }
                }

                threadVectorMaps[t] = localVectorMap;
                threadWordCountMaps[t] = localWordCountMap;
                threadLosses[t] = tloss;
            });
        }

        // Wait for all threads to finish
        for (auto &th : threadPool)
        {
            th.join();
        }

        // Average the vectors after threads complete
        AverageThreadResults(threadVectorMaps, threadWordCountMaps, vectorSize);

        // Calculate total loss for this epoch
        float totalLoss = 0.0f;
        for (const float &loss : threadLosses)
        {
            totalLoss += loss;
        }

        cout << "Epoch: " << epoch + 1 << " Total Loss: " << totalLoss << endl;
    }

    return vectorMap;
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
