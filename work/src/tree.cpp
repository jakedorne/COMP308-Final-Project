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
	windy = false;
	vector<string> rule1 = { "D[LXV]D[RXV]LX", "D[RXV]D[LXV]RX" };
	vector<string> rule2 = { "D[LXV]D[RXV]DX", "D[RXV]D[LXV]DX" };
	vector<string> rule3 = { "DL[[X]RX]RD[RDX]LX", "DR[[X]LX]LD[LDX]RX" };
	LSystemRules->push_back(rule1);
	LSystemRules->push_back(rule2);
	LSystemRules->push_back(rule3);
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
				i += LSystemRules->at(ruleSet)[0].size()-1;
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
			if(!isdigit(LSystem.at(i+1))){
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

void Tree::drawLine(int dCount) {
	//cout << "Draw" << endl;
	/*
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, length*dCount, 0);
	glEnd();
	glTranslatef(0, length*dCount, 0);
	*/
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	//cgraCylinder((float)lineWidth/50, (float)lineWidth / 50, length*dCount); 
	cgraCylinder(0.06, 0.03, length*dCount);
	glPopMatrix();
	glTranslatef(0, length*dCount, 0);
}

void Tree::push() {
	//cout << "Push" << endl;
	glPushMatrix();
	if (lineWidth > 0) {
		lineWidth -= 1;
	}
}

void Tree::pop() {
	//cout << "Pop" << endl;
	glPopMatrix();
	lineWidth += 1;
}

void Tree::rotR() {
	//cout << "rotR" << endl;
	glRotatef(-angle, 1, 0, 0);
	glRotatef(angle * 4, 0, 1, 0);
	glRotatef(-angle, 0, 0, 1);
}

void Tree::rotL() {
	//cout << "rotL" << endl;
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
	//cout << "leaf" << endl;
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

	float numR = (float)rand() / RAND_MAX;


	//Make branches move in the wind/rain
	if (windy) {
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
	
	
	//Grow the tree until it reaches max tree height
	if (currentDepth < TREEDEPTH && length <= MAX_TREE_LENGTH) {
		length += 0.0005;
	}

	//cout << elapsedTime << " : " << lastElapsedTime << endl;
	if (elapsedTime - lastElapsedTime > 2 && currentDepth < TREEDEPTH) {
		currentDepth++;
		lastElapsedTime = elapsedTime;

	}
}

void Tree::setWindy(bool change) {
	windy = change;
}

void Tree::setAngle(float a) {
	angle += a;
}
