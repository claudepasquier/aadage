# AADAGE architecture

This document maps the implementation to the algorithm described in
*Pasquier et al., "Attributed graph mining in the presence of automorphism",
Knowledge and Information Systems, 2017* (see `../article.pdf`).

## Problem (paper §3)

Mine all frequent attributed subgraphs of one or more directed attributed
graphs, where each node carries a set of items. Frequency is either
per-transaction (number of graphs containing the pattern) or, in the
single-graph setting, the minimum number of unique nodes a pattern node is
mapped to (Bringmann & Nijssen). AADAGE focuses on **content-closed**
patterns (§3.2.2).

## Canonical form (paper §4) → `src/canonical.{h,cpp}`

A pattern is encoded as the depth-first sequence of its spanning-tree nodes,
each node being a triple `(Q, d, p)`: itemset `Q`, depth `d`, and an optional
back-reference `p` for reentrant links. In the code:

| Paper concept | Code |
|---|---|
| Itemset order (§4.1) | `myComp3()` |
| Node code order (§4.2) | `myComp2()`, `Candidates::compare` |
| Pattern code order (§4.3) | lexicographic comparison of triple sequences |
| Serialization for output | `serialize()` |
| Tree navigation on codes | `getParent()`, `getPreviousSibbling()` |
| Substructure comparison | `compareSubStructure()` |
| Canonical test (§5.1) | `isCanonical()` (returns −1/0/1: canonical / canonical but saturated / non-canonical) |
| Cycle canonicity (§5.3) | `isCycleCanonical()` |

## Enumeration (paper §5–6) → `src/aadage.cpp`

| Paper concept | Code |
|---|---|
| Candidate set init (single nodes) | `initializeCandidates()`, `getfrequentNodes()`, `removeUnfrequentNodes()` |
| Rightmost-path extension: itemset / structural (§5) | `extendFirstCandidate()` (itemset extension on the last node, structural extension along the right path) |
| Reentrant links (§5.2) | negative-label back-references inside `extendFirstCandidate()` |
| Cycles (§5.3) | back-reference to a right-path node + `isCycleCanonical()` filter |
| Automorphic patterns (§5.4) | split-node detection and mapping reduction inside `extendFirstCandidate()` |
| Main loop (Fig. 5) | the `while (!theEnd) { performJoin(...); }` loop in `main()` |
| Content-closure test (§6, lines 13–18) | `markPreviousNonClosedCandidates()`, `filterNewCandidates()` |
| Output of closed patterns | final loop in `main()` + `writeSolution()` |

## Data model → per-class files

- `NodeInfo` — node of an input graph: itemset (`labels`), `childNodes`, `depth`.
- `SequenceInfo` — one embedding: index of the graph + list of node positions.
- `CandidateInfo` — occurrence list + closure/frequency bookkeeping.
- `Candidates` — candidate storage: current frontier (`currentCandidates`),
  accepted candidates (`candidates`) and freshly generated ones
  (`newCandidates` keyed by the canonical code).

## I/O → `src/aadage.cpp`

Three readers selected by `getInputFormat()`:
- extended Sleuth/treeMiner format (`processInputFileSleuth`),
- HybridTreeMiner format (`processInputFileHybrid`),
- native AADAGE `g/n/e` format (`processInputFile`).
A pattern file (`-p`) is read by `processPatternFile()`.

## Known limitations / future work

- `aadage.cpp` still holds the mining driver, CLI parsing and I/O; further
  modularisation (Options struct, io/ module) is planned — see the roadmap in
  the repository README history.
- `memUsed()` and `getopt` usage are POSIX-only.
