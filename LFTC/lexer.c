#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "lexer.h"
#include "utils.h"

Token *tokens;	// single linked list of tokens
Token *lastTk;		// the last token in list

int line=1;		// the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code)
{
	Token *tk=safeAlloc(sizeof(Token));
	tk->code=code;
	tk->line=line;
	tk->next=NULL;
	if(lastTk){
		lastTk->next=tk;
		}else{
		tokens=tk;
		}
	lastTk=tk;
	return tk;
}

char *extract(const char *begin,const char *end) //extrage intr o zona alocata dinamic noua tot subsirul dintre cei doi pointeri
{
	int len;
	char* extraction;
	len = end - begin;
	extraction = safeAlloc(len+1);
	/*if (extraction == NULL)
	{
		printf("Error at allocationg memory");
		return 1;
	}*/
	strncpy(extraction, begin, len);
	extraction[len]='\0';
	//free(extraction);
	return extraction;

}

Token *tokenize(const char *pch){
	const char *start;
	Token *tk;
	for(;;){
		switch(*pch){
			case ' ':
            case '\t':
			    pch++;
			    break;

			case '\r':		// handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
				if(pch[1]=='\n')
                {
                    pch++;
                }
				// fallthrough to \n
			case '\n':
				line++;
				pch++;
				break;

			case '\0':
			    addTk(END);
                return tokens;

			case ',':
			    addTk(COMMA);
			    pch++;
			    break;

            case ';':
                addTk(SEMICOLON);
			    pch++;
			    break;

            case '(':
                addTk(LPAR);
			    pch++;
			    break;

            case ')':
                addTk(RPAR);
			    pch++;
			    break;

             case '[':
                addTk(LBRACKET);
			    pch++;
			    break;

             case ']':
                addTk(RBRACKET);
			    pch++;
			    break;

             case '{':
                addTk(LACC);
			    pch++;
			    break;

              case '}':
                addTk(RACC);
			    pch++;
			    break;

            case '+':
                addTk(ADD);
			    pch++;
			    break;

            case '-':
                addTk(SUB);
			    pch++;
			    break;

            case '*':
                addTk(MUL);
			    pch++;
			    break;

            case '/':
                if(pch[1]!='/')
                {
                   addTk(DIV);
                   pch++;
                }
                else if(pch[1]=='/')
                {
                   while(*pch!='\n' && *pch!='\0')
                   {
                      pch++;
                   }
                }
			    break;

            case '.':
                addTk(DOT);
			    pch++;
			    break;

            case '&':
                if(pch[1]=='&')
                {
                   addTk(AND);
                   pch+=2;
                }
			    break;

            case '|':
                if(pch[1]=='|')
                {
                   addTk(OR);
                   pch+=2;
                }
			    break;

             case '!':
                if(pch[1]!='=')
                {
                   addTk(NOT);
                   pch++;
                }
                else
                {
                   addTk(NOTEQ);
                   pch+=2;
                }
			    break;

			case '=':
				if(pch[1]=='=')
                    {
                        addTk(EQUAL);
                        pch+=2;
					}
                else
					{
                        addTk(ASSIGN);
                        pch++;
					}
				break;

            case '<':
				if(pch[1]=='=')
                    {
                        addTk(LESSEQ);
                        pch+=2;
					}
                else
					{
                        addTk(LESS);
                        pch++;
					}
				break;

            case '>':
				if(pch[1]=='=')
                    {
                        addTk(GREATEREQ);
                        pch+=2;
					}
                else
					{
                        addTk(GREATER);
                        pch++;
					}
				break;

			default:
				if(isalpha(*pch)||*pch=='_') // cazul pentru cuvinte cheie
                    {
                        for(start=pch++;isalnum(*pch)||*pch=='_';pch++)
                            {}

                        char *text=extract(start,pch);
                        if(strcmp(text,"char")==0)
                        {
                            addTk(TYPE_CHAR);
                        }

                        else if(strcmp(text,"double")==0)
                        {
                            addTk(TYPE_DOUBLE);
                        }

                        else if(strcmp(text,"else")==0)
                        {
                            addTk(ELSE);
                        }

                        else if(strcmp(text,"if")==0)
                        {
                            addTk(IF);
                        }

                        else if(strcmp(text,"int")==0)
                        {
                            addTk(TYPE_INT);
                        }

                        else if(strcmp(text,"return")==0)
                        {
                            addTk(RETURN);
                        }

                        else if(strcmp(text,"struct")==0)
                        {
                            addTk(STRUCT);
                        }

                        else if(strcmp(text,"void")==0)
                        {
                            addTk(VOID);
                        }

                        else if(strcmp(text,"while")==0)
                        {
                            addTk(WHILE);
                        }
                        else
                        {
                            tk=addTk(ID);
                            tk->text=text;
                        }
					}
                else if (isdigit(*pch)) //cazul pt numere
                {
                    int ok_dot=0,ok_e=0;
                    char *ptr;
                    for(start=pch++;isalnum(*pch) || *pch=='.' || *pch=='_' || *pch=='e' || *pch=='E' || *pch=='+' || *pch=='-';pch++)
                    {
                        if(*pch=='E' || *pch=='e')
                        {
                            ok_e=1;
                        }

                        if(*pch=='.')
                        {
                            ok_dot=1;
                        }

                    }

                     char *text=extract(start,pch);

                        if(ok_e==1 || ok_dot==1)
                        {
                            tk=addTk(DOUBLE);//e double
                            tk->d=atof(text); //convertim textul in double
                            printf("%s#%g\n",text,tk->d);
                        }
                        else
                        {
                            tk=addTk(INT); //e int
                            tk->i=atoi(text); // convertim textul in int
                        }
                }
                else if(*pch=='"' || *pch=='\'') // cazul pentru caractere, siruri de caractere, comment-uri
                {
                    int ok_quote=0;
                    pch++;
                    for(start=pch++;isalnum(*pch) || *pch == '"' || *pch == '\'';pch++)
                    {
                        if (*pch == '\'')
                        {
                            ok_quote = 1;
                        }
                    }

                    if(ok_quote)
                    {
                        char *ca=extract(start,pch-1);
                        tk=addTk(CHAR);
                        tk->c=*ca;
                    }
                    else
                    {
                        char *txt=extract(start,pch-1);
                        tk=addTk(STRING);
                        tk->text=txt;
                    }
                }
				else
                {
                    err("invalid char: %c (%d)",*pch,*pch);
                }
			}
		}
	}

