//----------------------------------------------------------------------------+
//   This file is part of AADAGE                                              |
//                                                                            |
//   AADAGE is free software: you can redistribute it and/or modify it        |
//   under the terms of the GNU General Public License as published by        |
//   the Free Software Foundation, either version 3 of the License, or        |
//   (at your option) any later version.                                      |
//   You should have received a copy of the GNU General Public License        |
//   along with AADAGE. If not, see <http://www.gnu.org/licenses/>.           |
//                                                                            |
//   Author: Claude Pasquier (I3S Laboratory, CNRS, Université Côte d'Azur)   |
//   Contact: claude.pasquier@univ-cotedazur.fr                               |
//   Created on december 2012                                                 |
//                                                                            |
//----------------------------------------------------------------------------+
// Entry point of the method.                                                 |
//----------------------------------------------------------------------------+

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <deque>
#include <iterator>
#include <set>
#include <algorithm>
#include <map>
#include "NodeInfo.h"
#include "SequenceInfo.h"
#include "Candidates.h"
#include "common.h"
#include "limits.h"
#include <sys/time.h>

#include "graph.hh"

#include <unistd.h>

// needed to get memory usage
#include "sys/types.h"
#include "sys/sysinfo.h"

using namespace std;
/* global variables */
bool verbose = false;
bool orderedSearch = false;
bool outputFrequentTrees = false;
bool outputFrequency = false;
bool countUnique = true;
bool closedSearch = false;
bool denseDataset = false;
bool undirectedGraph = false;
string inputFile;
string patternFile;
bool patternSearch = false;
int absoluteSupport = 0;
float relativeSupport = 0.0;
int absoluteMaxSupport = INT_MAX;
float relativeMaxSupport = 0.0;
int maximumGap = -1;
int maxDepth = -1;

/* information on the datasets */
int datasetSize = 0;
int patternsCount = 0;
vector<float> patternFrequencies;

bool closedItemsetSearch = false;
bool closedSequenceSearch = false;
bool rootedTreeSearch = false;
bool sequenceSearch = false;
set<int> useOnlyTheseLabels;
set<int> excludeTheselabels;
int useOnlyThisRoot = -1;

vector<map<int, set<int> > > ignoredNodesList;

/**
 * a flag that indicates if (some) nodes contain set of items.
 */
bool itemSetMining = false;

/**
 * Collects nodes from a tree serialization.
 * @param content a sequence of items representing a tree
 * @param nodes   a sequence of NodeInfo used as internal representation of a tree
 * @param idNode  the id of the first node in content
 * @param depth   the depth of the next node
 * @param sequences a set of nodes labels
 * @return the scope of the last read node
 */
int collectNodes(deque<string>& content, vector<NodeInfo>& nodes, int idNode,
		const int depth) {
	int scope = idNode;
	while (!content.empty()) {
		string node = content.front();
		content.pop_front();
		if (node.compare("-1") == 0)
			return (idNode);
		idNode += 1;
		scope = collectNodes(content, nodes, idNode, depth + 1);
		while ((int) nodes.size() < idNode + 1) {
			nodes.push_back(NodeInfo());
		}
		nodes[idNode] = NodeInfo(idNode, depth);
		deque<string> labels; // to store different labels
		split(node, labels, ":"); // collect different labels in labels;
		set<int>* labelsSet = & nodes[idNode].labels;
		for (deque<string>::const_iterator it = labels.begin(); it != labels.end();
				it++) {
			std::istringstream iss(*it);
			int nodeLabel;
			iss >> nodeLabel;
			labelsSet->insert(nodeLabel);
		}
		if (labelsSet->size() > 1) {
			itemSetMining = true;
		}
		set<int>* ptrChildNodes = &nodes[idNode].childNodes;
		for (int i = idNode+1 ; i <= scope ; i++){
			if (nodes[i].depth == depth+1){
				ptrChildNodes->insert(i);
			}
		}
		idNode = scope;
	}
	return (scope);
}

/**
 * Reads pattern file, collects items and patterns' occurences in the dataset.
 * @param fileName the name of the file that contain the data.
 * @param patternNodes the list of all items in the dataset.
 * @param patternFrequencies the frequency of each pattern
 * @return the number of trees in pattern dataset ; -1 if the process fails
 */
int processPatternFile(const string& fileName,
		vector<vector<NodeInfo> >& patternNodes,
		vector<float>& patternFrequencies) {
	ifstream inFile(fileName.c_str());
	int nbTrees = 0;
	if (inFile.is_open()) {
		string line;
		while (inFile.good()) {
			getline(inFile, line);
			deque<string> content; // to store tokens
			split(line, content, " "); // split line
			if (content.size() < 3)
				continue;
			if (content[content.size() - 2] != "-") {
				cout << "invalid content in pattern file  '" << patternFile
						<< "'" << endl;
				return (-1);
			}
			nbTrees++;
			float frequency;
			std::istringstream iss0(content[content.size() - 1]);
			iss0 >> frequency;
			patternFrequencies.push_back(frequency);
			vector<NodeInfo> nodes;
			content.erase(content.begin() + content.size() - 2, content.end());
			collectNodes(content, nodes, -1, 0);
			patternNodes.push_back(nodes);
		}
		inFile.close();
		return (nbTrees);
	} else {
		return (-1);
	}
}

/**
 * Reads input file, collects items and compute their occurences in the dataset.
 * @param fileName the name of the file that contain the data.
 * @param allNodes the list of all items in the dataset.
 * @param seqCount the number of occurences of each item.
 * @return the number of trees in input dataset ; -1 if the process fails
 */
int processInputFileSleuth(const string& fileName,
		vector<vector<NodeInfo> >& allNodes) {
	ifstream inFile(fileName.c_str());
	int nbTrees = 0;
	if (inFile.is_open()) {
		string line;
		while (inFile.good()) {
			getline(inFile, line);
			deque<string> content; // to store tokens
			split(line, content, " "); // split line
			if (content.size() < 3)
				continue;
			nbTrees++;
			int treeId;
			std::istringstream iss0(content[0]);
			iss0 >> treeId;
			int treeSize;
			std::istringstream iss2(content[2]);
			iss2 >> treeSize;
			vector<NodeInfo> nodes;
			content.erase(content.begin(), content.begin() + 3);
			collectNodes(content, nodes, -1, 0);
			allNodes.push_back(nodes);
		}
		inFile.close();
		return (nbTrees);
	} else {
		return (-1);
	}
}

/**
 * Reads input file, collects items and compute their occurences in the dataset.
 * @param fileName the name of the file that contain the data.
 * @param allNodes the list of all items in the dataset.
 * @return the number of trees in input dataset ; -1 if the process fails
 */
int processInputFileHybrid(const string& fileName,
		vector<vector<NodeInfo> >& allNodes) {
	ifstream inFile(fileName.c_str());
	int nbTrees = 0;
	if (inFile.is_open()) {
		string line;
		getline(inFile, line); // graph id
		while (inFile.good()) {
			getline(inFile, line);
			int nbNodes;
			std::istringstream iss(line);
			iss >> nbNodes;
			vector<NodeInfo> nodes;
			for (int i = 0 ; i < nbNodes ; i++) {
				getline(inFile, line);
				int idNode;
				idNode = i;
				NodeInfo node = NodeInfo(idNode, 1);

				deque<string> labels; // to store different labels
				split(line, labels, ":"); // collect different labels in labels;
				set<int>* labelsSet = & node.labels;
				for (deque<string>::const_iterator it = labels.begin(); it != labels.end();
						it++) {
					std::istringstream iss(*it);
					int nodeLabel;
					iss >> nodeLabel;
					labelsSet->insert(nodeLabel);
				}
				if (labelsSet->size() > 1) {
					itemSetMining = true;
				}
				nodes.push_back(node);
			}
			bool good = true;
			while (good){
				getline(inFile, line);
				deque<string> content; // to store tokens
				split(line, content, " "); // split line
				if (content.size() < 2) {
					allNodes.push_back(nodes);
					nbTrees += 1;
					break;
				}
				int branchId;
				std::istringstream iss_bId(content[0]);
				iss_bId >> branchId;
				int fromNode;
				std::istringstream iss_from(content[1]);
				iss_from >> fromNode;
				int toNode;
				std::istringstream iss_to(content[2]);
				iss_to >> toNode;
				nodes[fromNode-1].childNodes.insert(toNode-1);
				if (undirectedGraph){
					nodes[toNode-1].childNodes.insert(fromNode-1);
				}
			}
		}
		inFile.close();
		return (nbTrees);
	} else {
		return (-1);
	}
}

/**
 * Reads input file, collects items and compute their occurences in the dataset.
 * @param fileName the name of the file that contain the data.
 * @param allNodes the list of all items in the dataset.
 * @return the number of trees in input dataset ; -1 if the process fails
 */
