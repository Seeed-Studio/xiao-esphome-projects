#!/usr/bin/env python3
"""
Detect changes in YAML files by comparing anchored content between source and wiki repositories.
"""
import argparse
import json
import os


def extract_anchored_content(content: str, yaml_key: str = None) -> str:
    """
    Extract content between AUTO-SYNC anchors.
    For source: find any START/END.
    For wiki: find specific yaml_key.
    """
    if yaml_key:
        start_marker = f'# ==== AUTO-SYNC START: {yaml_key} ===='
    else:
        start_marker = '# ==== AUTO-SYNC START:'
    
    end_marker = '# ==== AUTO-SYNC END ===='
    
    start = content.find(start_marker)
    if start == -1:
        return None
    
    end = content.find(end_marker, start)
    if end == -1:
        return None
    
    return content[start:end + len(end_marker)].strip()


def main():
    parser = argparse.ArgumentParser(description="Detect YAML content changes")
    parser.add_argument('--mapping', required=True, help='Path to sync-mapping.json')
    parser.add_argument('--source', required=True, help='Path to source repo')
    parser.add_argument('--wiki', required=True, help='Path to wiki repo')
    args = parser.parse_args()
    
    # Load mapping
    with open(os.path.join(args.source, args.mapping), 'r') as f:
        mapping = json.load(f)
    
    changed_files = []
    
    for yaml_file, config in mapping.items():
        source_path = os.path.join(args.source, yaml_file)
        if not os.path.exists(source_path):
            continue
        
        # Read source content
        with open(source_path, 'r') as f:
            source_content = f.read()
        
        # Extract source anchored content
        source_anchored = extract_anchored_content(source_content)
        if not source_anchored:
            continue  # Skip if no anchors in source
        
        for target in config.get('targets', []):
            wiki_path = os.path.join(args.wiki, target['path'])
            yaml_key = target['yaml_key']
            
            if not os.path.exists(wiki_path):
                changed_files.append(yaml_file)
                break
            
            # Read wiki content
            with open(wiki_path, 'r') as f:
                wiki_content = f.read()
            
            # Extract wiki anchored content
            wiki_anchored = extract_anchored_content(wiki_content, yaml_key)
            if not wiki_anchored:
                changed_files.append(yaml_file)
                break
            
            if source_anchored != wiki_anchored:
                changed_files.append(yaml_file)
                break
    
    # Remove duplicates
    changed_files = list(set(changed_files))
    
    # Output to GITHUB_OUTPUT
    output_file = os.environ.get('GITHUB_OUTPUT')
    if output_file:
        with open(output_file, 'a') as f:
            if changed_files:
                print(f"Changed files: {changed_files}")
                f.write(f"has-changes=true\n")
                f.write(f"changed-files={json.dumps(changed_files)}\n")
            else:
                print("No changes detected")
                f.write(f"has-changes=false\n")
                f.write(f"changed-files=[]\n")
    else:
        # Fallback for testing
        if changed_files:
            print(f"has-changes=true")
            print(f"changed-files={json.dumps(changed_files)}")
        else:
            print(f"has-changes=false")
            print(f"changed-files=[]")


if __name__ == '__main__':
    main()