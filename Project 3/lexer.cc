#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = {
    "END_OF_FILE", "INT", "REAL", "BOOL",
    "TRUE", "FALSE", "IF", "WHILE", "SWITCH",
    "CASE", "PUBLIC", "PRIVATE", "NUM",
    "REALNUM", "NOT", "PLUS", "MINUS", "MULT",
    "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL",
    "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON",
    "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID",
    "ERROR"};

#define KEYWORDS_COUNT 11
string keyword[] = {"int", "real", "bool", "true",
                    "false", "if", "while", "switch",
                    "case", "public", "private"};

struct TableEntry
{
    string name;
    int line_number;
    int type;
    int printed;
};

struct Table
{
    TableEntry *item;
    Table *prev;
    Table *next;
};

LexicalAnalyzer input;
Table *symbol_table;
Token token;
int line = 0;
int enum_count = 4;
string output = "";

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int)this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    line = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char ch;
    bool spacing = false;

    input.GetChar(ch);
    line_no += (ch == '\n');
    line = line_no;

    while (!input.EndOfInput() && isspace(ch))
    {
        spacing = true;
        input.GetChar(ch);
        line_no += (ch == '\n');
        line = line_no;
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(ch);
    }
    return spacing;
}

bool LexicalAnalyzer::SkipComments()
{
    char ch;
    bool comments = false;
    if (input.EndOfInput())
    {
        input.UngetChar(ch);
        return comments;
    }

    input.GetChar(ch);

    if (ch == '/')
    {
        input.GetChar(ch);
        if (ch == '/')
        {
            comments = true;
            while (ch != '\n')
            {
                comments = true;
                input.GetChar(ch);
            }
            line_no++;
            line = line_no;
            SkipComments();
        }
        else
        {
            comments = false;
            exit(0);
        }
    }
    else
    {
        input.UngetChar(ch);
        return comments;
    }
    return true;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return (TokenType)(i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    bool realNUM = false;
    input.GetChar(c);
    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            input.GetChar(c);
            if (c == '.')
            {
                input.GetChar(c);

                if (!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                    input.UngetChar(c);
                }
            }
            else
            {
                input.UngetChar(c);
            }
        }
        else
        {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if (c == '.')
            {
                input.GetChar(c);

                if (!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
        if (realNUM)
        {
            tmp.token_type = REALNUM;
        }
        else
        {
            tmp.token_type = NUM;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c))
    {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c))
        {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;

        if (IsKeyword(tmp.lexeme))
        {
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        }
        else
        {
            tmp.token_type = ID;
        }
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

TokenType LexicalAnalyzer::UngetToken(Token token)
{
    tokens.push_back(token);
    return token.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;
    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    if (c == '!')
    {
        tmp.token_type = NOT;
        return tmp;
    }
    if (c == '+')
    {
        tmp.token_type = PLUS;
        return tmp;
    }
    if (c == '-')
    {
        tmp.token_type = MINUS;
        return tmp;
    }
    if (c == '*')
    {
        tmp.token_type = MULT;
        return tmp;
    }
    if (c == '/')
    {
        tmp.token_type = DIV;
        return tmp;
    }
    if (c == '>')
    {
        input.GetChar(c);
        if (c == '=')
        {
            tmp.token_type = GTEQ;
        }
        else
        {
            input.UngetChar(c);
            tmp.token_type = GREATER;
        }
        return tmp;
    }
    if (c == '<')
    {
        input.GetChar(c);
        if (c == '=')
        {
            tmp.token_type = LTEQ;
        }
        else if (c == '>')
        {
            tmp.token_type = NOTEQUAL;
        }
        else
        {
            input.UngetChar(c);
            tmp.token_type = LESS;
        }
        return tmp;
    }

    if (c == '(')
    {
        tmp.token_type = LPAREN;
        return tmp;
    }

    if (c == ')')
    {
        tmp.token_type = RPAREN;
        return tmp;
    }
    if (c == '=')
    {
        tmp.token_type = EQUAL;
        return tmp;
    }
    if (c == ':')
    {
        tmp.token_type = COLON;
        return tmp;
    }
    if (c == ',')
    {
        tmp.token_type = COMMA;
        return tmp;
    }
    if (c == ';')
    {
        tmp.token_type = SEMICOLON;
        return tmp;
    }
    if (c == '{')
    {
        tmp.token_type = LBRACE;
        return tmp;
    }
    if (c == '}')
    {
        tmp.token_type = RBRACE;
        return tmp;
    }

    if (isdigit(c))
    {
        input.UngetChar(c);
        return ScanNumber();
    }
    else if (isalpha(c))
    {
        input.UngetChar(c);
        return ScanIdOrKeyword();
    }
    else if (input.EndOfInput())
    {
        tmp.token_type = END_OF_FILE;
    }
    else
    {
        tmp.token_type = ERROR;
    }
    return tmp;
}

void add_list(std::string name, int line, int type)
{
    if (symbol_table != NULL)
    {
        Table *temp = symbol_table;
        while (temp->next != NULL)
        {
            if (temp->item->name == name)
            {
                temp->item->type = type;
                return;
            }
            temp = temp->next;
        }
        TableEntry *new_item = new TableEntry();
        new_item->name = name;
        new_item->line_number = token.line_no;
        new_item->type = type;
        new_item->printed = 0;
        Table *new_entry = new Table();
        new_entry->item = new_item;
        new_entry->next = NULL;
        new_entry->prev = temp;
        temp->next = new_entry;
    }
    else
    {
        TableEntry *new_item = new TableEntry();
        new_item->name = name;
        new_item->line_number = token.line_no;
        new_item->type = type;
        new_item->printed = 0;
        Table *new_entry = new Table();
        new_entry->item = new_item;
        new_entry->next = NULL;
        new_entry->prev = NULL;
        symbol_table = new_entry;
    }
}

int search_list(std::string n)
{
    Table *temp = symbol_table;
    bool found = false;
    if (symbol_table != NULL)
    {
        while (temp->next != NULL)
        {
            if (strcmp(temp->item->name.c_str(), n.c_str()) != 0)
            {
                temp = temp->next;
            }
            else
            {
                found = true;
                return (temp->item->type);
            }
        }
        if (strcmp(temp->item->name.c_str(), n.c_str()) == 0)
        {
            found = true;
            return (temp->item->type);
        }
    }
    add_list(n, token.line_no, enum_count);
    enum_count++;
    return (enum_count - 1);
}

int parse_variable_list()
{
    int var;
    token = input.GetToken();
    add_list(token.lexeme, token.line_no, 0);

    if (token.token_type == ID)
    {
        token = input.GetToken();
        if (token.token_type == COLON)
        {
            input.UngetToken(token);
        }
        if (token.token_type == COMMA)
        {
            var = parse_variable_list();
        }
    }

    return 0;
}

int parse_body();

int parse_unary_operator()
{
    token = input.GetToken();
}

int parse_binary_operator()
{
    token = input.GetToken();
    if (token.token_type == PLUS)
    {
        return 15;
    }
    if (token.token_type == MINUS)
    {
        return 16;
    }
    if (token.token_type == MULT)
    {
        return 17;
    }
    if (token.token_type == DIV)
    {
        return 18;
    }
    if (token.token_type == GTEQ)
    {
        return 19;
    }
    if (token.token_type == GREATER)
    {
        return 20;
    }
    if (token.token_type == LTEQ)
    {
        return 21;
    }
    if (token.token_type == NOTEQUAL)
    {
        return 22;
    }
    if (token.token_type == LESS)
    {
        return 23;
    }
    if (token.token_type == EQUAL)
    {
        return 26;
    }

    return -1;
}

int parse_primary()
{
    token = input.GetToken();
    if (token.token_type == ID)
    {
        return search_list(token.lexeme);
    }
    if (token.token_type == NUM)
    {
        return 1;
    }
    if (token.token_type == REALNUM)
    {
        return 2;
    }
    if (token.token_type == TRUE)
    {
        return 3;
    }
    if (token.token_type == FALSE)
    {
        return 4;
    }
    return 0;
}

bool expression(int c)
{
    if (c != 15 && c != 16 && c != 17 && c != 18 && c != 19 && c != 20 && c != 21 &&
        c != 22 && c != 23 && c != 26)
    {
        return true;
    }
    return false;
}

int parse_expression()
{
    int var;
    token = input.GetToken();
    if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM ||
        token.token_type == TRUE || token.token_type == FALSE)
    {
        input.UngetToken(token);
        var = parse_primary();
    }
    if (token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT ||
        token.token_type == DIV || token.token_type == GREATER || token.token_type == LESS ||
        token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL ||
        token.token_type == NOTEQUAL)
    {
        input.UngetToken(token);
        var = parse_binary_operator();
        int left = parse_expression();
        int right = parse_expression();
        if ((left != right) || expression(var))
        {
            if (var == 15 || var == 16 || var == 17 || var == 18)
            {
                if (left < 3 && right > 3)
                {
                    update_types(right, left);
                    right = left;
                }
                else if (left > 3 && right < 3)
                {
                    update_types(right, left);
                    left = right;
                }
                else if (left > 3 && right > 3)
                {
                    update_types(right, left);
                    right = left;
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                    exit(0);
                }
            }
            else if (var == 19 || var == 20 || var == 21 || var == 22 || var == 23 || var == 26)
            {
                if (right > 3 && left > 3)
                {
                    update_types(right, left);
                    right = left;
                    return 3;
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                    exit(0);
                }
            }
            else
            {
                cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                exit(0);
            }
        }
        if (var == 19 || var == 20 || var == 21 || var == 23 || var == 26 || var == 22)
        {
            var = 3;
        }
        else
        {
            var = right;
        }
    }
    if (token.token_type == NOT)
    {
        input.UngetToken(token);
        var = parse_unary_operator();
        var = parse_expression();
        if (var != 3)
        {
            cout << "TYPE MISMATCH " << token.line_no << " C3" << endl;
            exit(0);
        }
    }
    return var;
}

void compareLine(int line_No, int token_Type)
{
    Table *temp = symbol_table;
    while (temp->next != NULL)
    {
        if (temp->item->line_number == line_No)
        {
            temp->item->type = token_Type;
        }
        temp = temp->next;
    }
    if (temp->item->line_number == line_No)
    {
        temp->item->type = token_Type;
    }
}

void update_types(int currentType, int newType)
{
    Table *temp = symbol_table;

    while (temp->next != NULL)
    {
        if (temp->item->type == currentType)
        {
            temp->item->type = newType;
        }
        temp = temp->next;
    }
    if (temp->item->type == currentType)
    {
        temp->item->type = newType;
    }
}

int parse_id()
{
    int var;
    string name;
    int LHS;
    int RHS;
    token = input.GetToken();
    if (token.token_type == ID)
    {
        LHS = search_list(token.lexeme);
        token = input.GetToken();
        if (token.token_type == EQUAL)
        {
            token = input.GetToken();
            if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM ||
                token.token_type == TRUE || token.token_type == FALSE || token.token_type == PLUS ||
                token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV ||
                token.token_type == LESS || token.token_type == GREATER || token.token_type == GTEQ ||
                token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL ||
                token.token_type == NOT)
            {
                input.UngetToken(token);
                RHS = parse_expression();

                if (LHS <= 3)
                {
                    if (LHS != RHS)
                    {
                        if (LHS <= 3)
                        {
                            cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
                            exit(1);
                        }
                        else
                        {
                            update_types(RHS, LHS);
                            RHS = LHS;
                        }
                    }
                }
                else
                {
                    update_types(LHS, RHS);
                    LHS = RHS;
                }
                token = input.GetToken();
            }
        }
    }
    return 0;
}

int parse_case()
{

    int var;
    token = input.GetToken();
    if (token.token_type == CASE)
    {
        token = input.GetToken();
        if (token.token_type == NUM)
        {
            token = input.GetToken();
            if (token.token_type == COLON)
            {
                var = parse_body();
            }
        }
    }
    return 0;
}

int parse_caselist()
{
    int var;
    token = input.GetToken();
    if (token.token_type == CASE)
    {
        input.UngetToken(token);
        var = parse_case();
        token = input.GetToken();
        if (token.token_type == CASE)
        {
            input.UngetToken(token);
            var = parse_caselist();
        }
        else if (token.token_type == RBRACE)
        {
            input.UngetToken(token);
        }
    }
    return 0;
}

int parse_switch()
{
    int var;
    token = input.GetToken();
    if (token.token_type == SWITCH)
    {
        token = input.GetToken();
        if (token.token_type == LPAREN)
        {
            var = parse_expression();
            if (var <= 3 && var != 1)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C5" << endl;
                exit(0);
            }
            token = input.GetToken();
            if (token.token_type == RPAREN)
            {
                token = input.GetToken();
                if (token.token_type == LBRACE)
                {
                    var = parse_caselist();
                    token = input.GetToken();
                }
            }
        }
    }
    return 0;
}

