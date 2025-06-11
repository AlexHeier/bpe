#include <iostream>
#include <map>
#include <vector>
#include <cmath>

#include "..\global.h"
#include "..\vocab\vocab.h"

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

void StartTest(string one, string two, string three, string target)
{
    vMap = LoadVectorsFromBinary();

    if (vMap.empty())
    {
        cerr << "Vector map is empty!" << endl;
        return;
    }

    int embeddingDim = vMap.begin()->second.size();
    vector<float> testVec(embeddingDim, 0.0f);

    vector<float> vec1 = TextToVector(one);
    vector<float> vec2 = TextToVector(two);
    vector<float> vec3 = TextToVector(three);
    vector<float> vec4 = TextToVector(target);

    if (vec1.empty() || vec2.empty() || vec3.empty() || vec4.empty())
    {
        cerr << "One or more input vectors are empty!" << endl;
        return;
    }

    if (vec1.size() != vec2.size() || vec1.size() != vec3.size() || vec1.size() != vec4.size())
    {
        cerr << "Vectors do not have the same length!" << endl;
        return;
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
    float cosineSim = dot / (sqrt(normA) * sqrt(normB));

    cout << "Relative difference between *" << one << " - *" << two << " + *" << three << " ~= " << target << " : " << cosineSim << endl;
}