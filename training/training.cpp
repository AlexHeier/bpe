#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <thread>
#include <set>
#include <tuple>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <mutex>
#include <sstream>
#include <unordered_map>

#include "..\global.h"
#include "..\vocab\vocab.h"
#include "..\training\generate.h"

using namespace std;
namespace fs = std::filesystem;

map<int, vector<float>> vectorMap;
mutex vectorMutex;
map<int, int> frequencyMap;
map<int, float> discardProb;
int totalWords = 0;
float samplingThreshold = 1e-5;
bool trainingDone = false;

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

vector<float> getVector(int id, unordered_map<int, vector<float>> &local)
{
    auto it = local.find(id);
    if (it != local.end())
    {
        return it->second;
    }
    else
    {
        return vectorMap[id];
    }
}

void saveToFile()
{
    while (true)
    {
        this_thread::sleep_for(chrono::minutes(5));
        vectorMutex.lock();
        map<int, vector<float>> local = vectorMap;
        vectorMutex.unlock();

        ofstream out(vectorFile, ios::binary);

        if (!out)
        {
            cout << "Failed to open " << vectorFile << " for saving. Continuing...";
            continue;
        }

        size_t mapSize = local.size();
        out.write(reinterpret_cast<const char *>(&mapSize), sizeof(mapSize));
        out.write(reinterpret_cast<const char *>(&vectorSize), sizeof(vectorSize));

        for (const auto &[key, vec] : local)
        {
            out.write(reinterpret_cast<const char *>(&key), sizeof(key));
            out.write(reinterpret_cast<const char *>(vec.data()), vectorSize * sizeof(float));
        }
    }
}

vector<string> GetFileNamesFromDirectory(const string &folderPath)
{
    vector<string> fileNames;
    for (const auto &entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            fileNames.push_back(entry.path().string());
        }
    }
    return fileNames;
}

void AverageThreadResults(
    const vector<unordered_map<int, vector<float>>> &threadVectorMaps,
    const vector<unordered_map<int, int>> &threadWordCountMaps,
    int vectorSize)
{
    set<int> all_ids;

    // Collect all unique ids
    for (const auto &wordCountMap : threadWordCountMaps)
    {
        for (const auto &[id, count] : wordCountMap)
        {
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

        vectorMutex.lock();
        vectorMap[id] = avgVector;
        vectorMutex.unlock();
    }
}

tuple<vector<float>, vector<float>, float> updateVectors(vector<float> tar, vector<float> con, int label, float lr)
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
        tar[i] -= lr * grad * original_con[i];
        con[i] -= lr * grad * original_tar[i];
    }

    return {tar, con, loss};
}

static vector<string> get_file_batch(const vector<string> &filenames, int start, int end)
{
    return vector<string>(filenames.begin() + start, filenames.begin() + end);
}

static void process_window(const vector<int> &ids, int j, int totalSize, int windowSize, int negativeSamples,
                           unordered_map<int, vector<float>> &localVectorMap,
                           unordered_map<int, int> &localWordCountMap,
                           float &tloss,
                           const map<int, vector<float>> &vectorMap,
                           float lr)
{
    int windowStart = max(j - windowSize, 0);
    int windowEnd = min(j + windowSize, totalSize);

    for (int k = windowStart; k < windowEnd; ++k)
    {
        if (k != j)
        {
            auto [targetVector, contextVector, loss] = updateVectors(getVector(ids[j], localVectorMap),
                                                                     getVector(ids[k], localVectorMap), 1, lr);
            localVectorMap[ids[j]] = targetVector;
            localVectorMap[ids[k]] = contextVector;
            tloss += loss;
            localWordCountMap[ids[j]]++;
            localWordCountMap[ids[k]]++;

            for (int n = 0; n < negativeSamples; ++n)
            {
                int randomIndex = rand() % totalSize;
                int randomWord = ids[randomIndex];
                auto [negTarget, negContext, negLoss] = updateVectors(getVector(ids[j], localVectorMap),
                                                                      getVector(randomWord, localVectorMap), 0, lr);
                localVectorMap[ids[j]] = negTarget;
                localVectorMap[randomWord] = negContext;
                localWordCountMap[ids[j]]++;
                localWordCountMap[randomWord]++;
            }
        }
    }
}

