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
// Class storing a list of candidates.                                        |
//----------------------------------------------------------------------------+

#include <fstream>
#include <deque>
#include <fstream>

#include "Candidates.h"
#include "common.h"
#include <algorithm>
Candidates::Candidates() {
}

Candidates::~Candidates() {
}

void Candidates::addNewCandidateNode(int treeId, int label, int nodeId
		) {
	vector<pair<set<int>, int> > labelList;
	set<int> labelSet;
	labelSet.insert(label);
	labelList.push_back(pair<set<int>, int>(labelSet, 0));
	vector<int> pathList;
	pathList.push_back(nodeId);
	vector<set<int> > labels;
	labels.push_back(labelSet);
	SequenceInfo si = SequenceInfo(treeId, pathList);
	newCandidates[labelList].subtrees.push_back(si);
}
void Candidates::addNewCandidate(vector<pair<set<int>, int> >& labelPath,
		SequenceInfo& si) {
	newCandidates[labelPath].subtrees.push_back(si);
}

void Candidates::storeNewCandidates() {
	for (map<vector<pair<set<int>, int> >, CandidateInfo >::reverse_iterator candIt = newCandidates.rbegin();
			candIt != newCandidates.rend();
			){
		pair<map<vector<pair<set<int>, int> >, CandidateInfo >::iterator, bool> ret;
		ret = candidates.insert(*candIt);
		newCandidates.erase(--(candIt.base()));
		currentCandidates.push_back(pair<vector<pair<set<int>, int> >, CandidateInfo* >(ret.first->first, &ret.first->second));
	}
	newCandidates.clear();

}

void Candidates::removeCandidate(bool keepCandidateInfo) {
	vector<pair<vector<pair<set<int>, int> >, CandidateInfo* > >::reverse_iterator ptrCand =  currentCandidates.rbegin();
	if (keepCandidateInfo){
		ptrCand->second->subtrees.clear();
	}else{
		candidates.erase(ptrCand->first);
	}
	currentCandidates.pop_back();
}
void Candidates::removeNewCandidate(const vector<pair<set<int>, int> >& labelPath) {
	newCandidates.erase(labelPath);
}

void Candidates::setSuperCount(CandidateInfo* candidateInfo,
		int count, int nbOcc) {
	candidateInfo->perTreeFrequency = count;
	candidateInfo->nbOccurences = nbOcc;
}

void Candidates::setClosed(CandidateInfo* candidateInfo) {
	candidateInfo->isClosed = true;
}
bool Candidates::removeUnfrequent(map<vector<set<int> >, int>& seqCount,
		int sequencesSize, bool verbose, int absoluteSupport,
		bool countUnique) {
	int nbFrequent = seqCount.size();
	bool theEnd = true;
	/*
	 * removing unfrequent trees
	 */
	for (map<vector<pair<set<int>, int> >, CandidateInfo >::iterator it =
			candidates.begin(); it != candidates.end();) {
		vector<SequenceInfo> values = it->second.subtrees;
		int count = 0;
		if (countUnique) {
			set<int> differentTrees;
			for (vector<SequenceInfo>::iterator sii = values.begin();
					sii != values.end(); sii++) {
				differentTrees.insert((*sii).getNbTree());
			}
			count = (int) differentTrees.size();
		} else {
			count = (int) values.size();
		}
		if (count >= absoluteSupport) {
			vector<set<int> > labelPath;
			convertLabel(it->first, labelPath);

			seqCount.insert(pair<vector<set<int> >, int>(labelPath, count));
			theEnd = false;
			++it;
		} else {
			candidates.erase(it);
		}
	}
	if (verbose) {
		cout << "nb of trees of size " << sequencesSize << ": "
				<< (seqCount.size() - nbFrequent) << endl;
	}

	return (theEnd);
}

/**
 * Converts the internal representation of node to an output representation
 * @param lp the sequence of nodes identifiers
 * @param labelPath the output representation of nodes
 */
void Candidates::convertLabel(const vector<pair<set<int>, int> >& lp,
		vector<set<int> >& labelPath) const {
	int depth = -1;
	for (int i = 0; i < (int) lp.size(); i++) {
		if (lp[i].second <= depth) {
			for (int j = lp[i].second; j <= depth; j++) {
				set<int> up;
				up.insert(-1);
				labelPath.push_back(up);
			}
		}
		labelPath.push_back(lp[i].first);
		depth = lp[i].second;
	}
}

const vector<SequenceInfo>* Candidates::getSequencesInfo(
		const CandidateInfo& candidateInfo) {
	return (&candidateInfo.subtrees);
}

