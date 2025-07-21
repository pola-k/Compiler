#include "Memory_Deallocation.h"
#include <string>
#include <vector>
#include<set>
#include<unordered_map>
using namespace std;

void deallocate_Memory(vector<vector<pair<set<char>, pair<int, int>>>>*& transition_table, unordered_map<string, set<char>>*& mapping, unordered_map<string, int>*& keywords)
{
    if (transition_table != nullptr)
    {
        delete transition_table;
        transition_table = nullptr;
    }

    if (mapping != nullptr)
    {
        delete mapping;
        mapping = nullptr;
    }

    if (keywords != nullptr)
    {
        delete keywords;
        keywords = nullptr;
    }
}
