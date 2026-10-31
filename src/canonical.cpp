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
// Implementation of the canonical-form utilities.                            |
// The functions below are extracted verbatim from aadage.cpp; the algorithm  |
// is unchanged (see docs/ARCHITECTURE.md for the paper↔code mapping).        |
//----------------------------------------------------------------------------

#include "canonical.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;

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
		if (diffDepth != 0) {
			return (-1);
		}
		const set<int>* label1 = &(label.begin() + pos1 + j)->first;
		const set<int>* label2 = &(label.begin() + pos2 + j)->first;
			// TODO : verify this
			if (label1 < label2){
				return (-1);
			}
			else if (label1 > label2) {
				return (1);
			}
	}
	if (len1 > len2) {
		return (-1);
	}
	else if (len1 < len2) {
		return (1);
	}
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
	for (unsigned int i = 0 ; i < label.size() ; i++){
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
		     if (compare == 1) {
			     return (1);
			}
		     else if (compare == -1)
			     identicalRightHand = false;
		}
		posLastChild = getParent(label, posLastChild);
	}
	if (identicalRightHand && previousSibblingFound) {
		return (0);
	}
	return (-1);
}

int isCycleCanonical(const vector<pair<set<int>, int> >& lab) {
	// test of canonical form in the presence of cycles
		vector<pair<set<int>, int> > label = lab;
		bool normalForm = true;
		for (unsigned int i = 0 ; i < lab.size()-2 ; i++){
			//shift
			pair<set<int>, int> bk1 = label.back();
			label.pop_back();
			pair<set<int>, int> bk2 = label.back();
			label.pop_back();
			bool stopShifting = false;
			(void)stopShifting;
			for (unsigned int j = 0 ; j < label.size() ; j++){
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
			for (unsigned int j = 0 ; j < label.size()-1 ; j++){
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