int processInputFile(const string& fileName,
		vector<vector<NodeInfo> >& allNodes) {
	ifstream inFile(fileName.c_str());
	int nbGraph = 0;
	map<int, int> nodeId2pos;
	if (inFile.is_open()) {
		string line;
		int graphNb;
		int nbNode = -1;
		vector<NodeInfo> nodes;
		while (inFile.good()) {
			getline(inFile, line);
			if (line[0] == '#'){
				continue;
			}
			if ((line[0] == 'g') || (line[0] == 't') ||(line[0] == 'X')){
				allNodes.push_back(nodes);
				nodes.clear();
				nodeId2pos.clear();
				deque<string> content; // to store tokens
				nbGraph += 1;
				nbNode = -1;
				continue;
			}
			if ((line[0] == 'n')||(line[0] == 'v')){
				deque<string> content; // to store tokens
				split(line, content, " "); // split line
				int nodeId;
				std::istringstream iss(content[1]);
				iss >> nodeId;
				if (nodeId2pos.count(nodeId) == 0){
					nbNode  += 1;
					nodeId2pos[nodeId] = nbNode;
					NodeInfo node = NodeInfo(nodeId2pos[nodeId], 1);
					nodes.push_back(node);
				}

				deque<string> labels; // to store different labels
				split(content[2], labels, ":"); // collect different labels in labels;
				NodeInfo* node = &nodes[nodeId2pos[nodeId]];
				set<int>* labelsSet = &node->labels;
				for (deque<string>::const_iterator it = labels.begin(); it != labels.end();
						it++) {
					std::istringstream iss(*it);
					int nodeLabel;
					iss >> nodeLabel;
					labelsSet->insert(nodeLabel);
				}
				if (labelsSet->size() > 1) {
					itemSetMining = true;
				}
				continue;
			}
			if ((line[0] == 'd')||(line[0] == 'e')){
				deque<string> content; // to store tokens
				split(line, content, " "); // split line
				int fromNode;
				std::istringstream iss_from(content[1]);
				iss_from >> fromNode;
				int toNode;
				std::istringstream iss_to(content[2]);
				iss_to >> toNode;
				if (nodeId2pos.count(fromNode) == 0){
					nbNode  += 1;
					nodeId2pos[fromNode] = nbNode;
					NodeInfo node = NodeInfo(nodeId2pos[fromNode], 1);
					nodes.push_back(node);
				}
				if (nodeId2pos.count(toNode) == 0){
					nbNode  += 1;
					nodeId2pos[toNode] = nbNode;
					NodeInfo node = NodeInfo(nodeId2pos[toNode], 1);
					nodes.push_back(node);
				}

				nodes[nodeId2pos[fromNode]].childNodes.insert(nodeId2pos[toNode]);
				if (undirectedGraph){
					nodes[nodeId2pos[toNode]].childNodes.insert(nodeId2pos[fromNode]);
				}
			}
		}
		inFile.close();
		allNodes.push_back(nodes);
		nbGraph += 1;
		return (nbGraph);
	} else {
		return (-1);
	}
}
int processInputFile_bak(const string& fileName,
		vector<vector<NodeInfo> >& allNodes) {
	ifstream inFile(fileName.c_str());
	int nbTrees = 0;
	if (inFile.is_open()) {
		string line;
		do {
			getline(inFile, line);
		} while(line[0]=='#');
		while (inFile.good()) {
			getline(inFile, line);
			int nbNodes;
			std::istringstream iss(line);
			iss >> nbNodes;
			vector<NodeInfo> nodes;
			for (int i = 0 ; i < nbNodes ; i++) {
				getline(inFile, line);
				int idNode;
				idNode = i;
				NodeInfo node = NodeInfo(idNode, 1);

				deque<string> labels; // to store different labels
				split(line, labels, ":"); // collect different labels in labels;
				set<int>* labelsSet = & node.labels;
				for (deque<string>::const_iterator it = labels.begin(); it != labels.end();
						it++) {
					std::istringstream iss(*it);
					int nodeLabel;
					iss >> nodeLabel;
					labelsSet->insert(nodeLabel);
				}
				if (labelsSet->size() > 1) {
					itemSetMining = true;
				}
				nodes.push_back(node);
			}
			bool good = true;
			while (good){
				getline(inFile, line);
				deque<string> content; // to store tokens
				split(line, content, " "); // split line
				if (content.size() < 2) {
					allNodes.push_back(nodes);
					nbTrees += 1;
					break;
				}
				int branchId;
				std::istringstream iss_bId(content[0]);
				iss_bId >> branchId;
				int fromNode;
				std::istringstream iss_from(content[1]);
				iss_from >> fromNode;
				int toNode;
				std::istringstream iss_to(content[2]);
				iss_to >> toNode;
				nodes[fromNode-1].childNodes.insert(toNode-1);
			}
		}
		inFile.close();
		return (nbTrees);
	} else {
		return (-1);
	}
}

/**
 * Reads the first line of input file and determine its format
 * @param fileName the name of the file that contain the data.
 * @return the data format
 *     . 1  = Sleuth/treeMiner format
 *     . 2  = HybridTreeMiner format
 *     . 3  = Imit format
 *     . -1 = unknown
 */
int getInputFormat(const string& fileName) {
	int fileFormat = -1;
	ifstream inFile(fileName.c_str());
	if (inFile.is_open() && inFile.good()) {
		string line;
		getline(inFile, line);
		deque<string> content; // to store tokens
		split(line, content, " "); // split line
		if ((content[0] == "g")||(content[0] == "t")||(content[0] == "XP"))
			fileFormat = 3;
		else if (content.size() == 1)
			fileFormat = 2;
		else if (content.size() == 2)
			fileFormat = 3;
		else if (content.size() > 3)
			fileFormat = 1;
	}
	inFile.close();
	return (fileFormat);
}

void collectAllChilds(const vector<NodeInfo>& listNodes, const int first, int depth, set<int>& childs) {
	childs.insert(listNodes[first].childNodes.begin(),listNodes[first].childNodes.end());
	if (depth == 0){
		return;
	}
	for (set<int>::const_iterator it = listNodes[first].childNodes.begin(); it != listNodes[first].childNodes.end(); it++){
		collectAllChilds(listNodes, *it, depth-1, childs);
	}
}

/**
 * Determines if there is an ancestor relationship between two nodes.
 * @param listNodes a sequence of nodes
 * @param first the id of the first node in the sequence
 * @param second the id of the second node in the sequence
 * @return true if first node is an ancestor of second node
 */
bool isAncestor(const vector<NodeInfo>& listNodes, const int first,
		const int second, int maxGap) {
	if (maxGap == 0) {
		if (listNodes[first].childNodes.count(second) > 0) {
			return (true);
		} else {
			return (false);
		}
	} else {
		set<int> allChilds;
		collectAllChilds(listNodes, first, maxGap, allChilds);
		if (allChilds.count(second) > 0) {
			return (true);
		} else {
			return (false);
		}

	}
}

string serialize(const vector<pair<set<int>, int> >& label) {
	string text;
	for (vector<pair<set<int>, int> >::const_iterator it = label.begin();
			it != label.end(); it++) {
		set<int> seq = (*it).first;
		for (set<int>::const_iterator it2 = seq.begin(); it2 != seq.end(); it2++) {
			std::ostringstream out;
			out << *it2;
			text += out.str() + ":";
		}
		std::ostringstream out;
		out << (*it).second;
		text += "/" + out.str();
		text += " ";
	}
	return (text);
}

vector<string> serializeSubTree(const int treeId, const int nodeId, const vector<vector<NodeInfo> >& nodes) {
	vector<string> textSet;
	string text;
	vector<int> orderedLabel;
	orderedLabel.assign(nodes[treeId][nodeId].labels.begin(), nodes[treeId][nodeId].labels.end());
	for (vector<int>::const_iterator it2 = orderedLabel.begin(); it2 != orderedLabel.end(); it2++) {
		std::ostringstream out;
		out << *it2;
		text += out.str() + ":";
	}
	textSet.push_back(text+"(");

	for (set<int>::iterator it = nodes[treeId][nodeId].childNodes.begin() ; it != nodes[treeId][nodeId].childNodes.end(); it++){
		string text;
		vector<int> orderedLabel;
		orderedLabel.assign(nodes[treeId][*it].labels.begin(), nodes[treeId][*it].labels.end());
		for (vector<int>::const_iterator it2 = orderedLabel.begin(); it2 != orderedLabel.end(); it2++) {
			std::ostringstream out;
			out << *it2;
			text += out.str() + ":";
		}
		textSet.push_back(text);
	}
	textSet.push_back(")");
	return (textSet);
}

void writeSolution(const Candidates& candidates,
		const vector<pair<set<int>, int> >& label, const int count) {
	vector<set<int> > labelPath;
	candidates.convertLabel(label, labelPath);
	string labelOfSequence;
	for (int i = 0; i < (int) labelPath.size(); i++) {
		string l;
		for (set<int>::const_iterator lIt = labelPath[i].begin();
				lIt != labelPath[i].end(); lIt++) {
			if (!l.empty()) {
				l += ":";
			}
			std::ostringstream out;
			out << *lIt;
			l += out.str();
		}
		labelOfSequence += l + " ";
	}
	cout << labelOfSequence << "- ";
	if (outputFrequency) {
		float frequency = (float) count / (float) datasetSize;
		cout << frequency << endl;
	} else {
		cout << count << endl;
	}
}

void writeSolutionForSearchPattern(const Candidates& candidates,
		const vector<pair<set<int>, int> >& label, const int count, const int basePattern) {
	vector<set<int> > labelPath;
	candidates.convertLabel(label, labelPath);
	string labelOfSequence;
	for (int i = 0; i < (int) labelPath.size(); i++) {
		string l;
		for (set<int>::const_iterator lIt = labelPath[i].begin();
				lIt != labelPath[i].end(); lIt++) {
			if (!l.empty()) {
				l += ":";
			}
			std::ostringstream out;
			out << *lIt;
			l += out.str();
		}
		labelOfSequence += l + " ";
	}
	cout << labelOfSequence << "- ";
	if (outputFrequency) {
		float frequency = (float) count / (float) datasetSize;
		cout << frequency << " (" << patternFrequencies[basePattern] << ")" << endl;
	} else {
		cout << count << " (" << patternFrequencies[basePattern] << ")" << endl;
	}
}

