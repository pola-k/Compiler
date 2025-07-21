#include "Lexer.h"
#include<iostream>
#include<fstream>
#include<unordered_map>
#include<set>
#include<vector>
#include "Initialiser.h"
#include "Printer.h"
#include "File_Handler.h"
using namespace std;

string lexer_first_pass(string filename) 
{
    ifstream readFile;
    readFile.open(filename);

    if (!readFile) 
    {
        cerr << "Error: Unable to open file " << filename << endl;
        return "";
    }

    const int BUFFER_SIZE = 25;
    char buffer[BUFFER_SIZE];
    char next_buffer[BUFFER_SIZE];
    int buffer_pos = 0;
    int buffer_end = 0;
    int next_buffer_end = 0;
    bool eof_reached = false;
    bool in_single_line_comment = false;
    bool in_multi_line_comment = false;
    bool last_char_was_space = true;
    bool in_string_literal = false;
    int line_count = 1;
    string processed_content;

    buffer_end = readFile.read(buffer, BUFFER_SIZE).gcount();
    if (buffer_end < BUFFER_SIZE) 
    {
        eof_reached = true;
    }
    else 
    {
        next_buffer_end = readFile.read(next_buffer, BUFFER_SIZE).gcount();
        if (next_buffer_end < BUFFER_SIZE) 
        {
            eof_reached = true;
        }
    }
    while (true) 
    {
        if (buffer_pos >= buffer_end) 
        {
            if (buffer_end == 0 || (eof_reached && next_buffer_end == 0)) 
            {
                break;
            }

            memcpy(buffer, next_buffer, next_buffer_end);
            buffer_end = next_buffer_end;
            buffer_pos = 0;

            if (!eof_reached) 
            {
                next_buffer_end = readFile.read(next_buffer, BUFFER_SIZE).gcount();
                if (next_buffer_end < BUFFER_SIZE) 
                {
                    eof_reached = true;
                }
            }
            else 
            {
                next_buffer_end = 0;
            }

            if (buffer_end == 0) 
            {
                break;
            }
        }

        char current_char = buffer[buffer_pos];
        char next_char = '\0';

        if (buffer_pos + 1 < buffer_end) 
        {
            next_char = buffer[buffer_pos + 1];
        }
        else if (next_buffer_end > 0) 
        {
            next_char = next_buffer[0];
        }

        if (current_char == '"' || current_char == '\\')
        {
            if (in_string_literal && current_char == '\\' && next_char == '"')
            {
                processed_content.push_back(current_char);
                processed_content.push_back(next_char);
                last_char_was_space = false;
                buffer_pos+= 2;
                continue;
            }
            else if(current_char == '"')
            {
                in_string_literal = !in_string_literal;
            }
        }

        if (!in_string_literal && !in_single_line_comment && !in_multi_line_comment) 
        {
            if (current_char == '/' && next_char == '/') 
            {
                in_single_line_comment = true;
                buffer_pos += 2;
                continue;
            }
            else if (current_char == '/' && next_char == '*') 
            {
                in_multi_line_comment = true;
                buffer_pos += 2;
                continue;
            }
        }
        else if (in_single_line_comment) 
        {
            if (current_char == '\n') 
            {
                in_single_line_comment = false;
                processed_content.push_back('\n');
                line_count++;
                last_char_was_space = true;
            }
            buffer_pos++;
            continue;
        }
        else if (in_multi_line_comment) 
        {
            if (current_char == '*' && next_char == '/') 
            {
                in_multi_line_comment = false;
                buffer_pos += 2;
                continue;
            }
            else if (current_char == '\n') 
            {
                processed_content.push_back('\n');
                line_count++;
                last_char_was_space = true;
                buffer_pos++;
                continue;
            }
            buffer_pos++;
            continue;
        }

        if (current_char == ' ' || current_char == '\t') 
        {
            if (!last_char_was_space) 
            {
                processed_content.push_back(' ');
                last_char_was_space = true;
            }
            buffer_pos++;
            continue;
        }
        else if (current_char == '\n') 
        {
            processed_content.push_back('\n');
            line_count++;
            last_char_was_space = true;
            buffer_pos++;
            if (in_string_literal)
            {
                in_string_literal = false;
            }
            continue;
        }

        processed_content.push_back(current_char);
        last_char_was_space = false;
        buffer_pos++;
    }

    readFile.close();

    return processed_content;
}

