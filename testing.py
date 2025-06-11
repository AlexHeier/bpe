from datasets import load_dataset
import subprocess, statistics, re

# Load the analogy dataset
dataset = load_dataset("tomasmcz/word2vec_analogy", split="train")
print(f"Loaded {len(dataset)} analogies")
print(f"1 is equal, 0 is not related, -1 is opposite")

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

cosine = []
words = []
count = 0
for item in dataset:
    a, b, c, d = item["word_a"], item["word_b"], item["word_c"], item["word_d"]
    cos = run_test(a, b, c, d)
    count += 1
    if cos is not None:
        cosine.append(cos)
        words.append((a, b, c, d))
    if count % 10 == 0:
        print(f"\rDone with {count} tests", end="", flush=True)
    if count % 500 == 0:
        min_index = cosine.index(min(cosine))
        max_index = cosine.index(max(cosine))
        print(f"\nBest cosine similarity case: {words[min_index]} -> {cosine[min_index]:.4f}")
        print(f"Worst cosine similarity case: {words[max_index]} -> {cosine[max_index]:.4f}")

print(f"\nTested {len(cosine)}/{len(dataset)} cases")
print(f"Mean cosine similarity: {statistics.mean(cosine):.4f}")

# Find min and max indices
min_index = cosine.index(min(cosine))
max_index = cosine.index(max(cosine))

print(f"\nBest cosine similarity case: {words[min_index]} -> {cosine[min_index]:.4f}")
print(f"Worst cosine similarity case: {words[max_index]} -> {cosine[max_index]:.4f}")
print("Testing completed.")