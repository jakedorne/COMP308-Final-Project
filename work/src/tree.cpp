#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>
#include <ctime>



#include "cgra_math.hpp"
#include "geometry.hpp"
#include "opengl.hpp"
#include "simple_image.hpp"
#include "tree.hpp"
#include "cgra_geometry.hpp"

using namespace std;
using namespace cgra;


Tree::Tree()
{
	treeString = "X";
	currentDepth = 0;
	length = 0.01;
	lineWidth = 5;
	lastTime = 0;
	elapsedTime = 0;
	lastElapsedTime = 0;
	angle = 20;
	incr = 0.1;
	grow = true;
	vector<string> rule1 = { "D[LXV]D[RXV]LX", "D[RXV]D[LXV]RX" }; //n=7, ang=20
	vector<string> rule2 = { "D[LXV]D[RXV]DX", "D[RXV]D[LXV]DX" }; //n=7, ang=25.7
	vector<string> rule4 = { "DL[[X]RX]RD[RDX]LX", "DR[[X]LX]LD[LDX]RX" }; //n=5, ang=22.5
	vector<string> rule3 = { "D[LD]D[RD]D", "D[RD]D[LD]D" }; //n=5, ang=25.7
	vector<string> rule5 = { "D[LD]D[RD][D]", "D[RD]D[LD][D]" }; //n=5, ang=20
	vector<string> rule6 = { "DDR[RDLDLD]L[LDRDRD]", "DDL[LDRDRD]R[RDLDLD]" }; //n=4, ang=22.5
	LSystemRules->push_back(rule1);
	LSystemRules->push_back(rule2);
	LSystemRules->push_back(rule3);
	LSystemRules->push_back(rule4);
	LSystemRules->push_back(rule5);
	LSystemRules->push_back(rule6);
}

void Tree::expandTree(float num, int ruleSet) {
	string ns = "";	//New string

	for (int i = 0; i < treeString.length(); i++) {
		ns = treeString.at(i);
		if (!ns.compare("D")) {
			treeString.replace(i, 1, "DD");
			i = i + 1;
		}
		else if (!ns.compare("X")) {
			{
				if (num < 0.4) {
					treeString.replace(i, 1, LSystemRules->at(ruleSet)[0]);

				}
				else {
					treeString.replace(i, 1, LSystemRules->at(ruleSet)[1]);
				}

//ASSUMES BOTH RULES ARE OF THE SAME LENGTH --------------------IMPORTANT @@@@@@@@
i += LSystemRules->at(ruleSet)[0].size() - 1;
			}

		}
	}
	trees->push_back(treeString);
}

void Tree::compressTree(string treeS) {
	string ns = ""; //New string

	int dCount;

	for (int i = 0; i < treeS.length();) {
		if (treeS.at(i) == 'D') {
			dCount = 0;
			if (treeS.at(i + 1) != 'D') {
				ns = ns + treeS.at(i);
				i++;
			}
			else {
				while (treeS.at(i + dCount) == 'D') {
					dCount++;
				}
				i += dCount;
				ns = ns + 'D' + to_string(dCount);
			}
		}
		else {
			ns = ns + treeS.at(i);
			i++;
		}
	}

	expandedTrees->push_back(ns);
}

void Tree::drawTree() {
	string cl; //Current letter
	string LSystem = trees->at(currentDepth); //change to currentDepth
	for (int i = 0; i < LSystem.length(); i++) {
		cl = LSystem.at(i);
		if (!cl.compare("D")) {
			if (!isdigit(LSystem.at(i + 1))) {
				drawLine(1);
			}
			else {
				int digitCount = 0;
				while (isdigit(LSystem.at(i + 1 + digitCount))) {
					digitCount++;
				}
				string str = LSystem.substr(i + 1, digitCount);

				int dCount = stoi(str);

				while (LSystem.at(i + dCount) == 'D') {
					dCount++;
				}
				i += digitCount;

				drawLine(dCount);
			}
		}
		else if (!cl.compare("X")) {
			drawLine(1);
		}
		else if (!cl.compare("[")) {
			push();
		}
		else if (!cl.compare("]")) {
			pop();
		}
		else if (!cl.compare("V")) {
			leaf();
		}
		else if (!cl.compare("R")) {
			rotR();
		}
		else if (!cl.compare("L")) {
			rotL();
		}
		else if (!cl.compare("F")) {
			rotF();
		}
		else if (!cl.compare("B")) {
			rotB();
		}
		else {
			cout << "drawTree shouldn't reach here" << endl;
		}
	}

}

