#pragma once
#include<unordered_map>
#include<string>
#include<set>
#include<vector>
using namespace std;

extern unordered_map<string, set<char>>* get_Map_of_Characters();
extern vector<pair<bool, string>> get_Accepting_State_Mapping();
extern unordered_map<string, int>* get_Keywords();
extern vector<vector<pair<set<char>, pair<int,int>>>>* get_Transition_Table(unordered_map<string,set<char>>*& get_Map_of_Characters);
extern pair<int,int> get_Next_Configuration(vector<pair<set<char>, pair<int, int>>> transition_table, char next_character);
extern int TOTAL_STATES;