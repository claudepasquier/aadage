# AADAGE (Automorphism Aware Directed Attributed Graph Explorer)

Implementation of the attributed graph mining method described in the manuscript:

> Pasquier, C., Flouvat, F., Sanhes, J., & Selmaoui-Folcher, N. (2017). Attributed graph mining in the presence of automorphism. Knowledge and Information Systems, 50, 569-584.

The full paper is available in [`article.pdf`](article.pdf); the mapping between the paper's sections and the source code is described in [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).

## Installation

Clone the repository and change to the project directory
```bash
git clone https://github.com/claudepasquier/aadage.git
cd aadage
```

Build the project with the following command:
```bash
make
```

The command builds AADAGE and the bliss tool (https://users.aalto.fi/~tjunttil/bliss/index.html) that is used by the method.

Additional targets:
```bash
make debug   # unoptimized build with debug symbols
make test    # build and run the unit tests (see tests/README.md)
make clean   # remove build artifacts
```

## Execution
The method can be run with the command:
```bash
./aadage <optional parameters> -i <inputFile>
```
The flag **-i** is used to specify the file storing the graph to be analysed. The optional parameters are the following:

| Flag   | Description |
| -------| ----------- |
| **-c** | Display only closed patterns. The closure concerns the calculation of topological closure and itemset closure. To use only topological/itemset closure, use option **-d** or **-e**.|
| **-d** | Display only patterns with closed itemsets. |
| **-e** | Display only closed tree patterns. |
| **-f** | Output frequency after each frequent pattern. If this option is not specified the output contains the number of each frequent pattern. |
| **-g** | Specifies the maximum allowed gap (default is 0). |
| **-i** | Input file storing the graph(s) to analyse (required). |
| **-m** | Count all occurences of a pattern in a graph. If this option is not specified, a transactional setting is assumed. |
| **-o** | Print frequent patterns to standard output. |
| **-s** | Specifies an absolute min support, or a relative one if value < 1 (default is 1). |
| **-v** | Verbose mode. |
| **-D** | Specify the maxDepth of the result patterns. |

Additional flags accepted by the parser (experimental / less documented, see `usage()` in `src/aadage.cpp`):

| Flag   | Description |
| -------| ----------- |
| **-O** | Ordered search (unordered search by default). |
| **-q** | Search for sequences. |
| **-r** | Search for patterns starting from the root of the trees (transactional setting only). |
| **-u** | Consider that the input graph is undirected. |
| **-y** | Special processing for dense datasets. |
| **-p** | Search for occurrences of the patterns contained in the given pattern file (see *Input Format*). |
| **-N** | Only search for patterns containing these node labels (colon-separated list). |
| **-E** | Exclude patterns containing these node labels (colon-separated list). |
| **-R** | Only explore patterns with this root label. |
| **-x** | Specifies an absolute max support, or a relative one if value < 1 (patterns with a support greater than this value are discarded). |

## Input Format

AADAGE provides support for two distinct data formats. Among them, one format is an extension of the format used by Sleuth, an algorithm for mining labeled trees (https://github.com/zakimjz/SLEUTH). The extended format accepted by AADAGE involves representing each node label, optionally, as a list of attributes separated by colons.

In the second format, which is specific to AADAGE, a graph is represented by a sequence of lines representing a graph, a node or an edge. The start of a new graph is simply specified with the prefix **g** at the beginning of the line. A line describing a node consists of the prefix **n** followed by the node's id and its attributes separated by colons. An edge is described by a line starting with the prefix **e**, followed by the id of the first node and the id of the second node. For example, a graph composed of three nodes with their respective set of attributes equal to {1,2}, {2,3,4} and {4}, and edges between node 1 and the two other nodes can be described with the following:

```
g 1
n 1 1:2
n 2 2:3:4
n 3 4
e 1 2
e 1 3
```
The same graph is expressed in extended Sleuth format by:

```
0 0 5 1:2 2:3:4 -1 4 -1
```

## Example
The file T10K.txt in the dataset folder contains 10000 attributed trees described with the extended Sleuth format.

The list of the 1022 patterns present in at least 50% of the trees is computed with:

```bash
./aadage -i dataset/T10K.txt -s 0.5 -o 
```

The list of the two closed patterns present in at least 50% of the trees is computed with:

```bash
./aadage -i dataset/T10K.txt -s 0.5 -c -o 
```
