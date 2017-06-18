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

Tree::Tree(float x, float z, int rule)
{
	treeString = "X";
	currentDepth = 0;
	length = 0.000001;
	lineWidth = 10;
	cylWidth = 0.3;
	lastTime = 0;
	elapsedTime = 0;
	lastElapsedTime = 0;
	angle = 20;
	incr = 0.1;
	grow = true;
    startedTexture = false;
	vector<string> rule1 = { "D[LXV]D[RXV]LX", "D[RXV]D[LXV]RX" }; //n=7, ang=20
	vector<string> rule2 = { "D[LXV]D[RXV]DX", "D[RXV]D[LXV]DX" }; //n=7, ang=25.7
	vector<string> rule4 = { "DL[[X]RX]RD[RDX]LX", "DR[[X]LX]LD[LDX]RX" }; //n=5, ang=22.5
	vector<string> rule3 = { "D[LD]D[RD]D", "D[RD]D[LD]D" }; //n=5, ang=25.7
	LSystemRules->push_back(rule1);
	LSystemRules->push_back(rule2);
	LSystemRules->push_back(rule3);
	LSystemRules->push_back(rule4);
//    initTextures();
	posX = x;
	posZ = z;
	ruleNo = rule;
}

void Tree::initTextures(){
    glGenTextures(1, &bark_texture);
    glActiveTexture(GL_TEXTURE0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    Image tex("./work/res/textures/bark.png");
    glBindTexture(GL_TEXTURE_2D, bark_texture); // Bind it as a 2D texture
    
    // Setup sampling strategies
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Finnaly, actually fill the data into our texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex.w, tex.h, tex.glFormat(), GL_UNSIGNED_BYTE, tex.dataPointer());
    
    glGenTextures(1, &leaf_texture);
    glActiveTexture(GL_TEXTURE0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    Image tex2("./work/res/textures/leaf.png");
    glBindTexture(GL_TEXTURE_2D, leaf_texture); // Bind it as a 2D texture
    
    // Setup sampling strategies
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Finnaly, actually fill the data into our texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex2.w, tex2.h, tex2.glFormat(), GL_UNSIGNED_BYTE, tex2.dataPointer());
}

void Tree::expandTree(float num) {
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
					treeString.replace(i, 1, LSystemRules->at(ruleNo)[0]);

				}
				else {
					treeString.replace(i, 1, LSystemRules->at(ruleNo)[1]);
				}
				
				//ASSUMES BOTH RULES ARE OF THE SAME LENGTH --------------------IMPORTANT @@@@@@@@
				i += LSystemRules->at(ruleNo)[0].size()-1;
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

void Tree::drawTree(bool dim) {
	glPushMatrix();
	glTranslatef(posX, 0, posZ);

    if(!startedTexture) {
        initTextures();
        startedTexture = true;
    }
	string cl; //Current letter
	string LSystem = trees->at(currentDepth); //change to currentDepth
	for (int i = 0; i < LSystem.length(); i++) {
		cl = LSystem.at(i);
		if (!cl.compare("D")) {
			if (!isdigit(LSystem.at(i + 1))) {
				if (dim) {
				drawCyl(1);
				}
				else {
					drawLine(1);
				}
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

				if (dim) {
					drawCyl(dCount);
				}
				else {
					drawLine(dCount);
				}
			}
		}
		else if (!cl.compare("X")) {
			if (dim) {
				drawCyl(1);
			}
			else {
				drawLine(1);
			}

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
		else {
			cout << "drawTree shouldn't reach here" << endl;
		}
	}

	glPopMatrix();
}

void Tree::drawLine(int dCount) {
//	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bark_texture);
	glLineWidth(lineWidth);
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, dCount*length, 0);
	glEnd();

	glTranslatef(0, dCount*length, 0);
	//glDisable(GL_TEXTURE_2D);
}

void Tree::drawCyl(int dCount) {
	cout << lineWidth << endl;
	if (lineWidth > 0) {
//		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bark_texture);

		glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		cgraCylinder((float)lineWidth / 50, (float)lineWidth / 50, length * dCount);
		glPopMatrix();
		glTranslatef(0, length * dCount, 0);
	}
	else {
		cout << "NIGGA" << endl;
	}
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

void Tree::leaf() {
//    glEnable(GL_TEXTURE_2D);
//    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, leaf_texture);
    
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_BLEND);
//    
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.5, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.5, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.5, 1.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.5, 0.0, 0.0);
    glEnd();
    
//    glDisable(GL_BLEND);
    //glDisable(GL_TEXTURE_2D);
    
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

		//cout << elapsedTime << " : " << lastElapsedTime << endl;
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
