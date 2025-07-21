#pragma once
#include "Common.h"
#include<vector>
using namespace std;

extern void display_Tokens(vector<vector<Token>> tokens);
extern void printTree(TreeNode* node, string prefix = "", bool isLast = true);
extern void printProgram(string filename);
extern void printTAC(string filename);