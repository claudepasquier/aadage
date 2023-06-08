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

#include <vector>
#include <string>
#include <deque>

#include <sstream>

#ifndef COMMON_H_
#define COMMON_H_

using namespace std;

void split(const string&, deque<string>&, const string& delimiters = " ",
		int mode = 0);
void join(vector<string>&, string&, const string&);
void join(const vector<int>&, string&, const string&);

#endif /* COMMON_H_ */
