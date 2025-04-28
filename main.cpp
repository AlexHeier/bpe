#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>

using namespace std;

#include "vocab/vocab.h"
#include "training/training.h"
#include "global.h"

bool parseArguments(int argc, char *argv[])
{
    bool rules = false;

    maxVocabSize = 10000;
    vocabLoops = 10000;
    vectorSize = 2000;
    epochs = 10;
    negativeSamples = 5;
    windowSize = 10;
    learningRate = 0.01f;
    threads = 1;

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];

        if (arg == "-threads" && i + 1 < argc)
        {
            threads = stoi(argv[++i]);
        }
        else if (arg == "-vocabsize" && i + 1 < argc)
        {
            maxVocabSize = stoi(argv[++i]);
        }
        else if (arg == "-vectorSize" && i + 1 < argc)
        {
            vectorSize = stoi(argv[++i]);
        }
        else if (arg == "-epochs" && i + 1 < argc)
        {
            epochs = stoi(argv[++i]);
        }
        else if (arg == "-window" && i + 1 < argc)
        {
            windowSize = stoi(argv[++i]);
        }
        else if (arg == "-lr" && i + 1 < argc)
        {
            learningRate = stof(argv[++i]);
        }
        else if (arg == "-vtrain" && i + 1 < argc)
        {
            RulesFromTraining(argv[++i]);
            rules = true;
        }
        else if (arg == "-vfile" && i + 1 < argc)
        {
            RulesFromFile(argv[++i]);
            rules = true;
        }
        else if (arg == "--help")
        {
            cout << endl;
            cout << "-threads <number> : Set the number of threads to use (default: 1)" << endl;
            cout << "-vocabsize <number> : Set the maximum vocabulary size (default: 10000)" << endl;
            cout << "-vectorSize <number> : Set the size of the vectors (default: 2000)" << endl;
            cout << "-epochs <number> : Set the number of epochs (default: 10)" << endl;
            cout << "-window <number> : Set the window size (default: 10)" << endl;
            cout << "-lr <float> : Set the learning rate (default: 0.01)" << endl;
            cout << "-vtrain <file> : Load rules from training data" << endl;
            cout << "-vfile <file> : Load rules from a file" << endl;
            cout << "--help : Show this help message" << endl;
            cout << endl;
        }
    }
    return rules;
}

int main(int argc, char *argv[])
{

    if (!parseArguments(argc, argv))
    {
        cout << "No rules provided. Use -vtrain or -vfile to provide rules." << endl;
        return 1;
    }

    string fodlerpath = "./training_data/";

    Training(fodlerpath);

    return 0;
}