int parse_while()
{
    int var;

    token = input.GetToken();
    if (token.token_type == WHILE)
    {
        token = input.GetToken();
        if (token.token_type == LPAREN)
        {
            var = parse_expression();
            if (var == 4 || var == 5)
            {
                var = 3;
            }
            if (var != 3)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(0);
            }
            token = input.GetToken();
            if (token.token_type == RPAREN)
            {
                var = parse_body();
            }
        }
    }
    return 0;
}

int parse_if()
{
    int var;
    token = input.GetToken();
    if (token.token_type == IF)
    {
        token = input.GetToken();
        if (token.token_type == LPAREN)
        {
            var = parse_expression();
            if (var != 3)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(0);
            }
            token = input.GetToken();
            if (token.token_type == RPAREN)
            {
                var = parse_body();
            }
        }
    }
    return 0;
}

int parse_statement()
{
    int var;
    token = input.GetToken();
    if (token.token_type == ID)
    {
        input.UngetToken(token);
        var = parse_id();
    }
    else if (token.token_type == IF)
    {
        input.UngetToken(token);
        var = parse_if();
    }
    else if (token.token_type == WHILE)
    {
        input.UngetToken(token);
        var = parse_while();
    }
    else if (token.token_type == SWITCH)
    {
        input.UngetToken(token);
        var = parse_switch();
    }
    return 0;
}

