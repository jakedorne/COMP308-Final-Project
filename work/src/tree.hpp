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
	string treeString;
	int currentDepth, lineWidth;
	float angle;
	float length;
	std::clock_t lastTime;
	double elapsedTime, lastElapsedTime;
	float incr;
	const int TREEDEPTH = 6;
	float MAX_TREE_LENGTH = 0.8;
	bool windy;
	

	Tree();

	void turnRight();
	void turnLeft();
	void pitchDown();
	void pitchUp();
	void rollLeft();
	void rollRight();

	void drawLine();
	void push();
	void pop();
	void leaf();
	void rotR();
	void rotL();

	void thicken(float param);
	void narrow(float param);
	void setThickness(float param);
	void setReduction(float param);

	void expandTree(float num);

	void drawTree();
	void animate();
    
};
