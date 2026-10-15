#include"parser.h"
FILE* output =NULL;
token courant;
int nb=1;
char c;

char tokens[15][13] = { "BEGIN", "END", "READ", "WRITE", "ID", "INTLITERAL",
								"LPAREN", "RPAREN", "SEMICOLON", "COMMA", "ASSIGNOP",
								"PLUSOP", "MINUSOP", "SCANEOF","FLOATLITERAL"};

void system_goal()
{
  /* (system goal>, ::= <program> SCANEOF */
    output=fopen("sortie_analyseur.txt","r");
    if(!output)
    {
        printf("ERREUR D'OUVERTURE !!!!");
        exit(-1);
    }
    program();
    match(SCANEOF);
    fclose(output);
}

void program()
{
  /*<program>  ::= begin <inst - list> end */
  courant=next_token();
  match(BEGIN);
  inst_list();
  match(END);
}

void match(token t)
{
   token to=courant;
   if(to==t)
   {
      //printf("MATCH \n");
      courant=next_token();
   }
   else
   {
      printf("Found %s ",tokens[to]);
      syntax_error(t);
     //courant=next_token();
   }
}

token next_token()
{
  char a;
  char *token_name=NULL;
  token_name=(char*)malloc(20*sizeof(char));
  fscanf(output,"%s%c%c",token_name,&a,&c);
  if(c!='\n') ungetc(c,output);
  else nb++;
  if(!strcmp(token_name,"BEGIN"))
        return BEGIN;
    if(!strcmp(token_name,"END"))
        return END;
    if(!strcmp(token_name,"READ"))
        return READ;
    if(!strcmp(token_name,"WRITE"))
        return WRITE;
    if(!strcmp(token_name,"ID"))
        return ID;
    if(!strcmp(token_name,"INTLITERAL"))
        return INTLITERAL;
    if(!strcmp(token_name,"LPAREN"))
        return LPAREN;
    if(!strcmp(token_name,"RPAREN"))
        return RPAREN;
    if(!strcmp(token_name,"SEMICOLON"))
        return SEMICOLON;
    if(!strcmp(token_name,"COMMA"))
        return COMMA;
    if(!strcmp(token_name,"ASSIGNOP"))
        return ASSIGNOP;
    if(!strcmp(token_name,"PLUSOP"))
        return PLUSOP;
    if(!strcmp(token_name,"MINUSOP"))
        return MINUSOP;
    if(!strcmp(token_name,"SCANEOF"))
        return SCANEOF;
    if(!strcmp(token_name,"FLOATLITERAL"))
        return FLOATLITERAL;
    return 15;
}

void syntax_error(token t)
{
    int temp;
    if(c=='\n' ) temp=nb-1 ;
    else temp=nb;
    switch(t)
    {
        case -1: printf("Expression manquante ou erreur de syntaxe ligne %d\n",temp);break;
        default : printf("Excpected %s ligne %d\n",tokens[t],temp);
    }
}

void inst_list()
{
  /* <inst-list> ::= <inst> { <inst> } */
  inst();
  while (1)
  {
    switch(courant)
    {
      case ID:
      case READ:
      case WRITE:
        inst();
        break;
      case END:
      case SCANEOF: return;
      default:
          syntax_error(-1);
          courant=next_token();
          break;
    }
  }
}

void inst()
{
  token tok = courant;
  //To put END back into the file
  switch (tok)
  {
    case ID:
      /* <inst> ::= ID := <expr> ; */
      match(ID);
      match(ASSIGNOP);
      expression();
      match(SEMICOLON);
      break;
    case READ:
      /* x <inst> :: = READ(<id list>); */
      match(READ);
      match(LPAREN);
      id_list();
      match(RPAREN);
      match(SEMICOLON);
      break;
    case WRITE:
      /*(inst > ::= WRITE(<expr list>); */
      match(WRITE);
      match(LPAREN);
      expr_list();
      match(RPAREN);
      match(SEMICOLON);
      break;
    default:
      printf("found unexpected %s ",tokens[tok]);
      syntax_error(-1);
      break;
  }
}

void prim()
{
  token tok = courant;
   switch (tok)
  {
    case LPAREN:
      /* <prim> ::= ( expr> ) */
      match(LPAREN);
      expression();
      match(RPAREN);
      break;
    case ID:
      /* <prim> ::= ID */
      match(ID);
      break;
    case INTLITERAL:
      /* <prim> ::= INTLITERAL */
      match(INTLITERAL);
      break;
    case FLOATLITERAL :
        match(FLOATLITERAL);
        break;
     case END: return;
     case SCANEOF : printf("Found %s",tokens[courant]);break;
    default:
       while(courant!=RPAREN&& courant!=COMMA && courant!=READ)
      {
        if(courant==END || courant==SCANEOF)
        {
            printf("Found unexpected %s ",tokens[courant]);break;
        }
        printf("Found %s " , tokens[courant]);
        syntax_error(-1);
        if(courant==SEMICOLON) break;
        else courant=next_token();
      }
      break;
  }

}

void id_list()
{
  /* <id_list> ::= ID { , ID } */
  //match(ID);
  int contin=1;
    while(contin)
    {
        switch(courant)
        {
            case ID : match(ID); contin=0; break;
            case COMMA: syntax_error(4); contin=0;break;
            case RPAREN :syntax_error(4); contin=0 ; break;
            case END:return;
            case SCANEOF : printf("Found %s",tokens[courant]); contin=0;break;
            default:
               printf("Found %s",tokens[courant]);
               courant=next_token();
        }
    }
  while (courant == COMMA)
  {
    match(COMMA);
       if(courant!=ID)
         {
             if(courant==COMMA || courant==RPAREN)
                syntax_error(4);
            else
             {
                 printf("FOUND %s ",tokens[courant]);
                 syntax_error(4);
                courant=next_token();
             }
         }
       else
            match(ID);
  }
}

void expression()
{
  token t=courant;
  /* <expr> ::= <prim> { <add op> <prim> } */
  prim();
  for(t = courant;  t==PLUSOP || t==MINUSOP; t = courant)
  {
    add_op();
    prim();
  }
}


void expr_list()
{
  /* <expr_list> ::= <expr> { , <expr> } */
  if(courant==COMMA ||courant==RPAREN) syntax_error(-1);
  else expression();
   while (courant == COMMA)
  {
    match(COMMA);
    if(courant==COMMA || courant==RPAREN)
        syntax_error(-1);
    else
        expression();
  }
}

int add_op()
{
  /* <add_opp> ::= PLUSOP || MINUSOP */
  token t = courant;
  if (t == PLUSOP || t ==MINUSOP) match(t);
  else syntax_error(t);
}









