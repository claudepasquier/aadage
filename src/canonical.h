//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
// Canonical form of directed attributed graphs (paper, Section 4 & 5).       |
// A pattern is encoded as a sequence of triples (Q, d, p) where Q is the     |
// itemset associated with a node, d its depth in the spanning tree and p an  |
// optional back-reference (stored as a negative label) to an existing node.  |
//----------------------------------------------------------------------------

#ifndef CANONICAL_H_
#define CANONICAL_H_

#include <set>
#include <string>
#include <vector>

/**
 * Human readable representation of a pattern (used by verbose output).
 */
std::string serialize(const std::vector<std::pair<std::set<int>, int> >& label);

/**
 * Returns the position of the previous sibling of the node at 'pos'
 * (a node at the same depth appearing earlier in the sequence),
 * or -1 if no such node exists.
 */
int getPreviousSibbling(const std::vector<std::pair<std::set<int>, int> >& label,
		const int pos);

/**
 * Returns the position of the parent of the node at 'pos'
 * (the closest preceding node with a strictly smaller depth).
 */
int getParent(const std::vector<std::pair<std::set<int>, int> >& label,
		const int pos);

/**
 * Total order on itemsets (paper, Section 4.1):
 * shorter prefix itemsets come first.
 */
bool myComp3(const std::set<int>& a, const std::set<int>& b);

/**
 * Order on node codes (paper, Section 4.2): depth first, then itemset.
 */
bool myComp2(const std::pair<std::set<int>, int>& a,
		const std::pair<std::set<int>, int>& b);

/**
 * Compare the substructures rooted at positions pos1 and pos2.
 * @return -1 / 0 / 1 (less / equal / greater).
 */
int compareSubStructure(const std::vector<std::pair<std::set<int>, int> >& label,
		const int pos1, const int pos2);

/**
 * Test whether a structure is in canonical form (paper, Section 5.1).
 * @return -1 canonical
 *          0 canonical, but any extension on the right hand path
 *            would break canonicity
 *          1 not canonical
 */
int isCanonical(const std::vector<std::pair<std::set<int>, int> >& lab);

/**
 * Test of canonical form in the presence of cycles (paper, Section 5.3).
 * @return -1 if canonical, 1 otherwise.
 */
int isCycleCanonical(const std::vector<std::pair<std::set<int>, int> >& lab);

#endif /* CANONICAL_H_ */