int getPreviousSibbling(const vector<pair<set<int>, int> >& label,
		const int pos) {
	int previousSibbling = pos - 1;
	while (previousSibbling >= 0) {
		if (label[previousSibbling].second == label[pos].second) {
			return (previousSibbling);
		}
		if (label[previousSibbling].second < label[pos].second) {
			return (-1);
		}
		previousSibbling -= 1;
	}
	return (previousSibbling);
}

int getParent(const vector<pair<set<int>, int> >& label, const int pos) {
	int parentPos = pos - 1;
	while (parentPos > 0) {
		if (label[parentPos].second < label[pos].second) {
			return (parentPos);
		}
		parentPos -= 1;
	}
	return (parentPos);
}

bool myComp(const set<int>& a, const set<int>& b) {
	return (a < b);
	set<int>::const_iterator it1 = a.begin();
	set<int>::const_iterator it2 = b.begin();
	while (it1 != a.end() && it2 != b.end()) {
		if (*it1 == *it2) {
			it1++;
			it2++;
			continue;
		}
		return (*it1 < *it2);
	}
	return (a.size() > b.size());
}

bool myComp2(const pair<set<int>, int>& a, const pair<set<int>, int>& b) {
	if (a.second != b.second) {
		return (a.second < b.second);
	}
	set<int>::const_iterator it1 = a.first.begin();
	set<int>::const_iterator it2 = b.first.begin();
	while (it1 != a.first.end() && it2 != b.first.end()) {
		if (*it1 == *it2) {
			it1++;
			it2++;
			continue;
		}
		return (*it1 < *it2);
	}
	return (a.first.size() > b.first.size());
}

bool myComp3(const set<int>& a, const set<int>& b) {
	set<int>::const_iterator it1 = a.begin();
	set<int>::const_iterator it2 = b.begin();
	while (it1 != a.end() && it2 != b.end()) {
		if (*it1 == *it2) {
			it1++;
			it2++;
			continue;
		}
		return (*it1 < *it2);
	}
	return (a.size() > b.size());
}

/*
 *	Compare two substructures. One starting at pos1, the other at pos2
 *
 *	Return value:
 *	-1 if pos1 substructure is lower than pos2 substructure
 *	0  if both substructures are equal
 *	1  if pos1 substructure is greater than pos2 substructure
 *
 */

int compareSubStructure(const vector<pair<set<int>, int> >& label, const int pos1,
		const int pos2) {
	int len1 = pos2 - pos1;
	int len2 = label.size() - pos2;
	if (len1 > len2) {
		int minLen = min(len1, len2);
		for (int j = 0; j < minLen; j++) {
			if (*(label.begin() + pos1 + j) != *(label.begin() + pos2 + j)) {
				break;
			}
		}
		return (-1); // subtree
	}
	for (int j = 0; j < len1; j++) {
		if (*(label.begin() + pos1 + j) == *(label.begin() + pos2 + j)) {
			continue;
		}
		bool diffDepth = ((label.begin() + pos1 + j)->second
				!= (label.begin() + pos2 + j)->second);
		if (diffDepth != 0)
			return (-1);
		const set<int>* label1 = &(label.begin() + pos1 + j)->first;
		const set<int>* label2 = &(label.begin() + pos2 + j)->first;
			if (label1 < label2)
				return (-1);
			else if (label1 > label2)
				return (1);
	}
	if (len1 > len2)
		return (-1);
	else if (len1 < len2)
		return (1);
	return (0);
}

/*
 *	test if a structures is in canonical form
 *
 *	Return value:
 *	-1 structure is in canonical form
 *	 0 structure is in canonical form but any extension done in the right hand path
 *	   will make it non canonical
 *	 1 structure is not in canonical form
 *
 */

int isCanonical(const vector<pair<set<int>, int> >& lab) {
	vector<pair<set<int>, int> > label = lab;
	for (uint i = 0 ; i < label.size() ; i++){
		if (*label[i].first.begin() < 0){
			int indx = -*label[i].first.begin()-1;
			label[i].first = label[indx].first;
		}
	}
	bool previousSibblingFound = false;
	bool identicalRightHand = true;
	int posLastChild = label.size() - 1;
	while (posLastChild > 0) {
		int posOfPreviousSibbling = getPreviousSibbling(label, posLastChild);
		if (posOfPreviousSibbling != -1) {
			previousSibblingFound = true;
		    int compare = compareSubStructure(label, posOfPreviousSibbling, posLastChild);
		     if (compare == 1)
			     return (1);
		     else if (compare == -1)
			     identicalRightHand = false;
		}
		posLastChild = getParent(label, posLastChild);
	}
	if (identicalRightHand && previousSibblingFound)
		return (0);

	return (-1);
}

int isCycleCanonical(const vector<pair<set<int>, int> >& lab) {
	// test of canonical form in the presence of cycles
		vector<pair<set<int>, int> > label = lab;
		bool normalForm = true;
		for (uint i = 0 ; i < lab.size()-2 ; i++){
			//shift
			pair<set<int>, int> bk1 = label.back();
			label.pop_back();
			pair<set<int>, int> bk2 = label.back();
			label.pop_back();
			bool stopShifting = false;
			for (uint j = 0 ; j < label.size() ; j++){
				if (*label[j].first.begin() < 0){
					int a = *label[j].first.begin();
					if (a < 0){
						label[j].first.clear();
						label[j].first.insert(a - 1);
						label[j].first.insert(999);

					}
				}
				label[j].second += 1;
			}
			if (stopShifting){
				break;
			}
			bk1.second = bk2.second+1;
			bk2.second = 0;
			label.insert(label.begin(),bk2);
			if (*label[0].first.begin() < 0){
				int a = -*label[0].first.begin()-1;
				label[0].first = label[a].first;
				label[a].first.clear();
				label[a].first.insert(-1);
				label[a].first.insert(999);
			}
			label.push_back(bk1);
			bool found_smallest = false;
			for (uint j = 0 ; j < label.size()-1 ; j++){
				if (lab[j].second < label[j].second){
					found_smallest = true;
					break;
				} else if (lab[j].second > label[j].second){
					found_smallest = false;
					break;
				}

				if (lab[j].first > label[j].first){
					found_smallest = true;
					break;
				}
				else if (lab[j].first < label[j].first){
					found_smallest = false;
					break;
				}
			}
			if (found_smallest && isCanonical(label)){
				normalForm = false;
				break;
			}
		}
		if (normalForm){
			return (-1);
		} else {
			return (1);
		}
	}

bool isCanonical2(const vector<pair<set<int>, int> >& label) {
	if (label.size() == 1){
		return (true);
	}
	map<set<int>, int> attributes2int;

	vector<set<int> > attributes;
	for (vector<pair<set<int>, int> >::const_iterator it = label.begin() ; it != label.end() ; it++) {
		if (*it->first.begin() >= 0)
			attributes2int[it->first]=0;
			attributes.push_back(it->first);
	}


	std::sort(attributes.begin(), attributes.end(), myComp3);

	for (int i = 0; i < (int) attributes.size(); i++) {
		attributes2int[attributes[i]] = i;
	}


	bliss::Digraph g = bliss::Digraph(0);
	g.set_splitting_heuristic(bliss::Digraph::shs_fm);


	int depth = -1;
	vector<int> path;
	map<int, int> nodeId2vertex;
	for (int i = 0; i < (int) label.size(); i++) {
		int vertex;
		if (*label[i].first.begin() < 0){
			// an edge to an existing vertex
			vertex = nodeId2vertex[-(*label[i].first.begin())];
		}else{
			vertex = g.add_vertex(attributes2int[label[i].first]);
			nodeId2vertex[i]=vertex;
		}
		if (label[i].second <= depth) {
			for (int j = label[i].second; j <= depth; j++) {
				path.pop_back();
			}
		}

		if (path.size() > 0) {
			g.add_edge(path.back(), vertex);
		}
		path.push_back(vertex);
		depth = label[i].second;
	}
	FILE *fp = fopen("f1", "w");
	g.write_dimacs(fp);
	fclose(fp);

	bliss::Stats stats;
	const unsigned int *cl = g.canonical_form(stats, NULL, NULL);
	cout << "==>cl=" << *cl << endl;
	bliss::Digraph *cf = g.permute(cl);
	FILE *fp2 = fopen("f2", "w");
	cf->write_dimacs(fp2);
	fclose(fp2);
	if (cf->cmp(g) == 0){
		return (true);
	} else{
		return (false);
	}
}


int matchesPattern(Candidates& candidates, CandidateInfo& candidateInfo,
		const vector<pair<set<int>, int> >& label,
		const vector<vector<NodeInfo> >& listNodes) {
	const vector<SequenceInfo>* values = candidates.getSequencesInfo(candidateInfo);

	for (vector<SequenceInfo>::const_iterator sii = values->begin();
			sii != values->end(); sii++) {
		if (sii->getNbTree() < patternsCount) {
			if (listNodes[sii->getNbTree()].size() == label.size()){
				bool found = true;
				for (int i = 0 ; i < (int)label.size() ; i++) {
					if (label[i].first != listNodes[sii->getNbTree()][i].labels){
						found = false;
						break;
					}
				}
				if (found) {
					return (sii->getNbTree());
				}
			}
		}
	}
	return (-1);
}

