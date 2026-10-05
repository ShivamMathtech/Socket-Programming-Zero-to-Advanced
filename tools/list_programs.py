from pathlib import Path
import json
root = Path(__file__).resolve().parents[1]
for name, info in json.loads((root / 'programs.json').read_text()).items():
    print(f'{name:20} {info["source"]}')
