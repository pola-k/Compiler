#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Common.h"
using namespace std;

extern string newTemp();
extern string newLabel();
extern pair<bool, ExprAttrs> Mag_(vector<Token> tokens, int& num, TreeNode* parent, const string& expr);
extern pair<bool, ExprAttrs> Mag(vector<Token> tokens, int& num, TreeNode* parent);
extern pair<bool, ExprAttrs> Rvalue_(vector<Token> tokens, int& num, TreeNode* parent);
extern pair<bool, ExprAttrs> Expr(vector<Token> tokens, int& num, TreeNode* parent);
extern pair<bool, ExprAttrs> Factor(vector<Token> tokens, int& num, TreeNode* parent);
extern pair<bool, ExprAttrs> Term_(vector<Token> tokens, int& num, TreeNode* parent, const string& expr);
extern pair<bool, ExprAttrs> Term(vector<Token> tokens, int& num, TreeNode* parent);
extern pair<bool, ExprAttrs> OptExpr(vector<Token> tokens, int& num, TreeNode* parent);
extern pair<bool, ExprAttrs> Expr_(vector<Token> tokens, int& num, TreeNode* parent, const string& id);
extern bool ForStmt(vector<Token> tokens, int& num, TreeNode* parent);
extern bool WhileStmt(vector<Token> tokens, int& num, TreeNode* parent);
extern bool IfStmt(vector<Token> tokens, int& num, TreeNode* parent);
extern bool StmtList_(vector<Token> tokens, int& num, TreeNode* parent);
extern bool StmtList(vector<Token> tokens, int& num, TreeNode* parent);
extern bool CompoundStmt(vector<Token> tokens, int& num, TreeNode* parent);
extern bool Type(vector<Token> tokens, int& num, TreeNode* parent, string& data_type);
extern bool Stmt(vector<Token> tokens, int& num, TreeNode* parent);
extern bool Arg(vector<Token> tokens, int& num, TreeNode* parent);
extern bool Arglist_(vector<Token> tokens, int& num, TreeNode* parent);
extern bool Arglist(vector<Token> tokens, int& num, TreeNode* parent);
extern bool Declaration(vector<Token> tokens, int& num, TreeNode* parent);
extern bool conIdentList(vector<Token> tokens, int& num, TreeNode* parent, vector<string>& list);
extern bool IdentList(vector<Token> tokens, int& num, TreeNode* parent, vector<string>& list);
extern bool ElsePart(vector<Token> tokens, int& num, TreeNode* parent);
extern TreeNode* recursive_Descent_Parser(vector<Token> tokens);
extern int temp_counter;
extern int label_counter;