void Tree::drawTree(string ts) {
	vector<vector<string>> *partitions = new vector<vector<string>>();
	if (ts.find('[') != string::npos) {
		for (int i = 0; i < ts.size();) {
			int fCount = 0; //Front count
			string subStr = ts.substr(i, ts.size() - i);
			if (subStr.find('[') != string::npos) {
				while (ts.at(i + fCount) != '[') {
					fCount++;
				}
				//At first [ char
				int bCount = 1; //Bracket count
				int eCount = 0; //Extra count
				while (bCount > 0) {
					if (ts.at(fCount + 1 + eCount + i) == '[') {
						bCount++;
					}
					else if (ts.at(fCount + 1 + eCount + i) == ']') {
						bCount--;
					}
					eCount++;
				}
				vector<string> vec = { ts.substr(i,fCount), ts.substr(i + fCount + 1, eCount - 1) };
				partitions->push_back(vec);
				i++;
				i += eCount;
				i += fCount;
			}
			else {
				vector<string> vec = { "", subStr };
				partitions->push_back(vec);
				i += subStr.size();
			}
		}

		/*COMPLETED MAKING PARTITION LIST

			TODO
			- RECURSE OVER LIST
			- ONCE LIST FINISHED FIGURE OUT WHAT TO DRAW FOR SELF
			- DRAW SELF

		*/
		string fullCylinder = "";
		for (int i = 0; i < partitions->size(); i++) {
			push();
			string cl; //current letter
			for (int j = 0; j < partitions->at(i)[0].size(); j++) {
				cl = partitions->at(i)[0].at(j);
				if (!cl.compare("D")) {
					translate(1);
				}
				else if (!cl.compare("X")) {
					drawLine(1);
				}
				else if (!cl.compare("[")) {
					push();
				}
				else if (!cl.compare("]")) {
					pop();
				}
				else if (!cl.compare("V")) {
					//leaf();
				}
				else if (!cl.compare("R")) {
					rotR();
				}
				else if (!cl.compare("L")) {
					rotL();
				}
			}
			drawTree(partitions->at(i)[1]);
			pop();
			fullCylinder = fullCylinder + partitions->at(i)[0];

			/*
				RECURSION DONE NOW JUST DRAW COLLECTION OF ALL HEADER STRINGS
			*/

			drawTreeHeader(fullCylinder);
			
		}

	}
	else {
		string cl; //Current letter
		for (int i = 0; i < ts.size(); i++) {
		cl = ts.at(i);
		if (!cl.compare("D")) {
			if (!isdigit(ts.at(i + 1))) {
				drawLine(1);
			}
			else {
				int digitCount = 0;
				while (isdigit(ts.at(i + 1 + digitCount))) {
					digitCount++;
				}
				string str = ts.substr(i + 1, digitCount);

				int dCount = stoi(str);

				while (ts.at(i + dCount) == 'D') {
					dCount++;
				}
				i += digitCount;

				drawLine(dCount);
			}
		}
		else if (!cl.compare("X")) {
			drawLine(1);
		}
		else if (!cl.compare("[")) {
			push();
		}
		else if (!cl.compare("]")) {
			pop();
		}
		else if (!cl.compare("V")) {
			leaf();
		}
		else if (!cl.compare("R")) {
			rotR();
		}
		else if (!cl.compare("L")) {
			rotL();
		}
		else if (!cl.compare("F")) {
			rotF();
		}
		else if (!cl.compare("B")) {
			rotB();
		}
		else {
			cout << "drawTree shouldn't reach here" << endl;
		}
	}
	}
	/*for (int i = 0; i < partitions->size(); i++) {
		cout << "Partitions: " << partitions->at(i)[0] << " : " << partitions->at(i)[1] << endl;
	}*/
}

