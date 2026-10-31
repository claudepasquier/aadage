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
//   Unit tests for the canonical-form module (src/canonical.{h,cpp}).        |
//   Test cases labelled "paper Fig. N" come from the manuscript              |
//   "Attributed graph mining in the presence of automorphism".               |
//                                                                            |
//   NOTE: expectations pin the CURRENT shipped behaviour so that any        |
//   refactor that changes mining results is detected. Where the code        |
//   diverges from the paper's ideal ordering, a comment explains why.       |
//----------------------------------------------------------------------------

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "../src/canonical.h"

static int failures = 0;
static int checks = 0;

#define CHECK_EQ(actual, expected, msg) do {                                   \
	++checks;                                                                  \
	if ((actual) != (expected)) {                                              \
		++failures;                                                            \
		std::cout << "FAIL " << msg << ": expected " << (expected)             \
		          << ", got " << (actual) << std::endl;                        \
	}                                                                          \
} while (0)

typedef std::vector<std::pair<std::set<int>, int> > Pattern;

// single-item node helper
static std::set<int> S(int item) {
	std::set<int> s;
	s.insert(item);
	return s;
}

// ---------------------------------------------------------------------------
// Paper Fig. 2: four isomorphic patterns rooted in 'a' with children 'cd'
// (label 3 stands for the itemset {c,d}). The paper states the code order
// code(P4) < code(P3) < code(P2) < code(P1) and that P4 is the canonical form.
// ---------------------------------------------------------------------------

// P1 = (0,a,0)(1,cd,0)(2,cd,0)(2,cd,0)(1,cd,3)(1,cd,0)(2,cd,0)
static Pattern fig2_P1() {
	Pattern p;
	p.push_back(std::make_pair(S(1), 0));
	p.push_back(std::make_pair(S(3), 1));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(3), 2));
	// reentrant link to node 3: encoded as negative label -(3)-1 = -4
	p.push_back(std::make_pair(S(-4), 1));
	p.push_back(std::make_pair(S(3), 1));
	p.push_back(std::make_pair(S(3), 2));
	return p;
}

// P2 = (0,a,0)(1,cd,0)(2,cd,0)(2,cd,0)(1,cd,2)(1,cd,0)(2,cd,0)
static Pattern fig2_P2() {
	Pattern p;
	p.push_back(std::make_pair(S(1), 0));
	p.push_back(std::make_pair(S(3), 1));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(-3), 1)); // back-reference to node 2
	p.push_back(std::make_pair(S(3), 1));
	p.push_back(std::make_pair(S(3), 2));
	return p;
}

// P3 = (0,a,0)(1,cd,0)(2,cd,0)(2,cd,0)(1,cd,0)(2,cd,0)(1,cd,3)
static Pattern fig2_P3() {
	Pattern p;
	p.push_back(std::make_pair(S(1), 0));
	p.push_back(std::make_pair(S(3), 1));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(3), 1));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(-4), 1));
	return p;
}

// P4 = (0,a,0)(1,cd,0)(2,cd,0)(2,cd,0)(1,cd,0)(2,cd,0)(1,cd,2)
static Pattern fig2_P4() {
	Pattern p;
	p.push_back(std::make_pair(S(1), 0));
	p.push_back(std::make_pair(S(3), 1));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(3), 1));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(-3), 1));
	return p;
}

static void testPaperFig2() {
	Pattern p1 = fig2_P1(), p2 = fig2_P2(), p3 = fig2_P3(), p4 = fig2_P4();
	// shipped behaviour: P1 and P2 are rejected, P3 and P4 accepted.
	// (per the paper's ordering, P2 is non-canonical and P1 is canonical-but-
	// saturated; the pointer comparison inside compareSubStructure, marked
	// "TODO : verify this" in canonical.cpp, explains the divergence)
	CHECK_EQ(isCanonical(p1), 1, "paper Fig2 P1 canonical status");
	CHECK_EQ(isCanonical(p2), 1, "paper Fig2 P2 canonical status");
	CHECK_EQ(isCanonical(p3), -1, "paper Fig2 P3 canonical status");
	CHECK_EQ(isCanonical(p4), -1, "paper Fig2 P4 canonical status");
}

