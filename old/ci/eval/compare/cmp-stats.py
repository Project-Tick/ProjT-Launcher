#!/usr/bin/env python3
# =============================================================================
# ProjT Launcher - Build Statistics Comparison Tool
# =============================================================================
# Compares build statistics between two builds/commits.
# Used by CI to detect performance regressions or improvements.
#
# Usage:
#   python cmp-stats.py --explain before_stats/ after_stats/
# =============================================================================

import argparse
import json
import os
from pathlib import Path
from tabulate import tabulate
from typing import Final


def flatten_data(json_data: dict) -> dict:
    """
    Extracts and flattens metrics from JSON data.
    Handles nested structures by using dot notation.

    Args:
        json_data (dict): JSON data containing metrics.
    Returns:
        dict: Flattened metrics with keys as metric names.
    """
    flat_metrics = {}
    for key, value in json_data.items():
        if isinstance(value, (int, float)):
            flat_metrics[key] = value
        elif isinstance(value, dict):
            for subkey, subvalue in value.items():
                if isinstance(subvalue, (int, float)):
                    flat_metrics[f"{key}.{subkey}"] = subvalue
        elif isinstance(value, str):
            flat_metrics[key] = value

    return flat_metrics


def load_all_metrics(path: Path) -> dict:
    """
    Loads all stats JSON files from the specified path.

    Args:
        path (Path): Directory or file containing JSON stats.

    Returns:
        dict: Dictionary with filenames as keys and metrics as values.
    """
    metrics = {}
    
    if path.is_dir():
        for json_file in path.glob("**/*.json"):
            try:
                with json_file.open() as f:
                    data = json.load(f)
                metrics[str(json_file.relative_to(path))] = flatten_data(data)
            except (json.JSONDecodeError, IOError) as e:
                print(f"Warning: Could not load {json_file}: {e}")
    elif path.is_file():
        try:
            with path.open() as f:
                metrics[path.name] = flatten_data(json.load(f))
        except (json.JSONDecodeError, IOError) as e:
            print(f"Warning: Could not load {path}: {e}")

    return metrics


METRIC_EXPLANATIONS: Final[str] = """
### Metric Explanations

| Metric | Description |
|--------|-------------|
| build.time | Total build time in seconds |
| build.memory | Peak memory usage in MB |
| compile.units | Number of compilation units |
| link.time | Linking time in seconds |
| test.passed | Number of tests passed |
| test.failed | Number of tests failed |
| binary.size | Final binary size in bytes |
"""


def compare_metrics(before: dict, after: dict) -> tuple:
    """
    Compare metrics between two builds.
    
    Returns:
        tuple: (changed_metrics, unchanged_metrics)
    """
    changed = []
    unchanged = []
    
    # Get all metric keys from both
    all_keys = sorted(set(list(before.keys()) + list(after.keys())))
    
    for key in all_keys:
        before_val = before.get(key)
        after_val = after.get(key)
        
        if before_val is None or after_val is None:
            continue
            
        if isinstance(before_val, (int, float)) and isinstance(after_val, (int, float)):
            if before_val == after_val:
                unchanged.append({
                    "metric": key,
                    "value": before_val
                })
            else:
                diff = after_val - before_val
                pct_change = (diff / before_val * 100) if before_val != 0 else float('inf')
                changed.append({
                    "metric": key,
                    "before": before_val,
                    "after": after_val,
                    "diff": diff,
                    "pct_change": pct_change
                })
    
    return changed, unchanged


def format_results(changed: list, unchanged: list, explain: bool) -> str:
    """Format comparison results as markdown."""
    result = ""
    
    if unchanged:
        result += "## Unchanged Values\n\n"
        result += tabulate(
            [[m["metric"], m["value"]] for m in unchanged],
            headers=["Metric", "Value"],
            tablefmt="github"
        )
        result += "\n\n"
    
    if changed:
        result += "## Changed Values\n\n"
        result += tabulate(
            [[
                m["metric"],
                f"{m['before']:.4f}" if isinstance(m['before'], float) else m['before'],
                f"{m['after']:.4f}" if isinstance(m['after'], float) else m['after'],
                f"{m['diff']:+.4f}" if isinstance(m['diff'], float) else m['diff'],
                f"{m['pct_change']:+.2f}%" if isinstance(m['pct_change'], float) else "N/A"
            ] for m in changed],
            headers=["Metric", "Before", "After", "Diff", "Change %"],
            tablefmt="github"
        )
        result += "\n\n"
    
    if explain:
        result += METRIC_EXPLANATIONS
    
    if not changed and not unchanged:
        result = "No comparable metrics found.\n"
    
    return result


def main():
    parser = argparse.ArgumentParser(
        description="Build statistics comparison for ProjT Launcher"
    )
    parser.add_argument(
        "--explain", 
        action="store_true", 
        help="Include metric explanations"
    )
    parser.add_argument(
        "before", 
        help="File or directory containing baseline stats"
    )
    parser.add_argument(
        "after", 
        help="File or directory containing comparison stats"
    )

    args = parser.parse_args()

    before_path = Path(args.before)
    after_path = Path(args.after)
    
    if not before_path.exists():
        print(f"Error: {before_path} does not exist")
        return 1
        
    if not after_path.exists():
        print(f"Error: {after_path} does not exist")
        return 1

    before_metrics = load_all_metrics(before_path)
    after_metrics = load_all_metrics(after_path)
    
    # Merge all metrics from all files
    merged_before = {}
    merged_after = {}
    
    for metrics in before_metrics.values():
        merged_before.update(metrics)
    for metrics in after_metrics.values():
        merged_after.update(metrics)
    
    changed, unchanged = compare_metrics(merged_before, merged_after)
    
    output = format_results(changed, unchanged, args.explain)
    print(output)
    
    return 0


if __name__ == "__main__":
    exit(main())
