#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>

using namespace std;

#include "vocab/vocab.h"
#include "training/training.h"

int main(int argc, char* argv[]) {
    map<int, pair<int, int>> mergeRules;
    if (argc < 2) {
        cout << "Too Few arguments. Arg1: text, file or database" << endl << "Arg2: file name" << endl;
    }

    string format = argv[1];

    if (format == "training") {
        string filename = argv[2];
        mergeRules = RulesFromTraining(filename);

    } else if (format == "file") {
        string filename = argv[2];
        mergeRules = RulesFromFile(filename);

    } else if (format == "database") {
        // database

    } else {
        cout << "Invalid format. Use 'text', 'file', or 'database'." << endl;
        return 1;
    }

    float loss = Training(mergeRules, "training_data");
    cout << "Training loss: " << loss << endl;

    SaveVectors("vectors.txt");

    return 0;
}