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
// A class storing a subtree.                                                 |
//----------------------------------------------------------------------------+

#include "SequenceInfo.h"

SequenceInfo::SequenceInfo(int nbTree, vector<int>& pathList) {
	this->nbTree = nbTree;
	this->nbOfIdenticalSubTree =1;
	this->pathList = pathList;
}

bool SequenceInfo::operator==(const SequenceInfo &si) const {
	return ((nbTree == si.nbTree) && (pathList == si.pathList));
}

bool SequenceInfo::operator<(const SequenceInfo &si) const {
	if (nbTree < si.nbTree)
		return (true);
	if (nbTree > si.nbTree)
		return (false);
	if (pathList < si.pathList)
		return (true);
	return (false);
}


SequenceInfo::~SequenceInfo() {
}