void showTokens(const Token *tokens) //tre sa arate linia numele atomului si daca e cazul din ce caract e constituit
{
	for(const Token *tk=tokens;tk;tk=tk->next)
        {
            printf("%d  ",tk->line);
            switch(tk->code)
            {
                //ID
                case ID: printf("ID:%s\n",tk->text);break;

                //Cuvinte cheie
                case TYPE_CHAR: printf("TYPE_CHAR\n");break;
                case TYPE_INT: printf("TYPE_INT\n");break;
                case TYPE_DOUBLE: printf("TYPE_DOUBLE\n");break;
                case ELSE: printf("ELSE\n");break;
                case IF: printf("IF\n");break;
                case WHILE: printf("WHILE\n");break;
                case RETURN: printf("RETURN\n");break;
                case STRUCT: printf("STRUCT\n");break;
                case VOID: printf("VOID\n");break;

                //constante
                case INT: printf("INT:%d\n",tk->i);break;
                case DOUBLE: printf("DOUBLE:%g\n",tk->d);break;
                case CHAR: printf("CHAR:%c\n",tk->c);break;
                case STRING: printf("STRING:%s\n",tk->text);break;

                //delimitatori
                case COMMA: printf("COMMA\n");break;
                case SEMICOLON: printf("SEMICOLON\n");break;
                case LPAR: printf("LPAR\n");break;
                case RPAR: printf("RPAR\n");break;
                case LBRACKET: printf("LBRACKET\n");break;
                case RBRACKET: printf("RBRACKET\n");break;
                case LACC: printf("LACC\n");break;
                case RACC: printf("RACC\n");break;
                case END: printf("END\n");break;

                //Operatori
                case ADD: printf("ADD\n");break;
                case SUB: printf("SUB\n");break;
                case MUL: printf("MUL\n");break;
                case DIV: printf("DIV\n");break;
                case DOT: printf("DOT\n");break;
                case AND: printf("AND\n");break;
                case OR: printf("OR\n");break;
                case NOT: printf("NOT\n");break;
                case ASSIGN: printf("ASSIGN\n");break;
                case EQUAL: printf("EQUAL\n");break;
                case NOTEQ: printf("NOTEQ\n");break;
                case LESS: printf("LESS\n");break;
                case LESSEQ: printf("LESSEQ\n");break;
                case GREATER: printf("GREATER\n");break;
                case GREATEREQ: printf("GREATEREQ\n");break;

            default:break;
            }
		}
}
