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

#ifndef CANDIDATES_H_
#define CANDIDATES_H_

#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "CandidateInfo.h"
#include "NodeInfo.h"
#include <iostream>

using namespace std;

class Candidates {
public:
	Candidates();
	virtual ~Candidates();
	void addNewCandidateNode(int treeId, int label, int nodeId);
	void addNewCandidate(vector<pair<set<int>, int> >& labelPath, SequenceInfo& si);
	void removeCandidate(bool keepCandidateInfo);
	void removeNewCandidate(const vector<pair<set<int>, int> >& labelPath);
	void storeNewCandidates();
	void setSuperCount(CandidateInfo* candidateInfo,
			int count, int nbOcc);
	void setClosed(CandidateInfo* candidateInfo);
	bool removeUnfrequent(map<vector<set<int> >, int>& seqCount,
			int sequencesSize, bool verbose, int support, bool countUnique);
	void convertLabel(const vector<pair<set<int>, int> >& lp,
			vector<set<int> >& labelPath) const;

	const vector<SequenceInfo>* getSequencesInfo(const CandidateInfo& candidateInfo) ;


	class compare {
	public:
		static bool myComp(const pair<set<int>, int>& e1,
				const pair<set<int>, int>& e2) {
			if (e1.second != e2.second)
				return (e1.second < e2.second);
			set<int>::const_iterator i1 = e1.first.begin();
			set<int>::const_iterator i2 = e2.first.begin();
			while (i1 != e1.first.end() && i2 != e2.first.end()) {
				if (*i1 != *i2) {
					return (*i1 < *i2);
				}
				i1++;
				i2++;
			}
			if (e1.first.size() > e2.first.size())
				return (true);
			if (e1.first.size() < e2.first.size())
				return (false);
			return (e1.first < e2.first);
		}

		bool operator()(const vector<pair<set<int>, int> >& lhs,
				const vector<pair<set<int>, int> >& rhs) const {
			return (lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
					rhs.end(), myComp));
		}
	};

	vector<pair<vector<pair<set<int>, int> >, CandidateInfo* > > currentCandidates;

	map<vector<pair<set<int>, int> >, CandidateInfo > candidates;
	map<vector<pair<set<int>, int> >, CandidateInfo, compare > newCandidates;

};

#endif /* CANDIDATES_H_ */
