#include "Initialiser.h"
#include<unordered_map>
#include<string>
#include<set>
#include<vector>
#include<algorithm>
#include<fstream>
#include<string.h>
#include<iostream>
using namespace std;

unordered_map<string, set<char>>* get_Map_of_Characters()
{
    set<char> universal_set, letters, digit, other1_set, other2_set, other3_set, other4_set, other5_set, other6_set, other7_set, other8_set, other9_set, other10_set, other11_set, other12_set;
    unordered_map<string, set<char>>* character_mapping = new unordered_map<string, set<char>>;

    for (char c = 'A'; c <= 'Z'; ++c)
    {
        universal_set.insert(c);
        letters.insert(c);
    }
    for (char c = 'a'; c <= 'z'; ++c)
    {
        universal_set.insert(c);
        letters.insert(c);
    }
    for (char c = '0'; c <= '9'; ++c)
    {
        universal_set.insert(c);
        digit.insert(c);
    }

    string symbols = "+-*/%=&|^~!<>?:;,.(){}[]#\\\"' \n\t";

    for (auto c : symbols)
    {
        universal_set.insert(c);
    }

    character_mapping->insert({ "punctuation", {'[', '{', '(', ')', '}', ']', ','}});
    character_mapping->insert({ "operator", {'%', '*', '"', '/'}});
    character_mapping->insert({ "digits", digit});
    character_mapping->insert({ "letter", letters});

    set<char> temp;
    temp.insert({ '>', '=', '<' });
    set_difference(universal_set.begin(), universal_set.end(), temp.begin(), temp.end(), inserter(other1_set, other1_set.begin()));
    character_mapping->insert({ "other1", other1_set});

    temp.erase('>');
    set_difference(universal_set.begin(), universal_set.end(), temp.begin(), temp.end(), inserter(other2_set, other2_set.begin()));
    character_mapping->insert({ "other2", other2_set});

    temp.clear();
    temp.insert({ '+', '-' });
    temp.insert(digit.begin(), digit.end());
    set_difference(universal_set.begin(), universal_set.end(), temp.begin(), temp.end(), inserter(other3_set, other3_set.begin()));
    character_mapping->insert({ "other3", other3_set});

    set_difference(universal_set.begin(), universal_set.end(), digit.begin(), digit.end(), inserter(other4_set, other4_set.begin()));
    character_mapping->insert({ "other4", other4_set});

    temp.erase('+');
    temp.erase('-');
    temp.insert({ '.', 'E' });
    set_difference(universal_set.begin(), universal_set.end(), temp.begin(), temp.end(), inserter(other5_set, other5_set.begin()));
    character_mapping->insert({ "other5", other5_set});
    
    temp.erase('.');
    set_difference(universal_set.begin(), universal_set.end(), temp.begin(), temp.end(), inserter(other6_set, other6_set.begin()));
    character_mapping->insert({ "other6", other6_set});

    temp.clear();
    temp.insert({ 'i', 'o' });
    set_difference(universal_set.begin(), universal_set.end(), temp.begin(), temp.end(), inserter(other7_set, other7_set.begin()));
    character_mapping->insert({ "other7", other7_set});

    temp.clear();
    temp.insert('_');
    temp.insert(letters.begin(), letters.end());
    set_difference(universal_set.begin(), universal_set.end(), temp.begin(), temp.end(), inserter(other8_set, other8_set.begin()));
    character_mapping->insert({ "other8", other8_set});

    temp.clear();
    temp.insert('n');
    set_difference(letters.begin(), letters.end(), temp.begin(), temp.end(), inserter(other9_set, other9_set.begin()));
    character_mapping->insert({ "other9", other9_set});

    temp.clear();
    temp.insert('p');
    set_difference(letters.begin(), letters.end(), temp.begin(), temp.end(), inserter(other10_set, other10_set.begin()));
    character_mapping->insert({ "other10", other10_set});

    temp.clear();
    temp.insert('u');
    set_difference(letters.begin(), letters.end(), temp.begin(), temp.end(), inserter(other11_set, other11_set.begin()));
    character_mapping->insert({ "other11", other11_set});

    temp.clear();
    temp.insert('t');
    set_difference(letters.begin(), letters.end(), temp.begin(), temp.end(), inserter(other12_set, other12_set.begin()));
    character_mapping->insert({ "other12", other12_set});

    character_mapping->insert({ "_", {'_'}});
    character_mapping->insert({ "i", {'i'} });
    character_mapping->insert({ "n", {'n'} });
    character_mapping->insert({ "p", {'p'} });
    character_mapping->insert({ "u", {'u'} });
    character_mapping->insert({ "t", {'t'} });
    character_mapping->insert({ "-", {'-'} });
    character_mapping->insert({ ">", {'>'} });
    character_mapping->insert({ "=", {'='} });
    character_mapping->insert({ "|", {'|'} });
    character_mapping->insert({ "o", {'o'} });
    character_mapping->insert({ "<", {'<'} });
    character_mapping->insert({ "&", {'&'} });
    character_mapping->insert({ ":", {':'} });
    character_mapping->insert({ "!", {'!'} });
    character_mapping->insert({ "+", {'+'} });
    character_mapping->insert({ ".", {'.'} });
    character_mapping->insert({ "E", {'E'} });

    return character_mapping;
}