vector<vector<Token>> tokenize(unordered_map<string, int>* keywords, vector<pair<bool, string>> accepting_states, vector<vector<pair<set<char>, pair<int, int>>>>* transition_table, const string& preprocessed_content, unordered_map<string, set<char>>* char_mapping) 
{
    unordered_map<string, int> keyword_table_content;
    unordered_map<string, int> identifier_table_content;
    unordered_map<string, int> string_literal_content;
    unordered_map<string, int> punctuation_table;
    unordered_map<string, int> operator_table;
    unordered_map<string, int> number_table;
    vector<vector<Token>> tokens;
    vector<Token> current_line_tokens;
    vector<string> error_content;

    const int BUFFER_SIZE = 25;
    char buffer[BUFFER_SIZE];
    char next_buffer[BUFFER_SIZE];
    int buffer_pos = 0;
    int buffer_end = 0;
    int next_buffer_end = 0;
    bool eof_reached = false;
    int line_count = 1;

    const char* content = preprocessed_content.c_str();
    int content_length = preprocessed_content.length();
    int content_pos = 0;

    buffer_end = min(BUFFER_SIZE, content_length);
    memcpy(buffer, content, buffer_end);

    if (buffer_end == BUFFER_SIZE && buffer_end < content_length)
    {
        int remaining = content_length - buffer_end;
        next_buffer_end = min(BUFFER_SIZE, remaining);
        memcpy(next_buffer, content + buffer_end, next_buffer_end);
    }
    else 
    {
        eof_reached = true;
    }

    while (buffer_pos < buffer_end || (!eof_reached && next_buffer_end > 0)) 
    {
        if (buffer_pos >= buffer_end) 
        {
            if (next_buffer_end > 0) 
            {
                memcpy(buffer, next_buffer, next_buffer_end);
                buffer_end = next_buffer_end;
                buffer_pos = 0;

                content_pos += BUFFER_SIZE;

                int offset = content_pos + buffer_end;
                if (offset < content_length) 
                {
                    int remaining = content_length - offset;
                    next_buffer_end = min(BUFFER_SIZE, remaining);
                    memcpy(next_buffer, content + offset, next_buffer_end);
                }
                else 
                {
                    next_buffer_end = 0;
                    eof_reached = true;
                }
            }
            else 
            {
                break;
            }
        }

        char current_char = buffer[buffer_pos];

        if (current_char == '\n') 
        {
            if (!current_line_tokens.empty()) 
            {
                tokens.push_back(current_line_tokens);
                current_line_tokens.clear();
            }
            line_count++;
            buffer_pos++;
            continue;
        }

        if (current_char == ' ') 
        {
            buffer_pos++;
            continue;
        }

        string current_lexeme = "";
        int current_state = 0;
        bool error_occurred = false;

        if (current_char == '"') 
        {
            current_lexeme += current_char;
            buffer_pos++;

            while (buffer_pos < buffer_end || !eof_reached) 
            {
                if (buffer_pos >= buffer_end) 
                {
                    if (next_buffer_end > 0) 
                    {
                        memcpy(buffer, next_buffer, next_buffer_end);
                        buffer_end = next_buffer_end;
                        buffer_pos = 0;

                        content_pos += BUFFER_SIZE;

                        int offset = content_pos + buffer_end;
                        if (offset < content_length) 
                        {
                            int remaining = content_length - offset;
                            next_buffer_end = min(BUFFER_SIZE, remaining);
                            memcpy(next_buffer, content + offset, next_buffer_end);
                        }
                        else 
                        {
                            next_buffer_end = 0;
                            eof_reached = true;
                        }
                    }
                    else 
                    {
                        string error = "Line " + to_string(line_count) + ": Unterminated string literal\n";
                        error_content.push_back(error);
                        error_occurred = true;
                        break;
                    }
                }

                char c = buffer[buffer_pos];
                current_lexeme += c;
                buffer_pos++;

                if (c == '"' && (current_lexeme.length() < 2 || current_lexeme[current_lexeme.length() - 2] != '\\')) 
                {
                    Token token;
                    if (string_literal_content.find(current_lexeme) == string_literal_content.end()) 
                    {
                        string_literal_content.insert({ current_lexeme, string_literal_content.size() + 1 });
                    }
                    token.type = "STRING-LITERAL";
                    token.lexeme = current_lexeme;
                    token.position = string_literal_content.size();

                    current_line_tokens.push_back(token);
                    break;
                }
                if (c == '\n') 
                {
                    string error = "Line " + to_string(line_count) + ": Unterminated String Literal\n";
                    error_content.push_back(error);
                    error_occurred = true;
                    line_count++;
                    break;
                }
            }

            continue;
        }

        while (true) 
        {
            if (buffer_pos >= buffer_end && eof_reached && next_buffer_end == 0) 
            {
                break;
            }
            if (buffer_pos >= buffer_end) 
            {
                if (next_buffer_end > 0) 
                {
                    memcpy(buffer, next_buffer, next_buffer_end);
                    buffer_end = next_buffer_end;
                    buffer_pos = 0;

                    content_pos += BUFFER_SIZE;

                    int offset = content_pos + buffer_end;
                    if (offset < content_length) 
                    {
                        int remaining = content_length - offset;
                        next_buffer_end = min(BUFFER_SIZE, remaining);
                        memcpy(next_buffer, content + offset, next_buffer_end);
                    }
                    else 
                    {
                        next_buffer_end = 0;
                        eof_reached = true;
                    }
                }
                else 
                {
                    break;
                }
            }

            char next_char = buffer[buffer_pos];

            pair<int, int> next_configuration = get_Next_Configuration(transition_table->at(current_state), next_char);
            int next_state = next_configuration.first;
            int not_advance_state = next_configuration.second;

            if (next_state == -1) 
            {
                    Token token;
                    string lexeme = current_lexeme;

                    if (current_state == 55 || accepting_states[current_state].second == "id") 
                    {
                        if (identifier_table_content.find(lexeme) == identifier_table_content.end()) 
                        {
                            identifier_table_content.insert({ lexeme, identifier_table_content.size() + 1 });
                        }
                        token.type = "ID";
                        token.position = identifier_table_content[lexeme];
                        token.lexeme = lexeme;
                        current_line_tokens.push_back(token);
                    }
                    else if (accepting_states[current_state].second == "keyword" || current_state == 53) 
                    {
                        if (keywords->find(lexeme) == keywords->end()) 
                        {
                            if (!lexeme.empty()) 
                            { 
                                string error = "Line " + to_string(line_count) + ": Invalid Keyword or Identifier: " + lexeme + "\n";
                                error_content.push_back(error);
                            }
                        }
                        else 
                        {
                            if (keyword_table_content.find(lexeme) == keyword_table_content.end()) 
                            {
                                keyword_table_content.insert({ lexeme, keyword_table_content.size() + 1 });
                            }

                            token.type = "KEYWORD";
                            token.position = keyword_table_content.size();
                            token.lexeme = lexeme;

                            current_line_tokens.push_back(token);
                        }
                    }
                    else if (accepting_states[current_state].second == "number" || current_state == 40 || current_state == 51 || current_state == 49) 
                    {
                        if (number_table.find(lexeme) == number_table.end()) 
                        {
                            number_table.insert({ lexeme, number_table.size() + 1 });
                        }

                        token.type = "NUMBER";
                        token.lexeme = lexeme;
                        token.position = -1;

                        current_line_tokens.push_back(token);
                    }
                    else if (accepting_states[current_state].second == "punctuation") 
                    {
                        if (punctuation_table.find(lexeme) == punctuation_table.end()) 
                        {
                            punctuation_table.insert({ lexeme, punctuation_table.size() + 1 });
                        }

                        token.type = "PUNCTUATION";
                        token.lexeme = lexeme;
                        token.position = -1;
                      
                        current_line_tokens.push_back(token);
                    }
                    else if (accepting_states[current_state].second == "operator" || current_state == 34 || current_state == 29 || current_state == 38 || current_state == 39) 
                    {
                        if (operator_table.find(lexeme) == operator_table.end()) 
                        {
                            operator_table.insert({ lexeme, operator_table.size() + 1 });
                        }

                        token.type = "OPERATOR";
                        token.lexeme = lexeme;
                        token.position = -1;

                        current_line_tokens.push_back(token);
                    }
                    else if (!lexeme.empty()) 
                    {
                        string error = "Line " + to_string(line_count) + ": Unrecognized token: " + lexeme + "\n";
                        error_content.push_back(error);
                    }

                    bool illegal_character = true;

                    for (auto char_set : *char_mapping)
                    {
                        if (char_set.second.find(next_char) != char_set.second.end())
                        {
                            illegal_character = false;
                            break;
                        }
                    }

                    if(illegal_character)
                    {
                        string error = "Line " + to_string(line_count) + ": Illegal character: " + next_char + "\n";
                        error_content.push_back(error);
                        buffer_pos++;
                    }
                    break;
            }
            else 
            {
                if (!not_advance_state) 
                {
                    current_lexeme += next_char;
                    buffer_pos++;
                }
                else 
                {
                    current_state = next_state;
                    continue;
                }

                current_state = next_state;

                char peek_char = '\0';
                if (buffer_pos < buffer_end) 
                {
                    peek_char = buffer[buffer_pos];
                }
                else if (next_buffer_end > 0) 
                {
                    peek_char = next_buffer[0];
                }
                else if (eof_reached || peek_char != '\0')
                {
                    Token token;
                    string lexeme = current_lexeme;

                    if (current_state == 56 || accepting_states[current_state].second == "id")
                    {
                        if (identifier_table_content.find(lexeme) == identifier_table_content.end())
                        {
                            identifier_table_content.insert({ lexeme, identifier_table_content.size() + 1 });
                        }

                        token.type = "ID";
                        token.position = identifier_table_content[lexeme];
                        token.lexeme = lexeme;

                        current_line_tokens.push_back(token);
                    }
                    else if (accepting_states[current_state].second == "keyword" || current_state == 53)
                    {
                        if (keywords->find(lexeme) == keywords->end())
                        {
                            if (!lexeme.empty())
                            {
                                string error = "Line " + to_string(line_count) + ": Invalid Keyword or Identifier: " + lexeme + "\n";
                                error_content.push_back(error);
                            }
                        }
                        else
                        {
                            if (keyword_table_content.find(lexeme) == keyword_table_content.end())
                            {
                                keyword_table_content.insert({ lexeme, keyword_table_content.size() + 1 });
                            }

                            token.type = "KEYWORD";
                            token.position = keyword_table_content.size();
                            token.lexeme = lexeme;

                            current_line_tokens.push_back(token);
                        }
                    }
                    else if (accepting_states[current_state].second == "number" || current_state == 40 || current_state == 51 || current_state == 49)
                    {
                        if (number_table.find(lexeme) == number_table.end())
                        {
                            number_table.insert({ lexeme, number_table.size() + 1 });
                        }

                        token.type = "NUMBER";
                        token.position = -1;
                        token.lexeme = lexeme;

                        current_line_tokens.push_back(token);
                    }
                    else if (accepting_states[current_state].second == "punctuation")
                    {
                        if (punctuation_table.find(lexeme) == punctuation_table.end())
                        {
                            punctuation_table.insert({ lexeme, punctuation_table.size() + 1 });
                        }

                        token.type = "PUNCTUATION";
                        token.position = -1;
                        token.lexeme = lexeme;

                        current_line_tokens.push_back(token);
                    }
                    else if (accepting_states[current_state].second == "operator" || current_state == 34 || current_state == 29 || current_state == 38 || current_state == 39)
                    {
                        if (operator_table.find(lexeme) == operator_table.end())
                        {
                            operator_table.insert({ lexeme, operator_table.size() + 1 });
                        }

                        token.type = "OPERATOR";
                        token.position = -1;
                        token.lexeme = lexeme;

                        current_line_tokens.push_back(token);
                    }

                    break;
                }
            }
        }
    }

    if (!current_line_tokens.empty())
    {
        tokens.push_back(current_line_tokens);
    }

    display_Tokens(tokens);
    write_Tokens(tokens);
    write_Table(identifier_table_content, "Symbol-Table.txt");
    write_Table(string_literal_content, "String-Literal-Table.txt");
    write_Table(keyword_table_content, "Keyword-Table.txt");
    write_Table(punctuation_table, "Punctuation-Table.txt");
    write_Table(operator_table, "Operator-Table.txt");
    write_Table(number_table, "Number-Table.txt");
    write_Errors(error_content);

    return tokens;
}

vector<Token> lexer(unordered_map<string, int>* keywords, vector<pair<bool, string>> accepting_states, vector<vector<pair<set<char>, pair<int, int>>>>* transition_table, string filename, unordered_map<string, set<char>>* char_mapping) {

    string preprocessed_content = lexer_first_pass(filename);

    if (preprocessed_content.empty()) 
    {
        cerr << "Error: Unable to preprocess file " << filename << endl;
        return {};
    }

    vector<vector<Token>> token_two_d = tokenize(keywords, accepting_states, transition_table, preprocessed_content, char_mapping);
    vector<Token> tokens;

    for (int i = 0; i < token_two_d.size(); i++)
    {
        for (int j = 0; j < token_two_d[i].size(); j++)
        {
            tokens.push_back(token_two_d[i][j]);
        }
    }

    return tokens;
}
