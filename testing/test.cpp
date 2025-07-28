#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <climits>
#include <sstream>
#include <utility>

#include "../global.h"
#include "../vocab/vocab.h"

using namespace std;

map<int, vector<float>> vMap;

vector<float> TextToVector(const string &text)
{
    vector<int> encodedText = Encode(text);
    if (encodedText.empty())
        return {};

    int embeddingDim = vMap.begin()->second.size();
    vector<float> vec(embeddingDim, 0.0f);
    int count = 0;

    for (const auto &id : encodedText)
    {
        const auto &wordVec = vMap[id];
        for (int i = 0; i < embeddingDim; ++i)
        {
            vec[i] += wordVec[i];
        }
        count++;
    }

    for (int i = 0; i < embeddingDim; ++i)
    {
        vec[i] /= count;
    }

    return vec;
}

vector<float> Normalize(const vector<float> &vec)
{
    vector<float> out = vec;
    float norm = 0.0f;
    for (float x : vec)
        norm += x * x;
    norm = sqrt(norm);
    if (norm > 0.0f)
        for (float &x : out)
            x /= norm;
    return out;
}

float TestVectors(string one, string two, string three, string target)
{
    int embeddingDim = vMap.begin()->second.size();
    vector<float> testVec(embeddingDim, 0.0f);

    vector<float> vec1 = TextToVector(one);
    vector<float> vec2 = TextToVector(two);
    vector<float> vec3 = TextToVector(three);
    vector<float> vec4 = TextToVector(target);

    if (vec1.empty() || vec2.empty() || vec3.empty() || vec4.empty())
    {
        cerr << "One or more input vectors are empty!" << endl;
        return 0.0f;
    }

    if (vec1.size() != vec2.size() || vec1.size() != vec3.size() || vec1.size() != vec4.size())
    {
        cerr << "Vectors do not have the same length!" << endl;
        return 0.0f;
    }

    for (int i = 0; i < embeddingDim; ++i)
    {
        testVec[i] = vec1[i] - vec2[i] + vec3[i];
    }

    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (int i = 0; i < embeddingDim; ++i)
    {
        dot += testVec[i] * vec4[i];
        normA += testVec[i] * testVec[i];
        normB += vec4[i] * vec4[i];
    }

    if (normA == 0.0f || normB == 0.0f)
    {
        cout << "Zero norm encountered!" << endl;
        return 0.0f;
    }
    return dot / (sqrt(normA) * sqrt(normB));
}

void StartTest(string one, string two, string three, string target)
{
    vMap = LoadVectorsFromBinary();

    for (const auto v : vMap)
    {
        vector<float> normalizedVec = Normalize(v.second);
        vMap[v.first] = normalizedVec;
    }

    if (vMap.empty())
    {
        cerr << "Vector map is empty!" << endl;
        return;
    }

    float cosineSim = TestVectors(one, two, three, target);

    cout << "Relative difference between *" << one << " - *" << two << " + *" << three << " ~= " << target << " : " << cosineSim << endl;
}

void BatchTest(const string filePath)
{

    vector<float> cosineResults;
    pair<string, float> lowestResult = {"", INT_MAX};
    pair<string, float> highestResult = {"", INT_MIN};

    vMap = LoadVectorsFromBinary();

    for (const auto v : vMap)
    {
        vector<float> normalizedVec = Normalize(v.second);
        vMap[v.first] = normalizedVec;
    }

    if (vMap.empty())
    {
        cerr << "Vector map is empty!" << endl;
        return;
    }

    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filePath << endl;
        return;
    }
    int i = 0;
    string line;
    while (getline(file, line))
    {
        i++;
        istringstream iss(line);
        string word1, word2, word3, target;
        if (!(iss >> word1 >> word2 >> word3 >> target))
        {
            continue;
        } // Skip malformed lines

        float cosineSim = TestVectors(word1, word2, word3, target);

        if (cosineSim >= highestResult.second)
        {
            highestResult = {line, cosineSim};
        }
        else if (cosineSim <= lowestResult.second)
        {
            lowestResult = {line, cosineSim};
        }
        cosineResults.push_back(cosineSim);
    }

    float avarage = 0.0f;
    for (const auto &result : cosineResults)
    {
        avarage += result;
    }
    avarage /= cosineResults.size();
    cout << "Processed " << i << " lines" << endl;
    cout << "Avarage cosine similarity: " << avarage << endl;
    cout << "Highest result: '" << highestResult.first << "' with cosine similarity: " << highestResult.second << endl;
    cout << "Lowest result: '" << lowestResult.first << "' with cosine similarity: " << lowestResult.second << endl
         << endl;
}