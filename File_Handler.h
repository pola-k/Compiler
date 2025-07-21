#pragma once
#include<vector>
#include<unordered_map>
#include<string.h>
#include<string>
#include<fstream>
#include "Common.h"
using namespace std;

extern void write_Tokens(vector<vector<Token>> tokens);
extern void write_Table(unordered_map<string, int> table, string filename);
extern void write_Errors(vector<string> errors);
extern void writeTree(ofstream& writeFile, TreeNode* node, string prefix = "", bool isLast = true);
extern void writeSyntaxTree(TreeNode* node);
extern void emit(const string& code);