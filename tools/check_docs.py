"""Check relative Markdown links, local fragments and required chapter sections."""
from pathlib import Path
import re
import sys
root = Path(__file__).resolve().parents[1]
errors = []
sections = ['Learning Objectives','Why Do We Need This?','Concept','Architecture','How It Works',
            'Important Functions','Minimal Example','Line-by-Line Explanation','Compilation',
            'Execution','Expected Output','Experiment','Modify the Code','Common Errors',
            'Debugging Tips','Practice Problems','Exam Questions','Viva Questions',
            'Interview Questions','Mini Project','Chapter Summary','Next Chapter']
def anchors(text):
    found = set(); used = {}
    for title in re.findall(r'^#{1,6}\s+(.+)$',text,re.M):
        # GitHub removes punctuation and retains hyphens and spaces.
        slug = ''.join(c for c in title.lower() if c.isalnum() or c in '-_ ' ).replace(' ','-')
        count = used.get(slug,0); used[slug] = count+1
        found.add(slug + (f'-{count}' if count else ''))
    return found
for file in root.rglob('*.md'):
    body = file.read_text()
    if body.count('```') % 2: errors.append(f'{file.relative_to(root)}: unmatched code fence')
    # Code examples can contain illustrative Markdown: only prose links are actual navigation.
    prose = re.sub(r'```.*?```','',body,flags=re.S)
    for link in re.findall(r'!?\[[^\]]*\]\(([^)]+)\)',prose):
        url = link.split()[0].strip('<>')
        if re.match(r'^[a-zA-Z]+:',url): continue
        path, _, fragment = url.partition('#')
        target = (file.parent / path).resolve() if path else file
        if not target.exists(): errors.append(f'{file.relative_to(root)}: missing {url}')
        elif fragment and target.is_file() and target.suffix == '.md':
            if fragment not in anchors(target.read_text()): errors.append(f'{file.relative_to(root)}: unknown anchor {url}')
for chapter in sorted(root.glob('[0-9][0-9]-*')):
    readme = chapter/'README.md'
    if not readme.exists(): errors.append(f'{chapter.name}: missing README'); continue
    content = readme.read_text()
    for title in sections:
        if not re.search(r'^## .*'+re.escape(title)+r'\s*$',content,re.M): errors.append(f'{chapter.name}: missing section {title}')
if errors:
    print('\n'.join(errors)); sys.exit(1)
print(f'Documentation checks passed: {len(list(root.rglob("*.md")))} Markdown files; 14 chapter templates.')
