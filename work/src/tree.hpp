#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <chrono>

#include "cgra_math.hpp"
#include "opengl.hpp"

using namespace std;
using namespace cgra;


class Tree {
private:

public:
	vector<string> *trees = new vector<string>();
	vector<string> *expandedTrees = new vector<string>();
	string treeString;
	int currentDepth, lineWidth;
	float angle;
	float length;
	std::clock_t lastTime;
	double elapsedTime, lastElapsedTime;
	float incr;
	const int TREEDEPTH = 6;
	float MAX_TREE_LENGTH = 0.4;
	bool windy;

	Tree();

	void drawLine(int dCount);
	void push();
	void pop();
	void leaf();
	void rotR();
	void rotL();
	void rotF();
	void rotB();

	void expandTree(float num);
	void compressTree(string treeS);

	void drawTree();
	void animate();
	void setWindy(bool change);
    
};