void Tree::drawTreeHeader(string str) {
	cout << str << endl;
	if (str.size() == 0) return;
	int dCount = 0, splitIndex = 0;
	for (int i = 0; i < str.size(); i++) {
		if (str.at(i) == 'D') {
			dCount++;
		}
		else {
			splitIndex = i;
			break;
		}
	}
	if (dCount > 0) {
		//drawLine(dCount);
	}
	else {
		//SWITCH ON OTHER THINGS
	}
	//drawTreeHeader(str.substr(splitIndex, str.size() - splitIndex));
}
void Tree::translate(int dCount) {
	glTranslatef(0, length*dCount, 0);
}

void Tree::drawLine(int dCount) {
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	//cgraCylinder((float)lineWidth/50, (float)lineWidth / 50, length*dCount); 
	cgraCylinder(0.06, 0.03, length*dCount);
	glPopMatrix();
	glTranslatef(0, length*dCount, 0);
}

void Tree::push() {
	glPushMatrix();
	if (lineWidth > 0) {
		lineWidth -= 1;
	}
}

void Tree::pop() {
	glPopMatrix();
	lineWidth += 1;
}

void Tree::rotR() {
	glRotatef(-angle, 1, 0, 0);
	glRotatef(angle * 4, 0, 1, 0);
	glRotatef(-angle, 0, 0, 1);
}

void Tree::rotL() {
	glRotatef(angle, 1, 0, 0);
	glRotatef(angle * 4, 0, 1, 0);
	glRotatef(angle, 0, 0, 1);
}

void Tree::rotF() {
	glRotatef(angle, 1, 0, 0);
	glRotatef(angle * 4, 0, 1, 0);
	glRotatef(angle, 0, 0, 1);
}

void Tree::rotB() {

}

void Tree::leaf() {
	glBegin(GL_TRIANGLES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.2, 0, 0.3);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(-0.2, 0, -0.3);
	glVertex3f(0, 1, 0);
	glEnd();
}

void Tree::animate() {
	if (lastTime == 0) {
		lastTime = clock();
	}

	elapsedTime = (clock() - lastTime) / (double)CLOCKS_PER_SEC;

	if (grow) {
		//Grow the tree until it reaches max tree height
		if (currentDepth < TREEDEPTH && length <= MAX_TREE_LENGTH) {
			length += 0.0005;
		}

		if (elapsedTime - lastElapsedTime > 2 && currentDepth < TREEDEPTH) {
			currentDepth++;
			lastElapsedTime = elapsedTime;

		}
	}
	else {
		if (currentDepth > 0 && length >= 0.1) {
			length -= 0.0005;
		}
		if (elapsedTime - lastElapsedTime > 2 && currentDepth > 0) {
			currentDepth--;
			lastElapsedTime = elapsedTime;

		}
	}
}

void Tree::applyWind() {
	//Make branches move in the wind/rain
	float numR = (float)rand() / RAND_MAX;
		if (angle > 21.5) {
			if (numR < 0.5) {
				incr = -0.15;
			}
			else {
				incr = -0.1;
			}
		}
		else if (angle < 18.5) {
			if (numR > 0.5) {
				incr = 0.15;
			}
			else {
				incr = 0.1;
			}
		}
		angle += incr;
}

void Tree::setAngle(float a) {
	angle += a;
}

bool Tree::checkSame(string str) {
	for (int i = 0; i < str.size(); i++) {
		if (!str.at(0) == str.at(i)) {
			return false;
		}
	}
	return true;
}
