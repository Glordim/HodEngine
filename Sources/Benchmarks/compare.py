#!/usr/bin/env python3
"""
Compare Hod vs Std benchmark results from a Google Benchmark JSON output.

Usage:
    BenchmarksCore.exe --benchmark_out=results.json --benchmark_out_format=json
    python compare.py results.json
"""

import json
import sys


def fmt_time(ns):
    if ns >= 1_000_000_000:
        return f"{ns / 1_000_000_000:.2f} s "
    if ns >= 1_000_000:
        return f"{ns / 1_000_000:.2f} ms"
    if ns >= 1_000:
        return f"{ns / 1_000:.2f} us"
    return f"{ns:.2f} ns"


def parse_name(name):
    """Return (impl, operation) or (None, None)."""
    for impl in ("Hod", "Std"):
        if name.startswith(impl + "_"):
            return impl, name[len(impl) + 1:]
    return None, None


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <benchmark_output.json>")
        sys.exit(1)

    with open(sys.argv[1]) as f:
        data = json.load(f)

    # Group by operation, keeping insertion order for each impl
    ops_order = []
    benchmarks = {}
    for b in data.get("benchmarks", []):
        # Skip aggregate entries (mean, median, stddev)
        if b.get("run_type") == "aggregate":
            continue
        impl, op = parse_name(b["name"])
        if impl is None:
            continue
        if op not in benchmarks:
            benchmarks[op] = {}
            ops_order.append(op)
        benchmarks[op][impl] = b["cpu_time"]

    COL_OP    = 42
    COL_TIME  = 14
    COL_RATIO = 9

    sep = "-" * (COL_OP + COL_TIME * 2 + COL_RATIO + 20)
    header = (
        f"{'Benchmark':<{COL_OP}}"
        f"{'Hod':>{COL_TIME}}"
        f"{'Std':>{COL_TIME}}"
        f"{'Ratio':>{COL_RATIO}}"
        f"  Winner"
    )

    print(sep)
    print(header)
    print(sep)

    current_group = None

    for op in ops_order:
        entry = benchmarks[op]
        if "Hod" not in entry or "Std" not in entry:
            continue

        # Print a blank line between benchmark groups (e.g. String_ vs Vector_)
        group = op.split("_")[0]
        if current_group is not None and group != current_group:
            print()
        current_group = group

        hod = entry["Hod"]
        std = entry["Std"]
        ratio = hod / std  # >1 means Hod is slower
        winner = "Hod ✓" if ratio <= 1.0 else "Std"
        arrow = f"{ratio:.2f}x"

        print(
            f"{op:<{COL_OP}}"
            f"{fmt_time(hod):>{COL_TIME}}"
            f"{fmt_time(std):>{COL_TIME}}"
            f"{arrow:>{COL_RATIO}}"
            f"  {winner}"
        )

    print(sep)


if __name__ == "__main__":
    main()
