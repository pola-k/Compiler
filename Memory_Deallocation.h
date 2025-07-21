#pragma once
#include <string>
#include <vector>
#include<set>
#include<unordered_map>
using namespace std;

extern void deallocate_Memory(vector<vector<pair<set<char>, pair<int, int>>>>*& transition_table, unordered_map<string, set<char>>*& mapping, unordered_map<string, int>*& keywords);
