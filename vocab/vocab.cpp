#include <iostream>
#include <fstream>
#include <vector>

#include "ASCII.h"
#include "..\global.h"
#include "saveDecodeCommon.h"
using namespace std;

map<int, pair<int, int>> RulesFromTraining(string filename)
{

    vector<int> asciiValues = InitASCII();

    vector<int> asciiText = FileToASCII(filename);

    cout << "Length of vocab training text " << asciiText.size() << endl;

    map<int, pair<int, int>> mergeRules;

    int i = 0;
    while (i < maxVocabSize-255)
    {
        i++;

        pair<int, int> mostCommonPair = FindMostCommon(asciiText);

        if (mostCommonPair.first == -1 && mostCommonPair.second == -1)
        {
            break;
        }

        int newID = asciiValues.size();
        asciiValues.push_back(newID);

        mergeRules[newID] = mostCommonPair;

        vector<int> newText;
        for (int j = 0; j < asciiText.size();)
        {
            if (j < asciiText.size() - 1 && asciiText[j] == mostCommonPair.first && asciiText[j + 1] == mostCommonPair.second)
            {
                newText.push_back(newID);
                j += 2;
            }
            else
            {
                newText.push_back(asciiText[j]);
                j += 1;
            }
        }

        cout << "\rCurrent vocab size: " << (i+1) << flush;

        asciiText = newText;
    }

    SaveMergeRules(mergeRules, "merge_rules.txt");

    return mergeRules;
}

void Decode(int id, const map<int, pair<int, int>> &mergeRules)
{
    if (id < 256)
    {
        cout << static_cast<char>(id);
    }
    else
    {
        auto it = mergeRules.find(id);
        if (it != mergeRules.end())
        {
            Decode(it->second.first, mergeRules);
            Decode(it->second.second, mergeRules);
        }
        else
        {
            cerr << "Unknown ID during decoding: " << id << endl;
        }
    }
}

map<int, pair<int, int>> RulesFromFile(string filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error opening file for reading: " << filename << endl;
        return MERGERULES;
    }

    int id, first, second;
    while (file >> id >> first >> second)
    {
        MERGERULES[id] = {first, second};
    }

    file.close();
    cout << "Merge rule count: " << MERGERULES.size() << endl;
    return MERGERULES;
}

vector<int> Encode(string text)
{
    vector<int> tokens;
    for (char c : text)
    {
        tokens.push_back(static_cast<unsigned char>(c));
    }

    for (const auto &rule : MERGERULES)
    {
        int id = rule.first;
        int first = rule.second.first;
        int second = rule.second.second;

        for (size_t i = 0; i+1 < tokens.size();)
        {
            if (tokens[i] == first && tokens[i+1] == second)
            {
                tokens[i] = id;
                tokens.erase(tokens.begin() + i + 1);
            } else {
                i++;
            }
        }
    }
    return tokens;
}