bool containsPattern(Candidates& candidates,
		const vector<pair<set<int>, int> >& label) {
	const vector<SequenceInfo>* values = &candidates.candidates[label].subtrees;

	for (vector<SequenceInfo>::const_iterator sii = values->begin();
			sii != values->end(); sii++) {
		if (sii->getNbTree() < patternsCount) {
			return (true);
		}
	}
	return (false);
}

int countMotifs(const vector<SequenceInfo>* values) {
	int count = 0;
	if (countUnique) {
		// one occurence per tree
		set<int> differentTrees;
		for (vector<SequenceInfo>::const_iterator sii = values->begin();
				sii != values->end(); sii++) {
			if ((*sii).getNbTree() >= patternsCount) {
				differentTrees.insert((*sii).getNbTree());
			}
		}
		count = (int) differentTrees.size();
	} else {
		count = INT_MAX;
		// method of Njissen
		vector<set<pair<int,int> > > differentNodes (values->at(0).getSize());
		for (vector<SequenceInfo>::const_iterator sii = values->begin();
				sii != values->end(); sii++) {
			if ((*sii).getNbTree() >= patternsCount){
				for (int i = 0 ; i < sii->getSize() ; i++){
					differentNodes[i].insert(pair<int,int>(sii->getNbTree(),sii->getElementAt(i)));
				}
			}
		}
		for (vector<set<pair<int,int> > >::iterator it = differentNodes.begin() ; it != differentNodes.end() ; it++){
			count = min(count, (int)it->size());
		}
	}
	return (count);
}

int countMotifsByLabel(Candidates& candidates,
		const vector<pair<set<int>, int> >& label) {
	return (countMotifs(&candidates.newCandidates[label].subtrees));
}


bool isSubSequence(const vector<pair<set<int>, int> >& lhs,
		const vector<pair<set<int>, int> >& rhs) {
	if (lhs == rhs)
		return (false);
	if (lhs.size() != rhs.size())
		return (false);
	for (int i = 0; i < (int) min(lhs.size(), rhs.size()); i++) {
		if (lhs[i].second != rhs[i].second)
			return (false);
		if (lhs[i].first == rhs[i].first)
			continue;
		if (!includes(rhs[i].first.begin(), rhs[i].first.end(),
				lhs[i].first.begin(), lhs[i].first.end())) {
			return (false);
		}
	}
	return (true);

}

int getNbOccurences(const vector<SequenceInfo>* values) {
	int occur;
	occur = ((int) values->size());
	return (occur);
}

int getNbOccurencesByLabel(Candidates& candidates,
		const vector<pair<set<int>, int> >& label) {
	return (getNbOccurences(&candidates.newCandidates[label].subtrees));
}

pair<int, int> getInheritedCount(Candidates& candidates,
		const vector<pair<set<int>, int> >& label) {
	pair<int, int> count;
	for (map<vector<pair<set<int>, int> >, CandidateInfo >::const_iterator it =
			candidates.candidates.begin()/*upper_bound(label)*/;
			it != candidates.candidates.end(); it++) {
//		vector<pair<set<int>, int> > l2 = (it->first);
		if (isSubSequence(label, it->first)) {
			count.first = max(count.first, it->second.perTreeFrequency);
			count.second = max(count.second, it->second.nbOccurences);
		}
	}
	return (count);
}

/*
 * Testing structure inclusion
 * if a substructure of label is found, then, mark it as nonClosed
 */
void markPreviousNonClosedCandidates(Candidates& candidates,
		const vector<pair<set<int>, int> >& label, int freq) {
	if (label.size() == 1) {
		return;
	}

	for (int i = 0; i < (int) label.size(); i++) {
		vector<pair<set<int>, int> > l = label;
		int depth = (l.begin() + i)->second;
		l.erase(l.begin() + i);
		for (int j = i; j < (int) l.size(); j++) {
			if ((l.begin() + j)->second > depth) {
				(l.begin() + j)->second -= 1;
			} else {
				break;
			}
		}
		for (map<vector<pair<set<int>, int> >, CandidateInfo >::iterator it =
				candidates.candidates.begin()/*upper_bound(label)*/;
				it != candidates.candidates.end(); it++) {
			if ((l == it->first)) { // || isSubSequence(it->first, l)) {

				if (it->second.perTreeFrequency == freq) {
					if (verbose)
						cout << "removing closed " << serialize(it->first)
								<< endl;
					candidates.setClosed(&it->second);
				}
//				count.first = max(count.first, it->second.first);
//				count.second = max(count.second, it->second.second);
			}
		}
	}
	return;
}
/*
 * get structurally inherited count
 * for patterns including label
 */
pair<int, int> getInheritedCountStructural(const Candidates& candidates,
		const vector<pair<set<int>, int> >& label) {
	pair<int, int> count;
	for (map<vector<pair<set<int>, int> >, CandidateInfo >::const_iterator it =
			candidates.candidates.begin()/*upper_bound(label)*/;
			it != candidates.candidates.end(); it++) {
		if (it->first.size() == 1) {
			continue;
		}
		for (int i = 0; i < (int) it->first.size(); i++) {
			vector<pair<set<int>, int> > l = it->first;
			int depth = (l.begin() + i)->second;
			l.erase(l.begin() + i);
			for (int j = i; j < (int) l.size(); j++) {
				if ((l.begin() + j)->second > depth) {
					(l.begin() + j)->second -= 1;
				} else {
					break;
				}
			}
			if ((label == l)) { // || isSubSequence(label, l)) {
				count.first = max(count.first, it->second.perTreeFrequency);
				count.second = max(count.second, it->second.nbOccurences);
			}
		}
	}
	return (count);
}

void filterNewCandidates(Candidates& candidates){
	if (patternSearch) {
		for (map<vector<pair<set<int>, int> >, CandidateInfo >::iterator candIt = candidates.newCandidates.begin(); candIt != candidates.newCandidates.end() ; candIt++){
			bool containsPattern = false;
			for (vector<SequenceInfo>::const_iterator sii = candIt->second.subtrees.begin();
					sii != candIt->second.subtrees.end(); sii++) {
				if (sii->getNbTree() < patternsCount) {
					containsPattern = true;
					break;
				}
			}

			if (!containsPattern) {
				candidates.newCandidates.erase(candIt++);
			} else {
				++candIt;
			}
			continue;
		}

		candidates.storeNewCandidates();
		return;
	}

	for (map<vector<pair<set<int>, int> >, CandidateInfo >::iterator candIt =
			candidates.newCandidates.begin();
			candIt != candidates.newCandidates.end(); ) {
		bool shouldDelete = false;
		if (!orderedSearch && (isCanonical(candIt->first) ==1)) {
			shouldDelete = true;
		}

		if (!shouldDelete){
			int count = countMotifs(&(candIt->second.subtrees));
			if (count < absoluteSupport) {
				shouldDelete = true;
			}
		}

		if (shouldDelete) {
			candidates.newCandidates.erase(candIt++);
		} else {
			++candIt;
		}
	}
	candidates.storeNewCandidates();
}

/**
 * Generates a new list of candidates from a previous one by appending individual nodes.
 * @param candidates a list of candidates
 * @param listNodes a list of nodes
 */
