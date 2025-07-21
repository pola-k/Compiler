#include "File_Handler.h"
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

void write_Tokens(vector<vector<Token>> tokens)
{
    ofstream writeFile;

    writeFile.open("Tokens.txt");

    for (int i = 0; i < tokens.size(); i++)
    {
        for (int j = 0; j < tokens[i].size() ; j++)
        {
            if (tokens[i][j].position == -1)
            {
                writeFile << "< " << tokens[i][j].lexeme << " > ";
            }
            else
            {
                writeFile << "< " << tokens[i][j].type << " , " << tokens[i][j].position << " > ";
            }
        }
        writeFile << endl;
    }

    writeFile.close();
}

void write_Table(unordered_map<string, int> table, string filename)
{
    ofstream writeFile;
    writeFile.open(filename);

    vector<pair<int, string>> sortedTable;

    for (auto token : table)
    {
        sortedTable.push_back({ token.second, token.first });
    }

    sort(sortedTable.begin(), sortedTable.end());

    for (int i = 0; i < sortedTable.size(); i++)
    {
        writeFile << sortedTable[i].second << " " << sortedTable[i].first << endl;
    }

    writeFile.close();
}

void write_Errors(vector<string> errors)
{
    ofstream writeFile;
    writeFile.open("Errors.txt");

    for (int i = 0; i < errors.size(); i++)
    {
        writeFile << errors[i];
    }

    writeFile.close();
}

void writeTree(ofstream& writeFile, TreeNode* node, string prefix, bool isLast)
{
    if (!node)
    {
        return;
    }

    writeFile << prefix;
    writeFile << (isLast ? "`-- " : "|-- ");
    writeFile << node->node_val << endl;

    prefix += (isLast ? "    " : "|   ");
    for (int i = 0; i < node->children.size(); i++)
    {
        writeTree(writeFile, node->children[i], prefix, i == node->children.size() - 1);
    }
}

void writeSyntaxTree(TreeNode* node)
{
    ofstream writeFile;
    writeFile.open("Syntax Tree.txt");
    writeTree(writeFile, node);
    writeFile.close();
}

void emit(const string& code) 
{
    ofstream writeFile;
    writeFile.open("TAC.txt", ios::app);
    writeFile << code << endl;
    writeFile.close();
}