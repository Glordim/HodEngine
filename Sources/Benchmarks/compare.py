#!/usr/bin/env python3
"""
Compare two benchmark implementations from a Google Benchmark JSON output.

Usage:
    BenchmarksCore.exe --benchmark_out=results.json --benchmark_out_format=json
    python compare.py results.json [baseline] [candidate]

If baseline and candidate are not specified, the first two impls found in the
JSON (by order of appearance) are used.
"""

import json
import sys

CYAN   = "\033[96m"
GREEN  = "\033[32m"
YELLOW = "\033[33m"
RED    = "\033[31m"
RESET  = "\033[0m"


def color(text, code):
    return f"{code}{text}{RESET}"


def fmt_time(ns):
    if ns >= 1_000_000_000:
        return f"{ns / 1_000_000_000:.2f} s "
    if ns >= 1_000_000:
        return f"{ns / 1_000_000:.2f} ms"
    if ns >= 1_000:
        return f"{ns / 1_000:.2f} us"
    return f"{ns:.2f} ns"


def parse_name(name, impls):
    """Return (impl, operation) or (None, None)."""
    for impl in impls:
        if name.startswith(impl + "_"):
            return impl, name[len(impl) + 1:]
    return None, None


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <benchmark_output.json> [baseline] [candidate]")
        sys.exit(1)

    with open(sys.argv[1]) as f:
        data = json.load(f)

    if len(sys.argv) >= 4:
        impl_baseline, impl_candidate = sys.argv[2], sys.argv[3]
        impls = [impl_baseline, impl_candidate]
    else:
        # Discover impls from the data (first two encountered, in order)
        impls = []
        for b in data.get("benchmarks", []):
            if b.get("run_type") == "aggregate":
                continue
            prefix = b["name"].split("_")[0]
            if prefix not in impls:
                impls.append(prefix)
            if len(impls) == 2:
                break

        if len(impls) < 2:
            print("Error: could not find two implementations in the benchmark data.")
            sys.exit(1)

        impl_baseline, impl_candidate = impls[0], impls[1]

    # Group by operation, keeping insertion order for each impl
    ops_order = []
    benchmarks = {}
    for b in data.get("benchmarks", []):
        # Skip aggregate entries (mean, median, stddev)
        if b.get("run_type") == "aggregate":
            continue
        impl, op = parse_name(b["name"], impls)
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
        f"{impl_baseline:>{COL_TIME}}"
        f"{impl_candidate:>{COL_TIME}}"
        f"{'Ratio':>{COL_RATIO}}"
        f"  Winner"
    )

    print(sep)
    print(header)
    print(sep)

    current_group = None

    for op in ops_order:
        entry = benchmarks[op]
        if impl_baseline not in entry or impl_candidate not in entry:
            continue

        # Print a blank line between benchmark groups (e.g. String_ vs Vector_)
        group = op.split("_")[0]
        if current_group is not None and group != current_group:
            print()
        current_group = group

        baseline  = entry[impl_baseline]
        candidate = entry[impl_candidate]
        ratio = baseline / candidate  # >1 means baseline is slower
        winner = f"{impl_baseline} {color('✓', GREEN)}" if ratio <= 1.0 else impl_candidate
        arrow = f"{ratio:.2f}x"
        ratio_col = color(f"{arrow:>{COL_RATIO}}", GREEN if ratio <= 1.0 else RED)

        print(
            f"{color(op, CYAN):<{COL_OP + len(CYAN) + len(RESET)}}"
            f"{color(f'{fmt_time(baseline):>{COL_TIME}}', YELLOW)}"
            f"{color(f'{fmt_time(candidate):>{COL_TIME}}', YELLOW)}"
            f"{ratio_col}"
            f"  {winner}"
        )

    print(sep)


if __name__ == "__main__":
    main()
