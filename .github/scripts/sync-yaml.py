#!/usr/bin/env python3
"""
Sync ESPHome YAML configurations to Wiki documentation.

This script reads YAML files from the source projects and updates
corresponding code blocks in wiki markdown files using inline anchor markers.
"""

import argparse
import json
import os
import re
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple


# YAML comment-based anchor markers (inside code blocks)
ANCHOR_START = "# ==== AUTO-SYNC START:"
ANCHOR_END = "# ==== AUTO-SYNC END ===="


def normalize_lines(text: str) -> str:
    """Normalize line endings and strip trailing whitespace on each line."""
    text = text.replace("\r\n", "\n").replace("\r", "\n")
    return "\n".join(line.rstrip() for line in text.split("\n"))


def load_mapping(mapping_path: str) -> Dict[str, Any]:
    """Load the sync mapping configuration."""
    with open(mapping_path, 'r', encoding='utf-8') as f:
        return json.load(f)


def read_yaml_file(yaml_path: str) -> Optional[str]:
    """
    Read a YAML file content, extracting only the content within AUTO-SYNC anchor markers.

    If no anchors are found, returns None to skip syncing.
    This allows source files to control what gets synced.
    """
    with open(yaml_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    content = ''.join(lines)
    
    # Find the anchor markers
    start_marker_pattern = re.compile(rf'{re.escape(ANCHOR_START)} (.+?) ====', re.MULTILINE)
    end_marker = ANCHOR_END
    
    start_match = start_marker_pattern.search(content)
    if not start_match:
        return None  # No anchors, skip syncing
    
    start_pos = start_match.end()
    end_pos = content.find(end_marker, start_pos)
    if end_pos == -1:
        return None  # Incomplete anchors, skip
    
    # Extract content between anchors
    anchored_content = normalize_lines(content[start_pos:end_pos]).strip()
    
    # Remove any remaining anchor lines within the content (defensive)
    anchored_lines = anchored_content.split('\n')
    filtered_lines = [
        line for line in anchored_lines
        if not line.strip().startswith(ANCHOR_START) and
           not line.strip().startswith(ANCHOR_END)
    ]
    
    return normalize_lines('\n'.join(filtered_lines)).strip()


def find_yaml_block_with_anchor(
    content: str,
    yaml_key: str
) -> Tuple[int, int, int, int]:
    """
    Find the code block containing the anchor markers.

    Returns:
        tuple: (code_block_start, code_block_end, content_start, content_end) or (-1, -1, -1, -1)
        - code_block_start: position of opening ```
        - code_block_end: position of closing ```
        - content_start: position after the START marker line
        - content_end: position before the END marker line
    """
    start_marker = f"{ANCHOR_START} {yaml_key} ===="

    # Find all code blocks (with optional language specifier)
    yaml_block_pattern = r'```[^\n]*\n(.*?)\n```'
    for match in re.finditer(yaml_block_pattern, content, re.DOTALL):
        block_start = match.start()
        block_end = match.end()
        block_content = match.group(1)

        # Check if this block contains our start marker
        if start_marker in block_content:
            # Find the start marker position
            start_marker_pos = block_content.find(start_marker)
            # Find the end of the start marker line
            start_line_end = block_content.find('\n', start_marker_pos)
            if start_line_end == -1:
                start_line_end = len(block_content)

            # Find the end marker (must be on its own line)
            end_marker = f"{ANCHOR_END}"
            end_marker_pos = block_content.find(end_marker, start_line_end)

            if end_marker_pos == -1:
                return (-1, -1, -1, -1)

            # Calculate the header length (```yaml\n or ```\n)
            header_end = content.find('\n', block_start)
            header_length = header_end - block_start + 1  # +1 for the \n

            # Calculate absolute positions
            abs_content_start = block_start + header_length + start_line_end + 1  # +1 to skip the \n after start marker line
            abs_content_end = block_start + header_length + end_marker_pos

            return (block_start, block_end, abs_content_start, abs_content_end)

    return (-1, -1, -1, -1)


def find_and_replace_anchor(
    content: str,
    yaml_key: str,
    yaml_content: str
) -> Tuple[str, bool]:
    """
    Find anchor markers in YAML code blocks and replace the content between them.

    Returns:
        tuple: (new_content, was_found)
    """
    block_start, block_end, content_start, content_end = find_yaml_block_with_anchor(
        content, yaml_key
    )

    if block_start == -1:
        return content, False

    # Build the new content
    # Replace content between START and END markers
    yaml_content = normalize_lines(yaml_content).rstrip('\n')
    new_content = (
        content[:content_start] +
        "\n" +
        yaml_content +
        "\n" +
        content[content_end:]
    )

    return new_content, True


def process_target_file(
    target_path: str,
    yaml_key: str,
    yaml_content: str,
    temp_dir: Path
) -> Tuple[bool, Optional[str]]:
    """
    Process a single target markdown file.

    Returns:
        tuple: (was_updated, temp_path_or_error)
    """
    if not os.path.exists(target_path):
        return False, f"File not found: {target_path}"

    with open(target_path, 'r', encoding='utf-8') as f:
        original_content = f.read()

    new_content, anchor_found = find_and_replace_anchor(
        original_content,
        yaml_key,
        yaml_content
    )

    if not anchor_found:
        return False, f"Anchor not found: {yaml_key}"

    if new_content == original_content:
        return False, "No changes needed"

    # Write to temp file
    temp_path = temp_dir / target_path.replace('/', '_')
    temp_path.parent.mkdir(parents=True, exist_ok=True)
    with open(temp_path, 'w', encoding='utf-8', newline='\n') as f:
        f.write(new_content)

    return True, str(temp_path)


def main():
    parser = argparse.ArgumentParser(
        description="Sync ESPHome YAML configs to Wiki documentation"
    )
    parser.add_argument(
        '--mapping',
        required=True,
        help='Path to sync-mapping.json'
    )
    parser.add_argument(
        '--source',
        required=True,
        help='Path to source projects directory'
    )
    parser.add_argument(
        '--target',
        required=True,
        help='Path to target wiki directory'
    )
    parser.add_argument(
        '--changed-files',
        help='JSON array of changed YAML files'
    )
    parser.add_argument(
        '--output',
        default='changes.json',
        help='Output JSON file for changes'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Dry run mode, do not modify files'
    )

    args = parser.parse_args()

    # Load mapping configuration
    mapping = load_mapping(args.mapping)

    # Parse changed files
    if args.changed_files:
        try:
            changed_files = json.loads(args.changed_files)
        except json.JSONDecodeError:
            changed_files = list(mapping.keys())
    else:
        # If no changed files specified, check all mapped files
        changed_files = list(mapping.keys())

    # Filter to only files in mapping (whitelist mode)
    # Exclude metadata keys starting with '_'
    valid_changed_files = [f for f in changed_files if f in mapping and not f.startswith('_')]
    skipped_files = [f for f in changed_files if f not in mapping or f.startswith('_')]

    # Prepare result tracking
    result = {
        'updated_configs': [],
        'updated_files': [],
        'missing_anchors': [],
        'skipped_files': skipped_files,
        'errors': []
    }

    temp_dir = Path(args.target) / '.sync-temp'
    if args.dry_run:
        temp_dir = Path('/tmp/sync-temp')

    has_changes = False

    # Process each changed YAML file
    for yaml_file in valid_changed_files:
        source_path = os.path.join(args.source, yaml_file)

        if not os.path.exists(source_path):
            result['errors'].append(f"Source file not found: {source_path}")
            continue

        yaml_content = read_yaml_file(source_path)
        if yaml_content is None:
            print(f"Skipping {yaml_file}: no valid anchors found in source")
            continue
        config = mapping[yaml_file]
        yaml_file_updated = False

        for target_spec in config.get('targets', []):
            target_path = os.path.join(args.target, target_spec['path'])
            yaml_key = target_spec['yaml_key']

            was_updated, result_info = process_target_file(
                target_path,
                yaml_key,
                yaml_content,
                temp_dir
            )

            if was_updated:
                result['updated_files'].append({
                    'source_file': yaml_file,
                    'target_path': target_spec['path'],
                    'yaml_key': yaml_key,
                    'temp_path': result_info
                })
                yaml_file_updated = True
                has_changes = True
            else:
                if "Anchor not found" in result_info:
                    result['missing_anchors'].append({
                        'source_file': yaml_file,
                        'target_path': target_spec['path'],
                        'yaml_key': yaml_key
                    })
                elif "File not found" in result_info:
                    result['errors'].append(result_info)
                # "No changes needed" is not an error

        if yaml_file_updated:
            result['updated_configs'].append(yaml_file)

    # Add temp_dir to result for later use
    result['temp_dir'] = str(temp_dir)
    result['has_changes'] = has_changes

    # Write output
    output_path = args.output if not args.dry_run else f'/tmp/{os.path.basename(args.output)}'
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(result, f, indent=2, ensure_ascii=False)

    # Print summary
    print(f"Sync Summary:")
    print(f"  Updated configs: {len(result['updated_configs'])}")
    print(f"  Updated files: {len(result['updated_files'])}")
    print(f"  Missing anchors: {len(result['missing_anchors'])}")
    print(f"  Skipped files: {len(result['skipped_files'])}")
    print(f"  Errors: {len(result['errors'])}")

    if result['missing_anchors']:
        print("\nMissing Anchors:")
        for anchor in result['missing_anchors']:
            print(f"  - {anchor['yaml_key']} in {anchor['target_path']}")

    if result['errors']:
        print("\nErrors:")
        for error in result['errors']:
            print(f"  - {error}")

    # Exit with error if there are missing anchors (fail-fast)
    if result['missing_anchors']:
        print("\nERROR: Missing required anchors in target files!")
        sys.exit(1)

    return 0


if __name__ == '__main__':
    sys.exit(main() or 0)