void extendFirstCandidate(Candidates& candidates, const vector<vector<NodeInfo> >& listNodes) {
	pair<vector<pair<set<int>, int> >, CandidateInfo* >* candi = &candidates.currentCandidates.back();
	const vector<pair<set<int>, int> >* label = &candi->first;
	CandidateInfo* candidateInfo = candi->second;
	vector<SequenceInfo>* values = &(candi->second->subtrees);

		if (patternSearch) {
			if (!containsPattern(candidates, *label)) {
				candidates.removeCandidate(false);
				return;
			}
		}

	int count = countMotifs(values);
	int nbOfOccurences = getNbOccurences(values);
	bool iscanonical1 = (isCanonical(*label) == -1);

	if (count < absoluteSupport) {
		candidates.removeCandidate(false);
		return;
	}

	int canonical = isCanonical(*label);
	int canonicalCycle = -1;
	if ((canonical != 1) && *(label->back().first.begin()) == -1){
		// test the canonicity of a pattern with cycle
		canonicalCycle = isCycleCanonical(*label);
	}

	if (!orderedSearch && ((canonical == 1) || (canonicalCycle ==1))) {
		candidates.removeCandidate(false);
		return;
	}

	bool extensionOfLastNodeAllowed = true;
	if (!orderedSearch && (canonical == 0)) {
		extensionOfLastNodeAllowed = false;
		int option = 2;
		if (option ==1){
			//only sibblings can be added to this pattern
			// we can keep only one occurence of path with the same right hand path
			vector<int> rightPathElts;
			int rightPathElt = label->size()-1;
			while (rightPathElt != -1) {
				rightPathElts.insert(rightPathElts.begin(),rightPathElt);
				rightPathElt = getParent(*label, rightPathElt);
			}

			set<pair<int,vector<int> > > pathsToRemove;
			vector<SequenceInfo> newValue;
			for (vector<SequenceInfo>::iterator it = values->begin();
					it != values->end();it++) {
				vector<int> rightPathContent;
				for (uint i = 0 ; i < rightPathElts.size() ; i++){
					rightPathContent.push_back(it->pathList[i]);
				}
				pair<int,vector<int> > elt = pair<int,vector<int> >(it->getNbTree(), rightPathContent);
				if (pathsToRemove.count(elt) > 0){
				} else{
					pathsToRemove.insert(elt);
					newValue.push_back(*it);
				}
			}
			values->clear();
			values->insert(values->end(),newValue.begin(),newValue.end());
		}
		else if (option ==2){
			set<set<int> > pathElts;
			vector<SequenceInfo> newValue;
			for (vector<SequenceInfo>::iterator it = values->begin();
					it != values->end();it++) {
				set<int> p;
				p.insert(it->pathList.begin(),it->pathList.end());

				if (pathElts.count(p) > 0){
				} else{
					pathElts.insert(p);
					newValue.push_back(*it);
				}
			}
			if (verbose) cout << "reduction of instances (automorph)" << values->size() << " to " << newValue.size() << endl;
			values->clear();
			values->insert(values->end(),newValue.begin(),newValue.end());
		}
	}
	bool newOptimisation = true;
	if (newOptimisation){
		if ((label->back().second == 1) && (label->back().first.size() ==1)){
			const int pos = label->size()-1;
			int posOfPreviousSibling = getPreviousSibbling(*label, label->size()-1);
			if ((posOfPreviousSibling != -1) && (*((*label)[posOfPreviousSibling].first.begin()) < *(label->back().first.begin()))){
				set<set<int> > pathElts;
				set<int> lastElt;
				vector<SequenceInfo> newValue;
				for (vector<SequenceInfo>::iterator it = values->begin();
						it != values->end();it++) {
					set<int> p;
					p.insert(it->pathList.begin(),it->pathList.end()-1);

					if ((pathElts.count(p) > 0)&&(lastElt.count(it->pathList.back())>0)){
					} else{
						pathElts.insert(p);
						lastElt.insert(it->pathList.back());
						newValue.push_back(*it);
					}
				}
				if (verbose) cout << "reduction of instances(opt1)" << values->size() << " to " << newValue.size() << endl;
				values->clear();
				values->insert(values->begin(),newValue.begin(),newValue.end());
			}
		}
	}

	bool newOptimisation2 = true;
	if (newOptimisation2){
		vector<int> rightPathElts;
		int rightPathElt = label->size()-1;
		while (rightPathElt != -1) {
			rightPathElts.insert(rightPathElts.begin(),rightPathElt);
			rightPathElt = getParent(*label, rightPathElt);
		}
		for (uint i = 1 ; i < rightPathElts.size()-1 ; i++){
			vector<pair<set<int>, int> > labelFromParent;
			labelFromParent.insert(labelFromParent.begin(), label->begin()+rightPathElts[i], label->end());
			bool good = true;
			for (vector<pair<set<int>, int> >::iterator it = labelFromParent.begin() ; it < labelFromParent.end() ; it++){
				if (*(it->first.begin()) < 0){
					good = false;
					break;
				}
			}
			if (good && isCanonical(labelFromParent) ==0 ){
				set<pair<vector<int>,set<int> > > pathElts;
				vector<SequenceInfo> newValue;
				for (vector<SequenceInfo>::iterator it = values->begin();
						it != values->end();it++) {
					set<int> s;
					s.insert(it->pathList.begin()+rightPathElts[i],it->pathList.end());
					vector<int> v;
					v.insert(v.begin(),it->pathList.begin(),it->pathList.begin()+rightPathElts[i]);
					pair<vector<int>,set<int> > p (v,s);
					if (pathElts.count(p) > 0){
					} else{
						pathElts.insert(p);
						newValue.push_back(*it);
					}
				}
				if (verbose)cout << "reduction of instances(opt2)" << values->size() << " to " << newValue.size() << endl;

				values->clear();
				values->insert(values->end(),newValue.begin(),newValue.end());
				break;

			}

		}
	}
	bool closedCandidate = false;
	if (closedItemsetSearch && itemSetMining) {
		pair<int, int> inheritedCount = getInheritedCount(candidates, *label);

		if (inheritedCount.first > 0
				&& inheritedCount.second == nbOfOccurences) {
			if (!closedCandidate && closedSequenceSearch) {
				candidates.setSuperCount(candidateInfo, count, nbOfOccurences);
			}
			if (closedSequenceSearch) {
				candidates.setClosed(candidateInfo);
			}
			candidates.removeCandidate(closedItemsetSearch || closedSequenceSearch);
			return;
		}
		if (inheritedCount.first >= count) {
			closedCandidate = true;
		}
	}

	if (itemSetMining && extensionOfLastNodeAllowed) {
		map<int, vector<SequenceInfo>* > memoLp;

		set<vector<pair<set<int>, int> > > superSequences;
		for (vector<SequenceInfo>::const_iterator c = values->begin();
				c != values->end(); c++) {
			int n = (*c).getLast();
			const NodeInfo* nodeId = &listNodes[(*c).getNbTree()][n];

			// add an item to the itemset of the last node
			const set<int>& currentLabel = label->back().first;
			if (nodeId->labels.size() > 1) {
				for (set<int>::const_iterator labelIt = nodeId->labels.begin();
						labelIt != nodeId->labels.end(); labelIt++) {
					if ((*labelIt) > (*currentLabel.rbegin())) {
						vector<SequenceInfo>* pvsi;
						if (memoLp.count(*labelIt) > 0) {
							pvsi = memoLp[*labelIt];
						} else {
							vector<pair<set<int>, int> > labelPath = *label;
							(labelPath.rbegin())->first.insert(*labelIt);
							if (closedItemsetSearch) {
								superSequences.insert(labelPath);
							}
							pvsi = &candidates.newCandidates[labelPath].subtrees;
							memoLp[*labelIt] = pvsi;
							if (verbose){
								cout <<"generated pattern : " << serialize(labelPath) << endl;
							}
						}
						vector<int> path = (*c).getPathList();
						SequenceInfo si = SequenceInfo((*c).getNbTree(), path);
						pvsi->push_back(si);
					}
				}
			}
		}

		if (closedItemsetSearch) {
			bool identicalSuperset = false;

			for (set<vector<pair<set<int>, int> > >::const_iterator supIt =
					superSequences.begin(); supIt != superSequences.end();
					supIt++) {

				if (getNbOccurencesByLabel(candidates, *supIt) == nbOfOccurences) {
					identicalSuperset = true;
				}
				if (countMotifsByLabel(candidates, *supIt) == count) {
					closedCandidate = true;

					if (closedSequenceSearch) {
						candidates.setClosed(candidateInfo);
						candidates.setSuperCount(candidateInfo, count, nbOfOccurences);
					}
				}
			}

			// if a candidates, created by adding a new item to the last itemset
			// has the same occurence of the current pattern, then, the extension of
			// the current pattern is not needed
			// in addition, the current pattern is redundant
			if (identicalSuperset) {
				candidates.removeNewCandidate(*label);
				if (closedSequenceSearch) {
					candidates.setClosed(candidateInfo);
					candidates.setSuperCount(candidateInfo, count, nbOfOccurences);
				}

				return;
			}
		}
	}

	if (!closedCandidate && closedSequenceSearch) {
		markPreviousNonClosedCandidates(candidates, *label, count);
		pair<int, int> inheritedCount = getInheritedCountStructural(candidates, *label);

		if (inheritedCount.first >= count) {
			closedCandidate = true;
			if (verbose)
				cout << "closed: " << serialize(*label) << endl;
		}

	}
	if (closedItemsetSearch || closedSequenceSearch) {
		candidates.setSuperCount(candidateInfo, count, nbOfOccurences);
		if (closedCandidate && closedSequenceSearch) {
			candidates.setClosed(candidateInfo);
		}
	}

	if (outputFrequentTrees && !closedCandidate && !closedSequenceSearch) {
		if (!patternSearch) {
			writeSolution(candidates, *label, count);
		} else {
			int basePattern = matchesPattern(candidates, *candidateInfo, *label, listNodes);
			if (basePattern != -1) {
				writeSolutionForSearchPattern(candidates, *label, count, basePattern);
			}
		}
	}

	map<pair<int, int>, vector<SequenceInfo>* > memoLp;
	map<int, set<int> > labelsGeneratingNonCanonicalPatterns;
	map<int, set<int> > labelsGeneratingCanonicalPatterns;

	for (vector<SequenceInfo>::const_iterator c = values->begin();
			c != values->end(); c++) {
		bool canAddDescendant = true;
		int n = (*c).getLast();
		vector<int> pl = (*c).getPathList();
		set<int> eltSet(pl.begin(), pl.end()-1);

		if (eltSet.count(n) != 0){
			canAddDescendant = false;
		}

		set<int> excludedNodes;
		if (undirectedGraph){
			// code to mine undirected graphs
			// find ancestor
			if ((*c).getSize() >= 2){
				int parentPos = (*c).getSize() - 2; // the last node of the path cannot be a parent
				int lastChildPos = (*c).getSize() - 1;

				while (!isAncestor(listNodes[(*c).getNbTree()], (*c).getElementAt(parentPos),
						(*c).getElementAt(lastChildPos),maximumGap)) {
					parentPos -= 1;
				}
				excludedNodes.insert((*c).getElementAt(parentPos)); // to avoid going back to parent
			}
		}

		if (extensionOfLastNodeAllowed && canAddDescendant && ((maxDepth == -1) || ((int)(*c).getSize() < maxDepth))){
			// add a descendant to the last node

			set<int>::iterator start = listNodes[(*c).getNbTree()][n].childNodes.begin();
			set<int>::iterator end = listNodes[(*c).getNbTree()][n].childNodes.end();
			set<int> childNodes;
			if (maximumGap != 0){
				collectAllChilds(listNodes[(*c).getNbTree()], n, maximumGap, childNodes);
				start = childNodes.begin();
				end = childNodes.end();
			}

			for (set<int>::iterator it = start ; it != end; it++){
				if (excludedNodes.count(*it) > 0){
					continue;
				}

				const NodeInfo* nodeIdi = &listNodes[(*c).getNbTree()][*it];
				if (!(nodeIdi->isFrequent())) {
					continue;
				}
				const int depth = label->back().second + 1;

				int reentrantNodeIndex = -1;
				for (uint i = 0 ; i < (*c).getPathList().size() ; i++){
					if ((*c).getPathList()[i] == *it){
						reentrantNodeIndex = i;
						break;
					}
				}
				if (reentrantNodeIndex != -1){
					// try to add an edge to an existing node
					int reentrantCode = -(reentrantNodeIndex+1);
					vector<SequenceInfo>* pvsi;
					if (memoLp.count(pair<int, int>(reentrantCode, depth)) > 0) {
						pvsi = memoLp[pair<int, int>(reentrantCode, depth)];
					} else {
						vector<pair<set<int>, int> > labelPath;
						labelPath = *label;
						set<int> labelSet;
						labelSet.insert(reentrantCode);
						labelSet.insert(999);

						labelPath.push_back(pair<set<int>, int>(labelSet, depth));

						pvsi = &candidates.newCandidates[labelPath].subtrees;
						memoLp[pair<int, int>(reentrantCode, depth)] = pvsi;
						if (verbose){
							cout <<"generated pattern : " << serialize(labelPath) << endl;
						}
					}
					vector<int> path = (*c).getPathList();
					path.push_back(*it);
					SequenceInfo si = SequenceInfo((*c).getNbTree(), path);
					pvsi->push_back(si);
					continue;
				}

				for (set<int>::const_iterator labelIt =
						listNodes[(*c).getNbTree()][*it].labels.begin();
						labelIt != listNodes[(*c).getNbTree()][*it].labels.end();
						labelIt++) {
					if (!orderedSearch && labelsGeneratingNonCanonicalPatterns[-1].count(*labelIt)> 0){
						continue; // there is no need to add this label
					}
					vector<SequenceInfo>* pvsi;
					if (memoLp.count(pair<int, int>(*labelIt, depth)) > 0) {
						pvsi = memoLp[pair<int, int>(*labelIt, depth)];
					} else {
						vector<pair<set<int>, int> > labelPath;
						labelPath = *label;
						set<int> labelSet;
						labelSet.insert(*labelIt);

						labelPath.push_back(pair<set<int>, int>(labelSet, depth));
						if (!orderedSearch){

							if (labelsGeneratingCanonicalPatterns[-1].count(*labelIt) == 0){
								if (isCanonical(labelPath) != 1) {
									labelsGeneratingCanonicalPatterns[-1].insert(*labelIt);
								} else{
									labelsGeneratingNonCanonicalPatterns[-1].insert(*labelIt);
									continue;
								}
							}
						}

						pvsi = &candidates.newCandidates[labelPath].subtrees;
						memoLp[pair<int, int>(*labelIt, depth)] = pvsi;
						if (verbose){
							cout <<"generated pattern : " << serialize(labelPath) << endl;
						}

					}
					vector<int> path = (*c).getPathList();
					path.push_back(*it);
					SequenceInfo si = SequenceInfo((*c).getNbTree(), path);
					pvsi->push_back(si);
				}
			}
		}

		if ((*c).getSize() < 2)
			continue;
		if (sequenceSearch) continue;
		// add sibblings at any level of the path

		int parentPos = (*c).getSize() - 2; // the last node of the path cannot be a parent
		int parentDepth = (*label)[parentPos].second;
		int lastChildPos = (*c).getSize() - 1;
		excludedNodes.clear();
		while (parentPos >= 0) {
			excludedNodes.insert((*c).getElementAt(lastChildPos));

			vector<int> pl = (*c).getPathList();
			set<int> eltSet(pl.begin(), pl.end());
			int n = (*c).getElementAt(parentPos);
			if (eltSet.count(n) != 1){
				continue;
			}


			// TODO not good for embedded DAGs
			if (maximumGap != 0){
				collectAllChilds(listNodes[(*c).getNbTree()], (*c).getElementAt(parentPos+1), maximumGap+1, excludedNodes);
			}else{
				excludedNodes.insert((*c).getElementAt(parentPos+1));
			}
			if ((*label)[parentPos].second > parentDepth){
				excludedNodes.insert((*c).getElementAt(parentPos));
				parentPos -= 1;
				continue;
			}
			if ((*label)[parentPos].second >= (*label)[lastChildPos].second){
				excludedNodes.insert((*c).getElementAt(parentPos));
				parentPos -= 1;
				continue;
			}

			if (undirectedGraph){
				excludedNodes.insert((*c).getElementAt(parentPos));
			}
			int parentNodeId = (*c).getElementAt(parentPos);
			const NodeInfo* parentNode =
					&listNodes[(*c).getNbTree()][parentNodeId];

			set<int>::iterator start = parentNode->childNodes.begin();
			set<int>::iterator end = parentNode->childNodes.end();
			set<int> childNodes;
			if (maximumGap != 0){
				collectAllChilds(listNodes[(*c).getNbTree()], parentNodeId, maximumGap, childNodes);
				start = childNodes.begin();
				end = childNodes.end();
			}

			for (set<int>::iterator it = start ; it != end; it++){
				if (excludedNodes.count(*it) > 0){
					continue;
				}
				if (maximumGap != 0){
					collectAllChilds(listNodes[(*c).getNbTree()], (*c).getElementAt(parentPos+1), maximumGap, excludedNodes);
				}

				const NodeInfo* nodeIdi = &listNodes[(*c).getNbTree()][*it];
				if (!nodeIdi->isFrequent()) {
					continue;
				}

				if (!orderedSearch) {
					// gets list of childs
					bool insertionPossible = true;
					for (int j = parentPos + 1;
							j <= lastChildPos;
							j++) {
						int childId = (*c).getElementAt(j);
						if (((*label)[j]).second == ((*label)[parentPos]).second + 1) {

							if (*it == childId){
								insertionPossible = false;
								break;
							}
							if (maximumGap != 0){
								if (isAncestor(listNodes[(*c).getNbTree()], *it, childId, maximumGap)){//|| isAncestor(listNodes[(*c).getNbTree()], childId, *it, maximumGap)) {
								insertionPossible = false;
								break;
							}}

						}
					}
					if (!insertionPossible) {
						continue;
					}
				}
				const int depth = (*label)[parentPos].second + 1;

				int reentrantNodeIndex = -1;
				for (uint i = 0 ; i < (*c).getPathList().size() ; i++){
					if ((*c).getPathList()[i] == *it){
						reentrantNodeIndex = i;
						break;
					}
				}
				if (reentrantNodeIndex != -1){
					// try to add an edge to an existing node
					int reentrantCode = -(reentrantNodeIndex+1);
					vector<SequenceInfo>* pvsi;
					if (memoLp.count(pair<int, int>(reentrantCode, depth)) > 0) {
						pvsi = memoLp[pair<int, int>(reentrantCode, depth)];
					} else {
						vector<pair<set<int>, int> > labelPath;
						labelPath = *label;
						set<int> labelSet;
						labelSet.insert(reentrantCode);
						labelSet.insert(999);

						labelPath.push_back(
								pair<set<int>, int>(labelSet, depth));

						pvsi = &candidates.newCandidates[labelPath].subtrees;
						memoLp[pair<int, int>(reentrantCode, depth)] = pvsi;
						if (verbose){
							cout <<"generated pattern : " << serialize(labelPath) << endl;
						}
					}

					vector<int> path = (*c).getPathList();
					path.push_back(*it);
					SequenceInfo si = SequenceInfo((*c).getNbTree(), path);
					pvsi->push_back(si);
					continue;
				}

				for (set<int>::const_iterator labelIt =
						listNodes[(*c).getNbTree()][*it].labels.begin();
						labelIt != listNodes[(*c).getNbTree()][*it].labels.end();
						labelIt++) {
					if (!orderedSearch && labelsGeneratingNonCanonicalPatterns[parentPos].count(*labelIt)> 0){
						continue; // there is no need to add this node
					}

					if (!orderedSearch
							&& (*labelIt < *((*label)[lastChildPos].first.begin()))) {
						labelsGeneratingNonCanonicalPatterns[parentPos].insert(*labelIt);
						continue;
					}
					vector<SequenceInfo>* pvsi;
					if (memoLp.count(pair<int, int>(*labelIt, depth)) > 0) {
						pvsi = memoLp[pair<int, int>(*labelIt, depth)];
					} else {
						vector<pair<set<int>, int> > labelPath;
						labelPath = *label;
						set<int> labelSet;
						labelSet.insert(*labelIt);

						labelPath.push_back(
								pair<set<int>, int>(labelSet, depth));
						if (!orderedSearch){
							if (labelsGeneratingCanonicalPatterns[parentPos].count(*labelIt) == 0){
								if (isCanonical(labelPath) != 1) {
									labelsGeneratingCanonicalPatterns[parentPos].insert(*labelIt);
								} else{
									labelsGeneratingNonCanonicalPatterns[parentPos].insert(*labelIt);
									continue;
								}
							}
						}
						pvsi = &candidates.newCandidates[labelPath].subtrees;
						memoLp[pair<int, int>(*labelIt, depth)] = pvsi;
						if (verbose){
							cout <<"generated pattern : " << serialize(labelPath) << endl;
						}
					}

					vector<int> path = (*c).getPathList();
					path.push_back(*it);
					SequenceInfo si = SequenceInfo((*c).getNbTree(), path);
					pvsi->push_back(si);
				}
			}
			lastChildPos = parentPos;
			parentPos -= 1;
			parentDepth = (*label)[parentPos].second;
			excludedNodes.clear();
		}
	}
	candidates.removeCandidate(closedItemsetSearch || closedSequenceSearch);
	return;
}

