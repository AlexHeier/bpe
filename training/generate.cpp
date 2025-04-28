#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <sstream>
#include <filesystem>
#include <string>
#include <fstream>
#include <thread>
#include <mutex>

#include "..\global.h"
#include "..\vocab\vocab.h"

using namespace std;

namespace fs = std::filesystem;

vector<float> createVector()
{
    random_device rd;
    mt19937 gen(rd());
    vector<float> vec;
    uniform_real_distribution dis(0.0, 1.0);

    vec.resize(vectorSize);

    for (int i = 0; i < vectorSize; ++i)
    {
        vec[i] = dis(gen);
    }
    return vec;
}

map<int, vector<float>> GenerateVectors()
{
    map<int, vector<float>> vectorMap;

    cout << "Generating base vector map..." << endl;
    cout.flush();
    for (int i = 0; i < 256; ++i)
    {
        vectorMap[i] = createVector();
    }
    cout << "Base vector map generated." << endl;

    int count = MERGERULES.size();

    cout << "Number of merge rules: " << count << endl;

    for (int key = 256; key < 256 + count; ++key)
    {
        vectorMap[key] = createVector();
    }
    cout << "Additional vectors generated." << endl;

    return vectorMap;
}

vector<int> TextToIDs(const string &folderPath)
{
    cout << "Generating IDs from text files..." << endl;
    vector<thread> threads;
    mutex resultMutex;
    vector<int> allIds;

    cout << "Reading files from: " << folderPath << endl;
    cout.flush();
    for (const auto &entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            threads.emplace_back([&, path = entry.path()]()
                                 {
                cout << "Reading file: " << path << endl;

                ifstream file(path);
                if (file) {
                    stringstream buffer;
                    buffer << file.rdbuf();
                    string content = buffer.str();
                    vector<int> ids = Encode(content);

                    lock_guard<mutex> lock(resultMutex);
                    allIds.insert(allIds.end(), ids.begin(), ids.end());
                } });
        }
    }

    for (auto &t : threads)
    {
        t.join();
    }

    if (allIds.empty())
    {
        cerr << "Error: No IDs generated from the files." << endl;
    }

    return allIds;
}
