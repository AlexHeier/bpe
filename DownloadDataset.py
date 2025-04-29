# pip install datasets

from datasets import load_dataset
import os
import re
import sys


output_dir = os.path.join(".", "training_data")
if not os.path.exists(output_dir):
    print(f"Error: The folder '{output_dir}' does not exist!")
    sys.exit(1)


dataset = load_dataset("wikipedia", "20220301.en", split="train", trust_remote_code=True)


def sanitize_filename(title, max_length=100):
    title = re.sub(r'[\\/*?:"<>|]', "_", title)
    return title[:max_length].strip()

article_count = 0
for i, article in enumerate(dataset):
    title = article["title"].strip()
    text = article["text"].strip()

    if not text:
        continue

    filename = sanitize_filename(title)
    file_path = os.path.join(output_dir, f"{filename}_{i}.txt")

    with open(file_path, "w", encoding="utf-8") as f:
        f.write(f"{title}\n{text}\n")

    article_count += 1

    if article_count % 1000 == 0:
        print(f"Saved {article_count} articles...")

print(f"Finished. Total articles saved: {article_count}")
