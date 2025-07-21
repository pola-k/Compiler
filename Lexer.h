#pragma once
#include<string>
#include<vector>
#include<unordered_map>
#include<set>
#include "Common.h"
using namespace std;

extern string lexer_first_pass(string filename);
extern vector<vector<Token>> tokenize(unordered_map<string, int>* keywords, vector<pair<bool, string>> accepting_states, vector<vector<pair<set<char>, pair<int, int>>>>* transition_table,
     const string& preprocessed_content, unordered_map<string, set<char>>* char_mapping);
extern vector<Token> lexer(unordered_map<string, int>* keywords, vector<pair<bool, string>> accepting_states, vector<vector<pair<set<char>, pair<int, int>>>>* transition_table, 
    string filename, unordered_map<string, set<char>>* char_mapping);
