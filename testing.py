from datasets import load_dataset
import subprocess, statistics, re

# Load the analogy dataset
dataset = load_dataset("tomasmcz/word2vec_analogy", split="train")
print(f"Loaded {len(dataset)} analogies")

def run_test(a, b, c, d):
    try:
        res = subprocess.run(
            ["./my_bpe.exe", "-test", a, b, c, d],
            capture_output=True, text=True, check=True
        )
        out = res.stdout.strip()
        m = re.search(r":\s*([0-9.+-eE]+)", out)
        return float(m.group(1)) if m else None
    except subprocess.CalledProcessError:
        return None

diffs = []
count = 0
for item in dataset:
    a, b, c, d = item["word_a"], item["word_b"], item["word_c"], item["word_d"]
    diff = run_test(a, b, c, d)
    count += 1
    if diff is not None:
        diffs.append(diff)
    if count > 20000:
        break

print(f"Tested {len(diffs)}/{len(dataset)} cases")
print(f"Mean diff: {statistics.mean(diffs):.4f}")
print(f"Min diff:  {min(diffs):.4f}")
print(f"Max diff:  {max(diffs):.4f}")