void performJoin(Candidates& candidates,
		const vector<vector<NodeInfo> >& listNodes) {
	extendFirstCandidate(candidates, listNodes);
	candidates.storeNewCandidates();
}

void initializeCandidates(Candidates& candidates,
		const vector<vector<NodeInfo> >& nodes) {
	int nbreducedCandidates = 0;
	int nbCandidates = 0;
	for (int t = 0; t < (int) nodes.size(); t++) {
		set<string> subtreesPerCandidates;
		set<int> ignoredNodes;
		for (int i = 0; i < (int) nodes[t].size(); i++) {
			if ((maximumGap != 0) && countUnique
					&& (ignoredNodes.count(i) > 0)) {
				continue;
			}
			if (!nodes[t][i].isFrequent()) {
				continue;
			}
			if (rootedTreeSearch && nodes[t][i].depth != 0) {
				continue;
			}

			if ((maximumGap != 0) && countUnique) {
				set<int>::iterator start = nodes[t][i].childNodes.begin();
				set<int>::iterator end = nodes[t][i].childNodes.end();
				set<int> childNodes;
				if (maximumGap != 0){
					collectAllChilds(nodes[t], i, maximumGap, childNodes);
					start = childNodes.begin();
					end = childNodes.end();
				}

					for (set<int>::iterator itj = start ; itj != end; itj++){
					if (includes(nodes[t][i].labels.begin(),
							nodes[t][i].labels.end(),
							nodes[t][*itj].labels.begin(),
							nodes[t][*itj].labels.end())) {
						ignoredNodes.insert(*itj);
					}
				}
			}
			nbCandidates += 1;
			bool oktoadd = true;
			if (countUnique && denseDataset){
				vector<string> subTree = serializeSubTree(t,i, nodes);
				if (!orderedSearch){
					std::sort(subTree.begin(), subTree.end());
				}
				string txt;
				for (vector<string>::const_iterator si = subTree.begin() ; si < subTree.end() ; si++){
					std::ostringstream out;
					out << *si;
					txt += out.str() + " ";
				}
				if (subtreesPerCandidates.count(txt) == 0){
					subtreesPerCandidates.insert(txt);
				} else {
					nbreducedCandidates+=1;
					oktoadd = false;
					ignoredNodes.insert(i);
				}
			}
			if (oktoadd){
				for (set<int>::const_iterator lIt = nodes[t][i].labels.begin();
						lIt != nodes[t][i].labels.end(); lIt++) {
					if (useOnlyThisRoot != -1){
						if (useOnlyThisRoot != *lIt) continue;
					}
					candidates.addNewCandidateNode(t, *lIt, i);
				}
			}
		}
	}
	candidates.storeNewCandidates();
	if (denseDataset){
		cout << "reduced candidates = " << nbreducedCandidates << " out of " << nbCandidates << "\n";
	}
}

