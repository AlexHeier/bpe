from datasets import load_dataset
import subprocess
import os

# Load dataset
dataset = load_dataset("tomasmcz/word2vec_analogy", split="train")
print(f"Loaded {len(dataset)} analogies")

# Write to temp file
temp_path = "analogy_temp.txt"
with open(temp_path, "w") as f:
    for item in dataset:
        a, b, c, d = item["word_a"], item["word_b"], item["word_c"], item["word_d"]
        f.write(f"{a} {b} {c} {d}\n")

# Run batch test and stream output directly to terminal
try:
    subprocess.run(["./my_bpe.exe", "-batchTest", temp_path], check=True)
finally:
    if os.path.exists(temp_path):
        os.remove(temp_path)
