#include "Common.h"
#include "File_Handler.h"
#include "Initialiser.h"
#include "Lexer.h"
#include "Parser.h"
#include "Printer.h"
#include "Memory_Deallocation.h"
#include<iostream> 
#include<vector>
#include<unordered_map>
#include<set>
#include<string.h>
#include<algorithm>
#include<fstream>
#include<string>
using namespace std;

int temp_counter = 0; 
int label_counter = 0;
int TOTAL_STATES = 0;

int main()
{
    ofstream writeFile("TAC.txt");
    writeFile.close();

    string FILE_NAME = "Program.txt";
    printProgram(FILE_NAME);

    unordered_map<string, set<char>>* character_mapping = get_Map_of_Characters();

    vector<vector<pair<set<char>, pair<int, int>>>>* transition_table = get_Transition_Table(character_mapping);
    if (transition_table == nullptr)
    {
        cout << "Transition Table is Empty" << endl;
        return 0;
    }

    vector<pair<bool, string>> accepting_states = get_Accepting_State_Mapping();
    if (accepting_states.empty())
    {
        cout << "There are no Accepting States" << endl;
        return 0;
    }

    unordered_map<string, int>* keywords = get_Keywords();
    if (keywords == nullptr)
    {
        cout << "There are no Keywords" << endl;
        return 0;
    }

    vector<Token> tokens = lexer(keywords, accepting_states, transition_table, FILE_NAME, character_mapping);

    TreeNode* root = recursive_Descent_Parser(tokens);

    if (root)
    {
        cout << "\n\n\nSyntax Tree" << endl;
        cout << "-------------------------------------------------------------------------------------------------------" << endl;
        printTree(root);
        writeSyntaxTree(root);
    }
    else
    {
        ofstream deleteFile;
        deleteFile.open("TAC.txt");
        deleteFile.close();
    }

    printTAC("TAC.txt");

    deallocate_Memory(transition_table, character_mapping, keywords);
    return 0;
}