void getfrequentNodes(const vector<vector<NodeInfo> >& allNodes,
		set<int>& frequents) {
	int frequentCount = 0;
	if (!countUnique) {
		map<int, int> count;

		for (vector<vector<NodeInfo> >::const_iterator t = allNodes.begin();
				t != allNodes.end(); t++) {
			for (vector<NodeInfo>::const_iterator ni = (*t).begin();
					ni != (*t).end(); ni++) {
				for (set<int>::const_iterator l = ni->labels.begin();
						l != ni->labels.end(); l++) {
					count[*l] += 1;
				}
			}
		}
		for (map<int, int>::const_iterator it = count.begin(); it != count.end();
				it++) {
			if ((it->second >= absoluteSupport) && (it->second <= absoluteMaxSupport)) {
				frequents.insert(it->first);
				frequentCount += 1;
				if (verbose) {
					cout << "freq of " << it->first << "=" << it->second << endl;
				}
			}
		}
	} else {
		map<int, set<int> > count;

		for (int i = 0; i < (int) allNodes.size(); i++) {
			for (vector<NodeInfo>::const_iterator ni = allNodes[i].begin();
					ni != allNodes[i].end(); ni++) {
				for (set<int>::const_iterator l = ni->labels.begin();
						l != ni->labels.end(); l++) {
					count[*l].insert(i);
				}
			}
		}
		for (map<int, set<int> >::const_iterator it = count.begin();
				it != count.end(); it++) {
			if (verbose) {
				cout << "freq of " << it->first << "=" << it->second.size() << endl;
			}

			if (((int) it->second.size() >= absoluteSupport) && ((int) it->second.size() <= absoluteMaxSupport)){
				frequents.insert(it->first);
				frequentCount += 1;
			}
		}
	}
}
void removeUnfrequentNodes(vector<vector<NodeInfo> >& allNodes,
		const set<int>& frequents) {
	for (vector<vector<NodeInfo> >::iterator t = allNodes.begin();
			t != allNodes.end(); t++) {
		for (vector<NodeInfo>::iterator ni = (*t).begin(); ni != (*t).end();
				ni++) {

			for (set<int>::iterator l = ni->labels.begin();
					l != ni->labels.end();) {
				std::set<int>::iterator current = l++;
				if (frequents.count(*current) == 0){
					ni->labels.erase(current);
				}
			}
		}
	}
}

/**
 * initializePatternNodes collects patterns and their occurences
 * @param fileName the name of the file that contains the data.
 * @param patternNodes the list of all items in the pattern dataset.
 * @return 1 if the processing is OK ; -1 if the process fails
 */
int initializePatternNodes(const string& fileName,
		vector<vector<NodeInfo> >& patternNodes,
		vector<float>& patternFrequencies) {
	int inputFormat = getInputFormat(inputFile);
	if (inputFormat != 1) {
		cout << "the format in file '" << inputFile << "' is unknown (only treeMiner format is allowed)" << endl;
		return (-1);
	}

	int nbTrees = processPatternFile(fileName, patternNodes,
			patternFrequencies);
	if (nbTrees == -1) {
		cout << "problem while opening file '" << inputFile << "'" << endl;
		return (-1);
	}

	if (verbose) {
		cout << "size of patternSet : " << nbTrees << endl;
		int nodeCount = 0;
		for (vector<vector<NodeInfo> >::const_iterator it = patternNodes.begin();
				it != patternNodes.end(); it++) {
			nodeCount += it->size();
		}
		cout << "total number of nodes in patternSet: " << nodeCount << endl;
	}

	return (0);
}

/**
 * initializeInputNodes collects items and compute their occurences in the dataset.
 * @param fileName the name of the file that contain the data.
 * @param allNodes the list of all items in the input dataset.
 * @return 1 if the processing is OK ; -1 if the process fails
 */
int initializeInputNodes(const string& fileName,
		vector<vector<NodeInfo> >& inputNodes) {
	int inputFormat = getInputFormat(inputFile);
	if (verbose) {
		cout << "detected input format = ";
		switch (inputFormat) {
		case 1 : cout << "sleuth/treeMiner"; break;
		case 2 : cout << "HybridTreeMiner"; break;
		case 3 : cout << "Imit"; break;
		}
		cout << endl;
	}
	int nbTrees = -1;
	if (inputFormat == 1) {
		nbTrees = processInputFileSleuth(inputFile, inputNodes);
	} else if (inputFormat == 2) {
		nbTrees = processInputFileHybrid(inputFile, inputNodes);
	} else if (inputFormat == 3) {
		nbTrees = processInputFile(inputFile, inputNodes);
	} else {
		return (-1);
	}
	if (nbTrees == -1) {
		cout << "problem while opening file '" << inputFile << "'" << endl;
		return (-1);
	}
	datasetSize = inputNodes.size();
	if (nbTrees != datasetSize) {
		cout << "problem reading file '" << inputFile << "'" << endl;
		return (-1);
	}
	if (verbose) {
		cout << "size of dataset : " << datasetSize << endl;
		int nodeCount = 0;
		for (vector<vector<NodeInfo> >::const_iterator it = inputNodes.begin();
				it != inputNodes.end(); it++) {
			nodeCount += it->size();
		}
		cout << "total number of nodes : " << nodeCount << endl;
	}

	if (relativeSupport > 0) {
		absoluteSupport = inputNodes.size() * relativeSupport;
		if (verbose) {
			cout << "absolute support is set to " << absoluteSupport << endl;
		}
	}

	if (relativeMaxSupport > 0) {
		absoluteMaxSupport = inputNodes.size() * relativeMaxSupport;
		if (verbose) {
			cout << "absolute max support is set to " << absoluteMaxSupport << endl;
		}
	}

	/*
	 * Removing unfrequent nodes
	 */
	set<int> frequentNodes;
	getfrequentNodes(inputNodes, frequentNodes);
	if (!useOnlyTheseLabels.empty()){
		frequentNodes = useOnlyTheseLabels;
	}
	if (!excludeTheselabels.empty()){
		for (set<int>::const_iterator it = excludeTheselabels.begin();
						it != excludeTheselabels.end(); it++) {
			frequentNodes.erase(*it);
		}
	}
	removeUnfrequentNodes(inputNodes, frequentNodes);
	if (verbose) {
		string outMessage = "";
		cout << "nb of frequent items : " << frequentNodes.size() << endl;
		cout << "list of frequent items :";
		for (set<int>::const_iterator it = frequentNodes.begin();
						it != frequentNodes.end(); it++) {
			cout << " " << *it;
		}
		cout << endl;
	}
	return (0);
}

