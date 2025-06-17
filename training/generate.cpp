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

vector<int> TextToIDs(const vector<string> &fileNames)
{
    vector<int> allIds;
    vector<thread> threads;
    mutex resultMutex;
    int errors = 0;
    mutex err;

    for (const auto &fileName : fileNames)
    {
        threads.emplace_back([&, path = fileName]() {

            ifstream file(path);
            if (file)
            {
                stringstream buffer;
                buffer << file.rdbuf();
                string content = buffer.str();
                vector<int> ids = Encode(content);

                lock_guard<mutex> lock(resultMutex); // Ensure thread safety when modifying shared resource
                allIds.insert(allIds.end(), ids.begin(), ids.end());
            }
            else
            {
                lock_guard<mutex> lock(err);
                errors++;
            }
        });
    }

    // Wait for all threads to finish
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

