# AADAGE unit tests

Dependency-free unit tests for the extracted modules. Run with:

```bash
make test    # from the aadage/ directory
# or
cd tests && make test
```

## Current coverage

- `test_canonical.cpp` — canonical-form module (`src/canonical.{h,cpp}`):
  - **paper Fig. 2**: the four isomorphic patterns must produce the code
    order `code(P4) < code(P3) < code(P2) < code(P1)` (Sect. 4.3).
  - tree navigation helpers (`getParent`, `getPreviousSibbling`).
  - canonical acceptance/rejection of simple chains, sorted/unsorted
    siblings and automorphic patterns (Sect. 5.1, `isCanonical`).
  - itemset ordering per Sect. 4.1 (`myComp3`).
  - `serialize` output format.

## Regression (golden) tests

The end-to-end outputs of the binary are stored under `golden/`. After any
refactor, verify byte-identical behaviour with:

```bash
make aadage
./aadage -i dataset/T10K.txt -s 0.5 -o  | diff - tests/golden/T10K_s0.5.txt
./aadage -i dataset/T10K.txt -s 0.5 -c -o | diff - tests/golden/T10K_s0.5_closed.txt
```
