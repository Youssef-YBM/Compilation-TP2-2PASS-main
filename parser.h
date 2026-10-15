#include <stdio.h>
#include<stdlib.h>

// ---------- Unites Lexicales
typedef enum enu
{
	BEGIN, END, READ, WRITE, ID, INTLITERAL, LPAREN, RPAREN,
	SEMICOLON, COMMA, ASSIGNOP, PLUSOP, MINUSOP, SCANEOF,FLOATLITERAL
}token;

char tokens[15][13];

void system_goal();
void program();
void match(token t);
token next_token();
void syntax_error(token t );
void inst_list();
void inst();
void prim();
void id_list();
void expression();
void expr_list();
int add_op();




