import os
import re

directories = ['core', 'frontends', 'tests']

for directory in directories:
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.c') or file.endswith('.h'):
                filepath = os.path.join(root, file)
                with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                # Check if it has SFG_
                if 'SFG_' in content:
                    new_content = content.replace('SFG_', 'LHR_')
                    
                    with open(filepath, 'w', encoding='utf-8') as f:
                        f.write(new_content)
                    print(f"Updated {filepath}")