int parse_statement_list()
{
    int var;
    token = input.GetToken();
    if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE ||
        token.token_type == SWITCH)
    {
        input.UngetToken(token);
        var = parse_statement();
        token = input.GetToken();
        if (token.token_type == RBRACE)
        {
            input.UngetToken(token);
        }
        else if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE ||
                 token.token_type == SWITCH)
        {
            input.UngetToken(token);
            var = parse_statement_list();
        }
    }
    return 0;
}

int parse_body()
{
    int var;
    token = input.GetToken();
    if (token.token_type == END_OF_FILE)
    {
        input.UngetToken(token);
        return 0;
    }
    else if (token.token_type == LBRACE)
    {
        var = parse_statement_list();
        token = input.GetToken();
        if (token.token_type == RBRACE)
        {
            return 0;
        }
    }
    return 0;
}

int parse_typename()
{
    token = input.GetToken();
    if (token.token_type == INT || token.token_type == REAL || token.token_type == BOOLEAN)
    {
        compareLine(token.line_no, token.token_type);
    }
    return (token.token_type);
}

int parse_variable_declaration()
{
    int var;
    token = input.GetToken();
    if (token.token_type == ID)
    {
        input.UngetToken(token);
        var = parse_variable_list();
        token = input.GetToken();
        if (token.token_type == COLON)
        {
            var = parse_typename();
            token = input.GetToken();
        }
    }
    return 0;
}

