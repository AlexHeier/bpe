#ifndef GLOBAL_H
#define GLOBAL_H

#include <map>
#include <vector>
#include <string>

using namespace std;

inline map<int, pair<int, int>> MERGERULES;

inline int maxVocabSize;

inline int vocabLoops;

inline size_t vectorSize;

inline int epochs;

inline int negativeSamples;

inline int windowSize;

inline float learningRate;

inline int threads;

inline int documentCount;

inline int repeatCount;


const string vectorFile = "vectors.bin";

map<int, vector<float>> LoadVectorsFromBinary();

#endif