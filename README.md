# Byte Pair Encoding

Training, decoding, encoding and vector training.

To get training data use the python script. Before running it remember to:
``` powershell
pip install datasets
```

run the program with --help to get more information.

(Readme comming later ;))

```sql
CREATE TABLE embeddings (
    id INT UNIQUE,
    vector FLOAT[],
    frequency BIGINT
);
```

## Run

### Training data
To get training data use the python script. Before running it remember to:
``` powershell
pip install datasets
```

### Compile
Windows:
```powershell
g++ -o my_program (Get-ChildItem -Recurse -Filter *.cpp | ForEach-Object { $_.FullName })
```

Linux:
```bash
g++ -o my_program $(find . -name "*.cpp")
```


