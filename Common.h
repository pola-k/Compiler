#pragma once
#include<string>
#include<string.h>
#include<vector>
using namespace std;

struct ExprAttrs 
{
    string place;  
    string code; 
    bool isLvalue; 
};

struct Token
{
    int position;
    string type;
    string lexeme;
};

struct TreeNode 
{
    string node_val;
    vector<TreeNode*> children;
    TreeNode(string v)
    {
        node_val = v;
    }
};

struct Tree
{
    TreeNode* root;
};