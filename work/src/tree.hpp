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
	vector<vector<string>> *LSystemRules = new vector<vector<string>>();

	string treeString;
	int currentDepth, lineWidth;
	float angle;
	float length;
	std::clock_t lastTime;
	double elapsedTime, lastElapsedTime;
	float incr;
	const int TREEDEPTH = 5;
	float MAX_TREE_LENGTH = 0.07;
	bool windy, grow;
	float cylWidth;
	float posX;
	float posZ;
	int ruleNo;
	float initAngle;
    // textures
    bool startedTexture;
    GLuint bark_texture;
    GLuint leaf_texture;


	Tree(float x, float z, int ruleNo);
    void initTextures();
	void drawLine(int dCount);
	void drawCyl(int dCount);
	void push();
	void pop();
	void leaf();
	void rotR();
	void rotL();

	void expandTree(float num);
	void compressTree(string treeS);

	void drawTree(bool dim);
	void animate();
	void applyWind(float wind);

	void setAngle(float a);
    
};