map<int, vector<float>> Training(const string &folderPath)
{
    cout << "Training started..." << endl;
    if (filesystem::exists(vectorFile))
    {
        cout << "Loading vector map from file" << endl;
        vectorMap = LoadVectorsFromBinary();
        cout << "Vector map loaded" << endl;
    }
    else
    {
        cout << vectorFile + " doesnt exsist. Generating vector map" << endl;
        vectorMap = GenerateVectors();
        cout << "Base vector map generated" << endl;
    }

    cout << "The folderpath: " << folderPath << endl;
    vector<string> fileNames = GetFileNamesFromDirectory(folderPath);
    int totalFiles = fileNames.size();
    cout << "Number of files found: " << totalFiles << endl;

    cout << "Starting saveing to file..." << endl;
    thread dbThread(saveToFile);

    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        cout << endl
             << "Epoch " << epoch + 1 << " started..." << endl;

        for (int batchStart = 0; batchStart < totalFiles; batchStart += documentCount)
        {
            float currentLR = learningRate * (1.0f - float(batchStart) / float(totalFiles));
            currentLR = max(currentLR, 0.001f * learningRate);

            int batchEnd = min(batchStart + documentCount, totalFiles);
            vector<string> batchFiles = get_file_batch(fileNames, batchStart, batchEnd);
            vector<int> ids = TextToIDs(batchFiles);

            cout << endl
                 << "Batch processing: Files " << batchStart + 1 << " to " << batchEnd << endl;

            for (const auto &id : ids)
            {
                frequencyMap[id]++;
                totalWords++;
            }

            for (const auto &pair : frequencyMap)
            {
                int id = pair.first;
                int freq = pair.second;
                float f = freq / (float)totalWords;
                float prob = 1.0f - sqrt(samplingThreshold / f);
                prob = max(0.0f, prob);
                discardProb[id] = prob;
            }

            int totalSize = ids.size();
            int chunkSize = (totalSize + threads - 1) / threads;

            vector<unordered_map<int, vector<float>>> threadVectorMaps(threads);
            vector<unordered_map<int, int>> threadWordCountMaps(threads);
            vector<thread> threadPool;
            vector<float> threadLosses(threads, 0.0f);

            for (int t = 0; t < threads; ++t)
            {
                threadPool.emplace_back([&, t]()
                                        {
                    unordered_map<int, vector<float>> localVectorMap;
                    unordered_map<int, int> localWordCountMap;
                    float tloss = 0.0f;

                    int startIdx = t * chunkSize;
                    int endIdx = min(startIdx + chunkSize, totalSize);

                    for (int j = startIdx; j < endIdx; ++j) {
                        if (discardProb.find(ids[j]) != discardProb.end() &&
                            static_cast<float>(rand()) / RAND_MAX < discardProb[ids[j]])
                        {
                            continue;
                        }
                        process_window(ids, j, totalSize, windowSize, negativeSamples,
                                        localVectorMap, localWordCountMap, tloss,
                                        vectorMap, currentLR);
                    }
                    
                
                    threadVectorMaps[t] = move(localVectorMap);
                    threadWordCountMaps[t] = move(localWordCountMap);
                    threadLosses[t] = tloss; });
            }

            for (auto &th : threadPool)
            {
                th.join();
            }

            AverageThreadResults(threadVectorMaps, threadWordCountMaps, vectorSize);

            float totalLoss = 0.0f;
            for (const float &loss : threadLosses)
            {
                totalLoss += loss;
            }

            cout << "Batch Loss: " << totalLoss << endl;
        }
    }

    trainingDone = true;
    dbThread.join();

    return vectorMap;
}