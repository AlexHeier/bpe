#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>

using namespace std;

#include "vocab/vocab.h"
#include "training/training.h"
#include "global.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cout << "Too Few arguments. Arg1: text, file or database" << endl << "Arg2: file name" << endl;
    }

    string format = argv[1];

    if (format == "training") {
        string filename = argv[2];
        RulesFromTraining(filename);

    } else if (format == "file") {
        string filename = argv[2];
        RulesFromFile(filename);

    } else if (format == "database") {
        // database

    } else {
        cout << "Invalid format. Use 'text', 'file', or 'database'." << endl;
        return 1;
    }

    string fodlerpath = "./training_data/";

    Training(fodlerpath);

    return 0;
}