/**
 * Prints usage on the standard output.
 */
void usage() {
	cout << "usage: "
			<< "aadage [-[cdefmOoqruvy]] [-p <patternFile>] [-N <requiredLabels>] [-E <excludedLabels>] [-R <rootLabel>] [-s <minSupport>] [-x <maxSupport>] [-D <maxDepth>] [-g <maxGap>]  -i <inputFile>"
			<< endl;
	cout << "       -c       Display only closed patterns." << endl;
	cout << "                    The closure concerns the calculation of"
			<< endl;
	cout << "                    topological closure and itemset closure." << endl;
	cout
			<< "                    To use only topological/itemset closure, use option -d or -e. "
			<< endl;
	cout << "       -d       Display only patterns with closed itemsets."
			<< endl;
	cout << "       -e       Display only closed tree patterns." << endl;
	cout << "       -f       Output frequency after each frequent tree." << endl;
	cout
			<< "                    If this option is not specified the output contains"
			<< endl;
	cout << "                    the number of each frequent pattern ."
			<< endl;
	cout << "       -m       count all occurences of a pattern in a graph." << endl;
	cout << "                    If this option is not specified, a transactional setting is assumed." << endl;
//	cout << "       -O       ordered search (unordered search by default)"
//			<< endl;
	cout << "       -o       Print frequent patterns to standard output." << endl;
//	cout << "       -q       search for sequences" << endl;
//	cout << "       -r       Search for patterns starting from the root of the trees" << endl;
//	cout << "                    This option is only valid for tree or DAG mining in a transactional setting." << endl;
//	cout << "       -u       Specifies that the graph is undirected." << endl;
	cout << "       -v       Verbose mode." << endl;
//	cout << "       -y       special processing for dense datasets" << endl;
//	cout << "       -p       specifies the name of a pattern file" << endl;
//	cout << "       -N       Specifies the labels that must be included in the patterns." << endl;
//	cout << "       -E       Specifies the labels that must be excluded from the patterns." << endl;
//	cout << "       -R       Specifies the labels that must be at the root of each pattern." << endl;

	cout
			<< "       -s       specifies an absolute min support, or a relative one if value < 1 (default is 1)."
			<< endl;
//	cout
//			<< "       -x       Specifies absolute max support (or relative one if value < 1)."
//			<< endl;
	cout
			<< "       -D       Specify the maxDepth of the result patterns."
			<< endl;
	cout
			<< "       -g       Specifies the maximum allowed gap (default is 0)."
			<< endl;


	cout		<< "       -i       Specifies the name of the input file (mandatory)."
			<< endl;

}

/**
 * Parses command line parameters.
 */
bool parseParameters(int argc, char **argv) {
	int index;
	int c;
	bool parametersOK = true;
	while ((c = getopt(argc, argv, "cdefmOoqruvyp:N:E:R:s:x:D:g:i:")) != -1) {
		switch (c) {
		case 'c':
			closedSearch = true;
			break;
		case 'd':
			closedItemsetSearch = true;
			break;
		case 'e':
			closedSequenceSearch = true;
			break;
		case 'f':
			outputFrequency = true;
			break;
		case 'm':
			countUnique = false;
			break;
		case 'O':
			orderedSearch = true;
			break;
		case 'o':
			outputFrequentTrees = true;
			break;
		case 'q':
			sequenceSearch = true;
			break;
		case 'r':
			rootedTreeSearch = true;
			break;
		case 'u':
			undirectedGraph = true;
			break;
		case 'v':
			verbose = true;
			break;
		case 'y':
			denseDataset = true;
			break;
		case 'p':
			patternFile = optarg;
			patternSearch = true;
			break;
		case 'N': {
			deque<string> labels;
			split(optarg, labels, ":"); // collect different labels in labels;
			for (deque<string>::const_iterator it = labels.begin(); it != labels.end();
				it++) {
				std::istringstream iss(*it);
				int nodeLabel;
				iss >> nodeLabel;
				useOnlyTheseLabels.insert(nodeLabel);
			}
		}
			break;
		case 'E': {
			deque<string> labels;
			split(optarg, labels, ":"); // collect different labels in labels;
			for (deque<string>::const_iterator it = labels.begin(); it != labels.end();
				it++) {
				std::istringstream iss(*it);
				int nodeLabel;
				iss >> nodeLabel;
				excludeTheselabels.insert(nodeLabel);
			}
		}
			break;
		case 'R': {
			std::istringstream iss(optarg);
			iss >> useOnlyThisRoot;
		}
			break;
		case 's': {
			std::istringstream iss(optarg);
			iss >> relativeSupport;
			if (relativeSupport >= 1) {
				absoluteSupport = relativeSupport;
				relativeSupport = 0;
			}
		}
			break;
		case 'x': {
			std::istringstream iss(optarg);
			iss >> relativeMaxSupport;
			if (relativeMaxSupport >= 1) {
				absoluteMaxSupport = relativeMaxSupport;
				relativeMaxSupport = 0;
			}
		}
			break;
		case 'D': {
			std::istringstream iss(optarg);
			iss >> maxDepth;
		}
			break;
		case 'g': {
			std::istringstream iss(optarg);
			iss >> maximumGap;
		}
			break;
		case 'i':
			inputFile = optarg;
			break;
		case '?':
			parametersOK = false;
			break;
		}
	}
	for (index = optind; index < argc; index++) {
		cerr << "Non-option argument '" << argv[index] << "'." << endl;
		parametersOK = false;
	}
	if (closedSearch) {
		closedItemsetSearch = true;
		closedSequenceSearch = true;
	}

	if (patternSearch > 0) {
		if (absoluteSupport != 0) {
			absoluteSupport = 0;
			cerr << "support set to '0' when using a pattern file" << endl;
		}
		if (closedItemsetSearch) {
			closedItemsetSearch = false;
			cerr << "ClosedSearch sets to false when using a pattern file" << endl;
		}
	}

	if (parametersOK) {
		if (verbose) {
			cout << "ordered=" << orderedSearch << ", unique count="
					<< countUnique << ", out=" << outputFrequentTrees
					<< ", infile=" << inputFile << ", support="
					<< (relativeSupport > 0 ? relativeSupport : absoluteSupport)
					<< ", max support="
					<< (relativeMaxSupport > 0 ? relativeMaxSupport : absoluteMaxSupport)
					<< endl;
		}
		return (true);
	} else {
		usage();
		return (false);
	}
}

double memUsed(){
    int tSize = 0, resident = 0, share = 0;
    ifstream buffer("/proc/self/statm");
    buffer >> tSize >> resident >> share;
    buffer.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    double rss = resident * page_size_kb;
    return (rss);
}

/**
 * main function.
 */
int main(int argc, char **argv) {
	if (!parseParameters(argc, argv)) {
		return (-1);
	}
	/*
	 * Read input file and initialize inputNodes
	 */
	vector<vector<NodeInfo> > inputNodes;
	if (initializeInputNodes(inputFile, inputNodes) == -1) {
		cout << "problem reading input dataset  '" << inputFile << "'" << endl;
		return (-1);
	}




	/*
	 * If a pattern file is specified
	 * Read pattern file and initialize patternNodes and patternOccurences
	 */
	vector<vector<NodeInfo> > patternNodes;
	if (patternSearch) {
		if (initializePatternNodes(patternFile, patternNodes,
				patternFrequencies) == -1) {
			cout << "problem reading pattern dataset  '" << patternFile << "'"
					<< endl;
			return (-1);
		}
		patternsCount = patternNodes.size();
	}

	if (patternSearch) {
		// add patternNodes at the beginning of inputNodes
		inputNodes.insert(inputNodes.begin(), patternNodes.begin(),
				patternNodes.end());
		// Now, the entire dataset is separated in two part
		// part one, the patterns (treeId < patternsCount)
		// part two, the attributed trees to mine (treeId >= patternsCount)
	}

	// creates first list of candidates. One for each node
	Candidates candidates;
	initializeCandidates(candidates, inputNodes);

	bool theEnd = candidates.currentCandidates.empty();
	while (!theEnd) {
		if (verbose){
			cout << "nb candidates : " << candidates.candidates.size() << "(memUsed=" << memUsed() << ")" << endl;
		}
		performJoin(candidates, inputNodes);
		theEnd = candidates.currentCandidates.empty();
	}
	if (closedSequenceSearch) {
		for (map<vector<pair<set<int>, int> >, CandidateInfo >::const_iterator it =
				candidates.candidates.begin(); it != candidates.candidates.end();
				it++) {
			if (!it->second.isClosed
					&& (it->second.perTreeFrequency >= absoluteSupport)) {
				writeSolution(candidates, it->first, it->second.perTreeFrequency);
			}
		}
	}
	return (0);
}
