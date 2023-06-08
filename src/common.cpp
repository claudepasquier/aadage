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
// Definition of common methods.                                              |
//----------------------------------------------------------------------------+
#include "common.h"

void split(const string& str, deque<string>& tokens, const string& delimiters,
		int fromPos) {
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, if fromPos == 0 add it to the vector.
		if (fromPos == 0) {
			tokens.push_back(str.substr(lastPos, pos - lastPos));
		} else {
			fromPos--;
		}
		// Skip delimiters.
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void join(vector<string>& tokens, string& result,
		const string& delimiter = " ") {
	for (vector<string>::iterator is = tokens.begin(); is != tokens.end();
			is++) {
		if (result.size() > 0)
			result += delimiter;
		result += *is;
	}
}

void join(const vector<int>& tokens, string& result, const string& delimiter =
		" ") {
	std::ostringstream oss;
	bool firstToken = true;
	for (int i = 0; i < (int) tokens.size(); i++) {
		if (firstToken) {
			oss << tokens[i];
			firstToken = false;
		} else {
			oss << delimiter << tokens[i];
		}
	}
	result = oss.str();
}

