# Byte Pair Encoding

This repository provides a comprehensive toolkit for Byte Pair Encoding (BPE), encompassing functionalities for training, encoding, decoding, and vector training.


## Fetures

- Training BPE vocabulary
- BPE encoding and decoding
- Generating and training vectors for vocabulary
- Downloading dataset (wikipedia, HugingFace)


## Command Line Options

| Option                  | Description                                                                 |
|-------------------------|-----------------------------------------------------------------------------|
| `-threads <number>`     | Set the number of threads to use (default: `4`)                             |
| `-vocabsize <number>`   | Set the maximum vocabulary size (default: `10000`)                          |
| `-vectorSize <number>`  | Set the size of the vectors (default: `2000`)                               |
| `-epochs <number>`      | Set the number of epochs (default: `10`)                                    |
| `-window <number>`      | Set the window size for context words (default: `10`)                       |
| `-repeat <number>`      | Set the number of training repeat times (default: `1`)                      |
| `-negative <number>`    | Set the number of negative samples for training (default: `5`)              |
| `-documentCount <num>`  | Set the number of documents to process (default: `10`)                      |
| `-lr <float>`           | Set the initial learning rate (default: `0.01`). Range: `0.00001 - 0.5`     |
| `-vtrain <file>`        | Load vocabulary training data from a file                                   |
| `-vfile <file>`         | Load vocabulary rules from a file                                           |
| `-test`                 | Run a semantic test on the `vectors.bin` file. Format: `w1 - w2 + w3 ~= w4` |
| `-help`                 | Display the help message                                                    |

## Getting started

### Installation
To clone the repository:
```powershell
   git clone https://github.com/AlexHeier/bpe.git
```

### Training data
Before running ```DownloadDataset.py```. You will need the rependencie:
``` powershell
pip install datasets
```

To install the dataset:
``` powershell
python ./DownloadDataset.py
```

### Compile
This project requres a way to compile CPP code. E.g: g++.

Windows:
```powershell
g++ -o my_bpe (Get-ChildItem -Recurse -Filter *.cpp | ForEach-Object { $_.FullName })
```

Linux:
```bash
g++ -o my_bpe $(find . -name "*.cpp")
```

## License
This project is licensed under the MIT License.