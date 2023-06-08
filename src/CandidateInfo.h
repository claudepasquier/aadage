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

#ifndef CANDIDATEINFO_H_
#define CANDIDATEINFO_H_

#include <vector>
#include "SequenceInfo.h"

using namespace std;

class CandidateInfo {
	friend class Candidates;

public:
	vector<SequenceInfo> subtrees;
	int perTreeFrequency;
	int nbOccurences;
	bool isClosed;
public:
	CandidateInfo();
	virtual ~CandidateInfo();
};

#endif /* CANDIDATEINFO_H_ */