// ---------------------------------------------------------------------------
// Structural helpers
// ---------------------------------------------------------------------------
static void testTreeNavigation() {
	// pattern: root(0) - child A(1) - grandchild(2) - child B(1)
	Pattern p;
	p.push_back(std::make_pair(S(1), 0));
	p.push_back(std::make_pair(S(2), 1));
	p.push_back(std::make_pair(S(3), 2));
	p.push_back(std::make_pair(S(4), 1));

	CHECK_EQ(getParent(p, 0), -1, "parent of root is -1 (no parent)");
	CHECK_EQ(getParent(p, 1), 0, "parent of node 1 is node 0");
	CHECK_EQ(getParent(p, 2), 1, "parent of node 2 is node 1");
	CHECK_EQ(getParent(p, 3), 0, "parent of node 3 is node 0");

	CHECK_EQ(getPreviousSibbling(p, 1), -1, "no previous sibling for node 1");
	CHECK_EQ(getPreviousSibbling(p, 3), 1, "previous sibling of node 3 is node 1");
	CHECK_EQ(getPreviousSibbling(p, 2), -1, "no previous sibling for node 2");
}

static void testCanonicalSimple() {
	// a simple path a-b-c is canonical
	Pattern path;
	path.push_back(std::make_pair(S(1), 0));
	path.push_back(std::make_pair(S(2), 1));
	path.push_back(std::make_pair(S(3), 2));
	CHECK_EQ(isCanonical(path), -1, "linear chain is canonical");

	// root with children labelled 3 then 2: the paper would reject this
	// ({2} should come first), but the historical pointer comparison in
	// compareSubStructure makes the current implementation accept it.
	// Pinned here so that a future fix is a conscious, reviewed decision.
	Pattern bad;
	bad.push_back(std::make_pair(S(1), 0));
	bad.push_back(std::make_pair(S(3), 1));
	bad.push_back(std::make_pair(S(2), 1));
	CHECK_EQ(isCanonical(bad), -1,
	         "descending siblings accepted (pointer-comparison quirk)");

	// the sorted version is canonical too
	Pattern good;
	good.push_back(std::make_pair(S(1), 0));
	good.push_back(std::make_pair(S(2), 1));
	good.push_back(std::make_pair(S(3), 1));
	CHECK_EQ(isCanonical(good), -1, "ascending sibling labels are canonical");

	// automorphic pattern: root with 2 identical children is canonical but
	// saturated (any right-path extension breaks canonicity) -> code 0
	Pattern automorphic;
	automorphic.push_back(std::make_pair(S(1), 0));
	automorphic.push_back(std::make_pair(S(2), 1));
	automorphic.push_back(std::make_pair(S(2), 1));
	CHECK_EQ(isCanonical(automorphic), 0, "automorphic pattern returns 0");
}

static void testItemsetOrdering() {
	// paper Section 4.1: {1,2} < {1,3} < {2} < {3}
	std::set<int> i12, i13, i2, i3;
	i12.insert(1); i12.insert(2);
	i13.insert(1); i13.insert(3);
	i2.insert(2);
	i3.insert(3);
	CHECK_EQ(myComp3(i12, i13), true,  "{1,2} < {1,3}");
	CHECK_EQ(myComp3(i13, i12), false, "{1,3} > {1,2}");
	CHECK_EQ(myComp3(i13, i2), true,   "{1,3} < {2} (prefix rule)");
	CHECK_EQ(myComp3(i2, i13), false,  "{2} > {1,3} (prefix rule)");
	CHECK_EQ(myComp3(i2, i3), true,    "{2} < {3}");
}

static void testSerialize() {
	Pattern p;
	p.push_back(std::make_pair(S(1), 0));
	std::set<int> two;
	two.insert(2); two.insert(3);
	p.push_back(std::make_pair(two, 1));
	CHECK_EQ(serialize(p) == std::string("1:/0 2:3:/1 "), true,
	         "serialize format");
}

int main() {
	testPaperFig2();
	testTreeNavigation();
	testCanonicalSimple();
	testItemsetOrdering();
	testSerialize();
	std::cout << (failures == 0 ? "OK" : "FAILED") << ": "
	          << (checks - failures) << "/" << checks << " checks passed"
	          << std::endl;
	return (failures == 0 ? 0 : 1);
}
