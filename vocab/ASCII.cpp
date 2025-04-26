#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

string loadFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return "";
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return content;
}

vector<int> toASCII(const string& str) {
    vector<int> asciiValues;
    asciiValues.reserve(str.length());

    for (char c : str) {
        asciiValues.push_back(static_cast<unsigned char>(c));
    }

    return asciiValues;
}

vector<int> FileToASCII(const string& filename) {
    string text = loadFile(filename);
    if (text.empty()) {
        return vector<int>();
    }
    return toASCII(text);
}

vector<int> InitASCII(){
    vector<int> asciivalues;
    asciivalues.reserve(256);
    for (int i = 0; i < 256; ++i) {
        asciivalues.push_back(i);
    }
    return asciivalues;
}