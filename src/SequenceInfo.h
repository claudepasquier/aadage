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

#ifndef SEQUENCEINFO_H_
#define SEQUENCEINFO_H_

#include <string>
#include <vector>
#include <set>

using namespace std;

class SequenceInfo {

private:
	int nbTree;
	int nbOfIdenticalSubTree;
public:
	vector<int> pathList;

public:
	/**
	 * Constructor
	 * @param nbTree the index of the tree in which the current subtree is found
	 * @param pathList the index of nodes included in the subtree
	 */
	SequenceInfo(int, vector<int>&);

	/**
	 * Destructor.
	 */
	virtual ~SequenceInfo();


	/**
	 * Equal operator.
	 * @param an instance of SequenceInfo
	 * @return true if the two objects contain the same information.
	 */
	bool operator==(const SequenceInfo &) const;

	/**
	 * Redefinition of the inferior operator.
	 * @param si a SequenceInfo
	 * @return true if the current instance is strictly lower than the given parameter
	 */
	bool operator<(const SequenceInfo &) const;

	/**
	 * Returns nbTree
	 */
	const int getNbTree() const {return (nbTree);};

	/**
	 * Returns pathList
	 */
	const vector<int> getPathList() const {return (pathList);};

	/**
	 * Returns size of pathList
	 */
	const int getSize() const {return (pathList.size());};
	/**
	 * Returns last element of pathList
	 */
	const int getLast() const {return (pathList.back());};
	/**
	 * Returns nth element of pathList
	 */
	const int getElementAt(int n) const {return (pathList[n]);};
};
#endif /* SEQUENCEINFO_H_ */
