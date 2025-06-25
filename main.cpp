#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <filesystem>
#include <string>

using namespace std;

#include "vocab/vocab.h"
#include "training/training.h"
#include "global.h"
#include "testing/test.h"

bool parseArguments(int argc, char *argv[])
{
    bool newRules = false;

    maxVocabSize = 25000;
    vectorSize = 768; // Same as ChatGPT 2
    epochs = 10;
    negativeSamples = 12;
    windowSize = 8;
    learningRate = 0.01f;
    threads = 12;
    documentCount = 500;
    repeatCount = 5;
    trainVectors = false;

    string RulesFile;
    string RulesTraining;

    cout << "---------------------------------------------------------------------------------------\n" << endl;

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];

        if (arg == "-threads" && i + 1 < argc)
        {
            threads = stoi(argv[++i]);
            cout << "Threads: " << threads << endl;
        }
        else if (arg == "-vocabsize" && i + 1 < argc)
        {
            maxVocabSize = stoi(argv[++i]);
            cout << "Max vocab size: " << maxVocabSize << endl;
        }
        else if (arg == "-vectorSize" && i + 1 < argc)
        {
            vectorSize = stoi(argv[++i]);
            cout << "Vector size: " << vectorSize << endl;
        }
        else if (arg == "-epochs" && i + 1 < argc)
        {
            epochs = stoi(argv[++i]);
            cout << "Epochs: " << epochs << endl;
        }
        else if (arg == "-window" && i + 1 < argc)
        {
            windowSize = stoi(argv[++i]);
            cout << "Window size: " << windowSize << endl;
        }
        else if (arg == "-negative" && i + 1 < argc)
        {
            negativeSamples = stoi(argv[++i]);
            cout << "Negative samples: " << negativeSamples << endl;
        }
        else if (arg == "-documentCount" && i + 1 < argc)
        {
            documentCount = stoi(argv[++i]);
            cout << "Document count: " << documentCount << endl;
        }
        else if (arg == "-repeat" && i + 1 < argc)
        {
            repeatCount = stoi(argv[++i]);
            cout << "Repeat count: " << repeatCount << endl;
        }
        else if (arg == "-lr" && i + 1 < argc)
        {
            learningRate = clamp(stof(argv[++i]), 1e-5f, 0.5f);
            cout << "Learning rate: " << learningRate << endl;
        }
        else if (arg == "-vtrain" && i + 1 < argc)
        {
            RulesTraining = argv[++i];
            
        }
        else if (arg == "-vfile" && i + 1 < argc)
        {
            RulesFile = argv[++i];        
        }
         else if (arg == "-test" && i + 4 < argc)
        {
            StartTest(argv[i+1], argv[i+2], argv[i+3], argv[i+4]);
            cout << "---------------------------------------------------------------------------------------\n" << endl;
            exit(0);
            
        }
        else if (arg == "-vecTrain")
        {
            trainVectors = true;
            cout << "Training vectors enabled" << endl;
            
        }
        else if (arg == "-help")
        {
            cout << endl;
            cout << "-threads <number> : Set the number of threads to use (default: 12)" << endl;
            cout << "-vocabsize <number> : Set the maximum vocabulary size (default: 25000)" << endl;
            cout << "-vectorSize <number> : Set the size of the vectors (default: 1024)" << endl;
            cout << "-epochs <number> : Set the number of epochs (default: 10)" << endl;
            cout << "-window <number> : Set the window size (default: 8)" << endl;
            cout << "-negative <number> : Set the number of negative samples (default: 12)" << endl;
            cout << "-documentCount <number> : Set the number of documents to process (default: 500)" << endl;
            cout << "-repeat <number> : Set the number of repeat times (default: 5)" << endl;
            cout << "-lr <float> : Set the start learning rate (default: 0.01). Range 0.00001 - 0.5" << endl;
            cout << "-vtrain <file> : Load rules from training data" << endl;
            cout << "-vfile <file> : Load rules from a file" << endl;

            cout << endl << "-vecTrain : Train vectors from file or vocabulary. Training data location: ./training_data/" << endl;
            cout << endl << "-test <w1> <w2> <w3> <w4> : Run a semantic test on the vectors.bin file. Returns cosine similarity from -1 to 1"  << endl;
            cout << "  Test format: word1 - word2 + word3 ~= word4" << endl;
            cout << endl << "-help : Show this help message" << endl;
            cout << endl;
        }
        
    }

    if (!RulesFile.empty()){
        cout << "Loading rules from file: " << RulesFile << endl;
        RulesFromFile(RulesFile);
        newRules = true;
    }
    if (!RulesTraining.empty()){
        cout << "Training rules from: " << RulesTraining << endl;
        RulesFromTraining(RulesTraining);
        newRules = true;
    }
    

    cout << "\n---------------------------------------------------------------------------------------\n" << endl;

    return newRules;
}

int main(int argc, char *argv[])
{
    if (!parseArguments(argc, argv))
    {
        cout << "No rules provided. Use -vtrain or -vfile to provide rules." << endl;
        return 1;
    }
    
    if (trainVectors){
        string fodlerpath = "./training_data/";
        Training(fodlerpath);
    }
    
    return 0;
}