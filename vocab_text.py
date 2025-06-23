from datasets import load_dataset
import re

# Load 1% of the English Wikipedia dataset
dataset = load_dataset("wikipedia", "20220301.en", split="train[:500]")

output_file = "vocabtext.txt"
end_token = "<|endoftext|>"

def clean(text):
    # Remove excessive whitespace, brackets, etc.
    text = re.sub(r'\s+', ' ', text)             # Collapse whitespace
    text = re.sub(r'\[\d+\]', '', text)          # Remove reference brackets like [1]
    text = re.sub(r'==+[^=]+==+', '', text)      # Remove section headers
    text = text.strip()
    return text

with open(output_file, "w", encoding="utf-8") as f:
    for article in dataset:
        cleaned = clean(article["text"])
        if cleaned:
            f.write(cleaned + f" {end_token}\n")

print(f"Saved {len(dataset)} articles to {output_file}")
