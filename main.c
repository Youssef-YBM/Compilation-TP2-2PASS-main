#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"
#define MAX 50



/** Declarer une variable global afin de savoir la ligne de l'erreur**/
   int ligneid=1;

   int newline=0;

/**Declaration du token buffer **/
typedef struct
{
    char token_tab[MAX];
    int indice;
}token_buffer ;

  /**declaration d'un buffer comme var globale **/
  token_buffer buffer;

  /** Fonction pour vider le buffer **/
  void clear_buffer()
  {
      buffer.indice=0;
  }
  /** Fonction pour placer un caracter dans le tab buffer**/
   void buffer_char(int car)
  {
    char c;
    c =(char)car;
    buffer.token_tab[buffer.indice]=c;
    buffer.token_tab[buffer.indice+1]='\0';
    buffer.indice++;
  }

  void to_maj()
{
	for (int i = 0; i < buffer.indice; i++)
		if ((buffer.token_tab[i] <= 'z') && (buffer.token_tab[i] >= 'a'))
			buffer.token_tab[i] = 'A' + buffer.token_tab[i] - 'a';
}

  /** Fonction pour s'assurer que le mot lu n'est pas un mot reservé **/
    token check_reserved()
   {
     to_maj();
    if(!strcmp(buffer.token_tab,"BEGIN")) return BEGIN;
    if(!strcmp(buffer.token_tab,"END")) return END;
    if(!strcmp(buffer.token_tab,"READ")) return READ;
    if(!strcmp(buffer.token_tab,"WRITE")) return WRITE;
    return ID;
   }

  /** Fonction qui affiche dans la console l erreur et sa ligne**/
  void  lexical_error(int sym)
    {
      printf("Erreur a la ligne %d caractere inconnu %c\n",ligneid ,sym);
    }



    void exposant(FILE* myFile, char* c)
{
    buffer_char(c);
    (*c) = fgetc(myFile);
    if ((!isdigit(*c)) && (*c != '+') && (*c != '-'))
        ungetc(*c, myFile);
    else
    {
        if ((*c == '+') || (*c == '-'))
        {
            buffer_char(*c);
            (*c) = fgetc(myFile);
        }
        if (isdigit(*c))
            read_number(myFile, c);
        //for example : when we have e+a to get a back into the file
        else
            ungetc(*c, myFile);
    }
}

void read_number(FILE* myFile, char* c)
{
    for ((*c) = fgetc(myFile); isdigit(*c); (*c) = fgetc(myFile))
        buffer_char(*c);
    ungetc(*c, myFile);
}


token scanner(FILE *inputFile)
{
  int in_char, c=0; ;
  clear_buffer();
  if(feof(inputFile)) return SCANEOF;

  while ((in_char = fgetc(inputFile)) != EOF)
  {
    if(in_char == '\n')
    {
        ligneid++;
        if(!newline) continue ;
        newline=0;
        return -1;
    }
    if (isspace(in_char))  continue;
    if (isalpha(in_char))
    {
      /*ID ::= LETTER | ID LETTER
      *               | ID DIGIT
      *               | ID UNDERSCORE
      */
      buffer_char(in_char);
      for (c = fgetc(inputFile); isalnum(c) || c == '_'; c =fgetc(inputFile))
        buffer_char(c);
      ungetc(c, inputFile );
      return check_reserved();
    }
     else if (isdigit(in_char))
        {
            /* INTLITERAL ::= DIGIT
            *               | INTLITERAL DIGIT
            */
            buffer_char(in_char);
            read_number(inputFile, &c);
            if (c == '.')
            {
                c = fgetc(inputFile);
                read_number(inputFile, &c);
                if ((c != 'e') && (c != 'E'))
                    return FLOATLITERAL;
            }
            if ((c == 'e') || (c == 'E'))
            {
                //fgetc to read the next char after the 'e'/'E'
                c = fgetc(inputFile);
                exposant(inputFile, &c);
                return FLOATLITERAL;
            }
            return INTLITERAL;
        }
        else if (in_char == '.')
        {
            c = fgetc(inputFile);
            if (isdigit(c))
            {
                read_number(inputFile, &c);
                if ((c == 'e') || (c == 'E'))
                {
                    c = fgetc(inputFile);
                    exposant(inputFile, &c);
                }
                return FLOATLITERAL;
            }
            else
            {
                ungetc(c, inputFile);
                lexical_error('.');
            }
        }
    else if (in_char == '(')
      return LPAREN;
    else if (in_char == ')')
      return RPAREN;
    else if (in_char == ';')
      return SEMICOLON;
    else if (in_char == ',')
      return COMMA;
    else if (in_char == '+')
      return PLUSOP;
    else if (in_char == ':') {
      /* look for ":=" */
      c = fgetc(inputFile);
      if (c == '=') return ASSIGNOP;
      else
      {
        ungetc(c,inputFile);
        lexical_error(in_char);
      }
    }
    else if (in_char == '-')
    {
      /* is it -- , comment start */
      c = fgetc(inputFile);
      if (c == '-')
      {

        do in_char = fgetc(inputFile);
        while(in_char != '\n');
        ligneid++;
      }
      else
      {
        ungetc(c,inputFile);
        return MINUSOP;
      }
    }
    else
      lexical_error(in_char);
  }/* while */
  return SCANEOF;
}/* scanner() */

void analyseurlex()
{
    FILE* myFile=NULL;
    FILE* output =NULL;
	int i;
	myFile=fopen("input.txt", "r");
	output=fopen("sortie_analyseur.txt", "w");

	if (!myFile  || !output)
	{
		printf("Error! Could not read one or more files\n");
		exit(-1);
	}

    while (strcmp(tokens[i = scanner(myFile)], "SCANEOF"))
    {
       if(i>=0)
       { fprintf(output, "%s ", tokens[i]);
         newline=1; }
       else fprintf(output,"%c",'\n');
    }
       fprintf(output,"%s",tokens[13]);
    fclose(output);
    fclose(myFile);

}

int main()
{
    analyseurlex();
    printf("Fin analyseur lexical\n");
    system_goal();

    return 0;
}





