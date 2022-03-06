#include <iostream>
#include <vector>
#include "lexer.h"

using namespace std;

struct symbol_entry{
    string variable_name;
    string variable_scope;
    int variable_instance;
};

LexicalAnalyzer lexer;
vector<string> output;
vector<struct symbol_entry> symbol_table;

void parser_begin();
void parse_eof();
void parse_global_variables();
void parse_scope();
void parse_public_private_variables(string curr_scope);
void parse_line(string curr_scope);
void parse_line_list(string curr_scope);

void new_global_vector(string variable_name, string variable_scope, int variable_instance);
void delete_symbol_vector(string closed_scope);
string search_global_vector(string variable_name, string variable_scope);
vector<string> parse_list();






