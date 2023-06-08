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
// A class that hold several parameters used to identify a node in a subtree. |
// Each instance of this class identifies a node with three variables:        |
//  - a node label,                                                           |
//  - the depth of the node in the tree structure,                            |
//  - an index that corresponds to the position of the node                   |
//    in the corresponding SequenceInfo data.                                 |
//----------------------------------------------------------------------------+

#include "NodeInfo.h"

NodeInfo::NodeInfo() {
	this->labels = set<int>();
	this->childNodes = set<int>();
	this->depth = -1;
}

NodeInfo::NodeInfo(int id, int depth) {
	this->labels = set<int>();
	this->childNodes = set<int>();
	this->depth = depth;
}

NodeInfo::~NodeInfo() {
}
