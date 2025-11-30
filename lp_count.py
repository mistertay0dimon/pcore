# This script was used when the OS was not on GitHub.
# This util counts % of the programming languages in this project.
import os
from collections import defaultdict

# ANSI colors
COLORS = {
    "C": "\033[90m",             # Grey
    "Assembly": "\033[38;5;94m", # Brown
    "Linker Script": "\033[97m", # White
    "Makefile": "\033[92m",      # Green
    "RESET": "\033[0m"
}

# Language extensions
LANG_EXT = {
    ".c": "C",
    ".h": "C",
    ".asm": "Assembly",
    ".s": "Assembly",
    ".ld": "Linker Script",
    ".lds": "Linker Script",
    ".mk": "Makefile",
}

# File names without extension (special cases)
SPECIAL_FILES = {
    "makefile": "Makefile",
}

def analyze_directory(root):
    stats = defaultdict(lambda: {"lines": 0, "bytes": 0, "files": 0})

    for dirpath, _, files in os.walk(root):
        for fname in files:
            name_lower = fname.lower()
            ext = os.path.splitext(fname)[1].lower()

            lang = None

            # Check special names like "Makefile"
            if name_lower in SPECIAL_FILES:
                lang = SPECIAL_FILES[name_lower]

            # Check extension
            elif ext in LANG_EXT:
                lang = LANG_EXT[ext]

            else:
                continue

            full_path = os.path.join(dirpath, fname)

            try:
                with open(full_path, "r", errors="ignore") as f:
                    lines = sum(1 for _ in f)
                size = os.path.getsize(full_path)

                stats[lang]["lines"] += lines
                stats[lang]["bytes"] += size
                stats[lang]["files"] += 1

            except Exception as e:
                print(f"Error reading {full_path}: {e}")

    return stats

def print_stats(stats):
    total_lines = sum(v["lines"] for v in stats.values())
    total_bytes = sum(v["bytes"] for v in stats.values())
    total_files = sum(v["files"] for v in stats.values())

    print("===PER LANGUAGE===")
    for lang, data in stats.items():
        color = COLORS.get(lang, COLORS["RESET"])

        line_percent = (data["lines"] / total_lines * 100) if total_lines else 0
        byte_percent = (data["bytes"] / total_bytes * 100) if total_bytes else 0
        file_percent = (data["files"] / total_files * 100) if total_files else 0

        print(
            f"{color}{lang}{COLORS['RESET']}: "
            f"{data['lines']} lines ({line_percent:.2f}%), "
            f"{data['bytes']} bytes ({byte_percent:.2f}%), "
            f"{data['files']} files ({file_percent:.2f}%)"
        )

    print("\n===TOTAL===")
    print(f"Total lines:  {total_lines}")
    print(f"Total bytes:  {total_bytes}")
    print(f"Total files:  {total_files}")

if __name__ == "__main__":
    current_dir = os.getcwd()
    stats = analyze_directory(current_dir)
    print_stats(stats)