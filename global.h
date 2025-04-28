#ifndef GLOBAL_H
#define GLOBAL_H

#include <map>

using namespace std;

inline map<int, pair<int, int>> MERGERULES;

inline int maxVocabSize = 10000;

inline int vocabLoops = 10000;

inline int vectorSize = 2000;

inline int epochs = 10;

inline int negativeSamples = 5;

inline int windowSize = 10;

inline float learningRate = 0.1f;

inline int threads = 16;

#endif