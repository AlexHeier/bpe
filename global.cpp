#include <vector>
#include <iostream>
#include <map>
#include <fstream>

#include "global.h"

using namespace std;

map<int, vector<float>> LoadVectorsFromBinary(){
    ifstream in(vectorFile, ios::binary);
    size_t mapSize;
    map<int, vector<float>> vMap;

    in.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
    in.read(reinterpret_cast<char*>(&vectorSize), sizeof(vectorSize));

    for (size_t i = 0; i < mapSize; ++i){
        int key;
        vector<float> vec(vectorSize);

        in.read(reinterpret_cast<char*>(&key), sizeof(key));
        in.read(reinterpret_cast<char*>(vec.data()), vectorSize * sizeof(float));

        vMap[key] = move(vec); // Moves the owner ship of the memory
    }
    return vMap;
}