vector<pair<bool, string>> get_Accepting_State_Mapping()
{
    vector<pair<bool, string>> mapping;

    ifstream readFile;
    readFile.open("Accepting-States.txt");

    if (!readFile.is_open())
    {
        cerr << "Error: Could not open file Accepting-States.txt" << endl;
        return {};
    }

    int status;
    string return_type;

    while (!readFile.eof())
    {
        readFile >> status;
        readFile >> return_type;
        mapping.push_back({ status, return_type });
    }

    readFile.close();
    
    return mapping;
}

unordered_map<string, int>* get_Keywords()
{
    unordered_map<string, int>* keywords = new unordered_map<string, int>;
    ifstream readFile;
    
    readFile.open("Keywords.txt");
    if (!readFile.is_open())
    {
        cerr << "Error: Could not open file Keywords.txt" << endl;
        return nullptr;
    }

    string word;

    while (!readFile.eof())
    {
        readFile >> word;
        keywords->insert({ word, 0 });
    }

    readFile.close();

    return keywords;
}

vector<vector<pair<set<char>, pair<int,int>>>>* get_Transition_Table(unordered_map<string,set<char>>*& get_Map_of_Characters)
{
    string character_set;
    int source;
    int destination;
    int advance;

    ifstream readFile;
    readFile.open("Transition-Table.txt");

    if (!readFile.is_open())
    {
        cerr << "Error: Could not open file Transition-Table.txt" << endl;
        return nullptr;
    }

    readFile >> TOTAL_STATES;
    vector<vector<pair<set<char>, pair<int, int>>>>* transition_table = new vector<vector<pair<set<char>, pair<int, int>>>>(TOTAL_STATES);

    while (!readFile.eof())
    {
        readFile >> source;
        readFile >> destination;
        readFile >> character_set;
        readFile >> advance;

        transition_table->at(source).push_back({ get_Map_of_Characters->at(character_set), {destination, advance} });
    }

    readFile.close();

    return transition_table;
}

pair<int,int> get_Next_Configuration(vector<pair<set<char>, pair<int, int>>> transition_table, char next_character)
{
    int next_state = -1;
    int not_advance_state = 0;

    for (int i = 0; i < transition_table.size(); i++)
    {
        if (next_state != -1)
        {
            break;
        }
        else
        {
            if (transition_table[i].first.find(next_character) != transition_table[i].first.end())
            {
                next_state = transition_table[i].second.first;
                not_advance_state = transition_table[i].second.second;
            }
        }
    }

    return { next_state, not_advance_state };
}