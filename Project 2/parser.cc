#include <iostream>
#include <vector>
#include "lexer.h"
#include "parser.h"

using namespace std;

void parser_begin(){
    Token new_token = lexer.GetToken();
    if(new_token.token_type != ID){
        cout << "Syntax Error"<<endl;
        exit(1); 
    } else {
        Token next_token = lexer.GetToken();
        if(next_token.token_type == LBRACE){
            lexer.UngetToken(next_token);
            lexer.UngetToken(new_token);
        } else if(next_token.token_type == COMMA){
            lexer.UngetToken(next_token);
            lexer.UngetToken(new_token);
            parse_global_variables();
        } else if(next_token.token_type == SEMICOLON){
            new_global_vector(new_token.lexeme, "::",0);        
        } else {
            cout << "Syntax Error"<<endl;
            exit(1); 
        }
        parse_scope();
    }  
    parse_eof();
}

void parse_eof(){
    Token end_token = lexer.GetToken();
    if(end_token.token_type != END_OF_FILE){
        cout << "Syntax Error"<<endl;
        exit(1); 
    }
}

void parse_global_variables(){
    Token new_token = lexer.GetToken();
    Token next_token = lexer.GetToken();
    if(new_token.token_type != ID){
        cout << "Syntax Error"<<endl;
        exit(1);
    } else if(next_token.token_type == COMMA){
        lexer.UngetToken(next_token);
        lexer.UngetToken(new_token);
        vector<string> variable_list = parse_list();
        int i = 0;
        while(i < variable_list.size())
            new_global_vector(variable_list[i++], "::", 0);
    } 
}

void parse_scope(){
    Token new_token = lexer.GetToken();
    Token next_token = lexer.GetToken();
    string curr_scope;
    if(new_token.token_type == ID && next_token.token_type == LBRACE){
        curr_scope = new_token.lexeme;
        parse_public_private_variables(curr_scope); 
        parse_public_private_variables(curr_scope);
        parse_line_list(curr_scope);
        Token this_token = lexer.GetToken();
        if(this_token.token_type != RBRACE){
            cout << "Syntax Error"<<endl;
            exit(1); 
        } else delete_symbol_vector(curr_scope);
    }
}

void parse_public_private_variables(string curr_scope){
    Token new_token = lexer.GetToken();
    if(new_token.token_type == PRIVATE || new_token.token_type == PUBLIC){
        Token next_token = lexer.GetToken();
        if(next_token.token_type == COLON){
            vector<string> variable_list = parse_list();
            int i = 0;
            if(new_token.token_type == PRIVATE){
                while(i < variable_list.size())
                    new_global_vector(variable_list[i++], curr_scope, 2);
            } else {
                while(i < variable_list.size())
                    new_global_vector(variable_list[i++], curr_scope, 1);
            }
        } else {
            cout << "Syntax Error"<<endl;
            exit(1); 
        }
    } else lexer.UngetToken(new_token);
}

void parse_line(string curr_scope){
    Token new_token = lexer.GetToken();
    string first_scope = "::";
    string second_scope = ".";
    string third_scope = " = ";
    if(new_token.token_type != ID)
        lexer.UngetToken(new_token);
    else {
        Token next_token = lexer.GetToken();
        if(next_token.token_type == LBRACE){
            lexer.UngetToken(next_token);
            lexer.UngetToken(new_token);
            parse_scope();
        } else if(next_token.token_type == EQUAL){
            Token this_token = lexer.GetToken();
            Token this_next_token = lexer.GetToken();
            if(this_token.token_type != ID && this_next_token.token_type != SEMICOLON){
                cout << "Syntax Error"<<endl;
                exit(1); 
            } else {
                string right_scope = search_global_vector(this_token.lexeme, curr_scope);
                string left_scope = search_global_vector(new_token.lexeme, curr_scope); 
                if(left_scope.length() != 0 && right_scope.length() != 0){
                    string resultant_string;
                    if(left_scope != first_scope && right_scope == first_scope) 
                        resultant_string = (left_scope + second_scope + new_token.lexeme + third_scope + right_scope + this_token.lexeme);
                    else if (left_scope != first_scope && right_scope != first_scope)
                        resultant_string = (left_scope + second_scope + new_token.lexeme + third_scope + right_scope + second_scope + this_token.lexeme);     
                    else if(left_scope == first_scope && right_scope != first_scope) 
                        resultant_string = (left_scope + new_token.lexeme + third_scope + right_scope + second_scope + this_token.lexeme);
                    else resultant_string = (left_scope + new_token.lexeme + third_scope + right_scope + this_token.lexeme);
                    output.push_back(resultant_string);
                }
            }
        } else {
            cout << "Syntax Error"<<endl;
            exit(1); 
        }
    } 
}

void parse_line_list(string curr_scope){
    Token new_token = lexer.GetToken();
    if(new_token.token_type != ID){
        cout << "Syntax Error"<<endl;
            exit(1); 
    } else {
        while(new_token.token_type == ID){
            lexer.UngetToken(new_token);
            parse_line(curr_scope);
            new_token = lexer.GetToken();
        }
        if(new_token.token_type != RBRACE){
            cout << "Syntax Error"<<endl;
            exit(1);
        }else lexer.UngetToken(new_token);
    } 
}

void new_global_vector(string variable_name, string variable_scope, int variable_instance){
    symbol_entry entry = {variable_name, variable_scope, variable_instance};
    symbol_table.push_back(entry);
}

void delete_symbol_vector(string scope){
    if(symbol_table.size() != 0){
        while(symbol_table.size() > 0 && symbol_table[symbol_table.size()-1].variable_scope == scope){
            symbol_table.pop_back();
            if(symbol_table.size() != 0) continue;
            else break;
        }
    }
}

string search_global_vector(string variable_name, string variable_scope){
    int i = symbol_table.size() - 1;
    while(i >= 0){
        if(symbol_table[i].variable_name == variable_name && symbol_table[i].variable_scope == variable_scope)
                return symbol_table[i].variable_scope;
        if(symbol_table[i].variable_name == variable_name){
            if(symbol_table[i].variable_instance == 0){
                return "::";
            } else if(symbol_table[i].variable_instance == 1){
                return symbol_table[i].variable_scope;
            }
        }
        i--;
    }
    return  "?";
}

vector<string> parse_list(){
    Token new_token = lexer.GetToken();
    vector<string> variable_list;
    if(new_token.token_type == COMMA) return parse_list();
    if(new_token.token_type == SEMICOLON) return variable_list;
    if(new_token.token_type != ID){
        cout << "Syntax Error"<<endl;
        exit(1); 
    } else {
        vector<string> response = parse_list();
        variable_list.push_back(new_token.lexeme);
        variable_list.insert(variable_list.end(), response.begin(), response.end());
    }  
    return variable_list;
}

int main(){
    parser_begin();
    parse_eof();
    int i = 0;
    while(i < output.size())
        cout << output[i++]<<endl;
}

