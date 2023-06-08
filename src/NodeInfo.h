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

#ifndef NODEINFO_H_
#define NODEINFO_H_

#include <set>

using namespace std;

class NodeInfo {
public:
	/**
	 * Empty constructor.
	 */
	NodeInfo();
	/**
	 * Constructor
	 * @param id the id of the node (not used)
	 * @param depth the depth of the node
	 */
	NodeInfo(int, int);

	/**
	 * Destructor.
	 */
	virtual ~NodeInfo();

	/**
	 * A flag that indicates if the node is frequent
	 * @return true if the node is frequent
	 */
	bool isFrequent() const {return (labels.size() > 0);};
//private:
	std::set<int> labels;
	std::set<int> childNodes;
	int depth;
};

#endif /* NODEINFO_H_ */
