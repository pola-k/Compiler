#include "Printer.h"
#include<iostream>
#include<string.h>
#include<fstream>
using namespace std;

void display_Tokens(vector<vector<Token>> tokens)
{
    cout << "\n\n\nTokenized Program" << endl;
    cout << "-------------------------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < tokens.size(); i++)
    {
        for (int j = 0; j < tokens[i].size(); j++)
        {
            if (tokens[i][j].position == -1)
            {
                cout <<  "< " << tokens[i][j].lexeme << " > ";
            }
            else
            {
                cout << "< " << tokens[i][j].type << " , " << tokens[i][j].position << " > ";
            }
        }
        cout << endl;
    }
}

void printTree(TreeNode* node, string prefix, bool isLast) 
{
    if (!node) 
    {
        return;
    }

    cout << prefix;
    cout << (isLast ? "`-- " : "|-- ");
    cout << node->node_val << endl;

    prefix += (isLast ? "    " : "|   ");
    for (int i = 0; i < node->children.size(); i++) 
    {
        printTree(node->children[i], prefix, i == node->children.size() - 1);
    }
}

void printProgram(string filename)
{
    ifstream readFile;
    string line;
    readFile.open(filename);

    cout << "Main Program" << endl;
    cout << "-------------------------------------------------------------------------------------------------------" << endl;

    while (getline(readFile, line))
    {
        cout << line << endl;
    }

    readFile.close();
}

void printTAC(string filename)
{
    ifstream readFile;
    string line;
    readFile.open(filename);

    cout << "\n\n\nThree Address Code" << endl;
    cout << "-------------------------------------------------------------------------------------------------------" << endl;

    while (getline(readFile, line))
    {
        cout << line << endl;
    }

    readFile.close();
}