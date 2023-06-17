# AADAGE (Automorphism Aware Directed Attributed Graph Explorer)

Implementation of the attributed graph mining method described in the manuscript:

>Pasquier, C., Flouvat, F., Sanhes, J., & Selmaoui-Folcher, N. (2017). Attributed graph mining in the presence of automorphism. Knowledge and Information Systems, 50, 569-584.

## Installation

Clone the repository and change to the project directory
```bash
git clone https://github.com/claudepasquier/aadage.git
cd aadage
```

build the project with the following command:
```bash
makefile
```

The command builds AADAGE and the bliss tool (https://users.aalto.fi/~tjunttil/bliss/index.html) that is used by the method.

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
| **-e** | Display only closed graph patterns. |
| **-f** | Output frequency after each frequent pattern. If this option is not specified the output contains the number of each frequent pattern. |
| **-m** | Count all occurences of a pattern in a graph. If this option is not specified, a transactional setting is assumed. |
| **-o** | Print frequent patterns to standard output. |
| **-v** | Verbose mode. |
| **-s** | specifies an absolute min support, or a relative one if value < 1 (default is 1).|
| **-D** | Specify the maxDepth of the result patterns. |
| **-g** | Specifies the maximum allowed gap (default is 0). |

## Input Format

ADDAGE provides support for two distinct data formats. Among them, one format is an extension of the format use by Sleuth, an algorithm for mining labeled trees (https://github.com/zakimjz/SLEUTH). The extended format accepted by AADAGE involves representing each node label, optionnaly, as a list of attributes separated by colons.

In the second format, which is specific to AADAGE, a graph is represented by a sequence of lines representing a graph, a node or an edge. The start of a new graph is simply specified with the prefix **g** at the beginning of the line. A line describing a node consists of the prefix **n** followed by the node's id and its attributes separated by colons. An edge is described by a line starting with the prefix **e**, followed by the id of the first node and the id of the second node. For example, a graph composed of three nodes with their respective set of attibutes equal to {1,2}, {2,3,4} and {4}, and edges between node 1 and the two other nodes can be described with the following:

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
The file T10K.txt in the dataset folded contains 10000 attributed trees described with the extended Sleuth format.

The list of the 1022 patterns present in at least 50\% of the trees is computed with:

```bash
./aadage -i dataset/T10K.txt -s 0.5 -o 
```

The list of the two closed patterns present in at least 50\% of the trees is computed with:

```bash
./aadage -i dataset/T10K.txt -s 0.5 -c -o 
```