int parse_variable_declaration_list()
{
    int var;
    token = input.GetToken();
    while (token.token_type == ID)
    {
        input.UngetToken(token);
        var = parse_variable_declaration();
        token = input.GetToken();
    }
    input.UngetToken(token);
    return 0;
}

int parse_global()
{
    int var;
    token = input.GetToken();
    if (token.token_type == ID)
    {
        input.UngetToken(token);
        var = parse_variable_declaration_list();
    }
    return 0;
}

int parse_program()
{
    int var;
    token = input.GetToken();
    if (token.token_type == END_OF_FILE)
    {
        return 0;
    }
    else
    {
        while (token.token_type != END_OF_FILE)
        {
            if (token.token_type == LBRACE)
            {
                input.UngetToken(token);
                var = parse_body();
            }
            else if (token.token_type == ID)
            {
                input.UngetToken(token);
                var = parse_global();
                var = parse_body();
            }
            else if (token.token_type == END_OF_FILE)
            {
                return 0;
            }
            token = input.GetToken();
        }
    }
}

void print_list()
{
    Table *temp = symbol_table;
    int curr;

    while (temp->next != NULL)
    {
        if (temp->item->type > 3 && temp->item->printed == 0)
        {
            curr = temp->item->type;
            output += temp->item->name;
            temp->item->printed = 1;
            while (temp->next != NULL)
            {
                temp = temp->next;
                if (temp->item->type == curr)
                {
                    output += ", " + temp->item->name;
                    temp->item->printed = 1;
                }
            }
            output += ": ? #";
            cout << output << endl;
            temp->item->printed = 1;
            output = "";
            temp = symbol_table;
        }
        else if (temp->item->type < 4 && temp->item->printed == 0)
        {
            string lCase = keyword[(temp->item->type) - 1];
            int curr = temp->item->type;
            output = temp->item->name + ": " + lCase + " #";
            cout << output << endl;
            output = "";
            temp->item->printed = 1;

            while (temp->next != NULL && temp->next->item->type == curr)
            {
                temp = temp->next;
                string left_case = keyword[(temp->item->type) - 1];
                output = temp->item->name + ": " + left_case + " #";
                cout << output << endl;
                temp->item->printed = 1;
                output = "";
            }
        }
        else
        {
            temp = temp->next;
        }
    }
    if (temp->item->type <= 3 && temp->item->printed == 0)
    {
        string left_case = keyword[(temp->item->type) - 1];
        output += temp->item->name + ": " + left_case + " #";
        cout << output << endl;
        output = "";
    }
    else if (temp->item->type > 3 && temp->item->printed == 0)
    {
        output += temp->item->name + ":" + " ? " + "#";
        cout << output << endl;
        output = "";
    }
}

int main()
{
    parse_program();
    print_list();
}
