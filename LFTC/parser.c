#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"

Token *iTk;		// the iterator in the tokens list
Token *consumedTk;		// the last consumed token

bool unit();
bool structDef();
bool varDef();
bool typeBase();
bool arrayDecl();
bool fnDef();
bool fnParam();
bool stm();
bool stmCompound();
bool expr();
bool exprAssign();
bool exprOr();
bool exprOrPrim();
bool exprAnd();
bool exprAndPrim();
bool exprEq();
bool exprEqPrim();
bool exprRel();
bool exprRelPrim();
bool exprAdd();
bool exprAddPrim();
bool exprMul();
bool exprMulPrim();
bool exprCast();
bool exprUnary();
bool exprPostfix();
bool exprPostfixPrim();
bool exprPrimary();

void tkerr(const char *fmt,...){
	fprintf(stderr,"error in line %d: ",iTk->line);
	va_list va;
	va_start(va,fmt);
	vfprintf(stderr,fmt,va);
	va_end(va);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);
	}

bool consume(int code)//mereu pt atomii lexicali
{
	if(iTk->code==code)
    {
		consumedTk=iTk;
		iTk=iTk->next;
		return true;
    }
	return false;
}

// unit: ( structDef | fnDef | varDef )* END
bool unit() //fiecare regula va fi implementata cu functia ei
{
	for(;;)
    {
		if(structDef()){} //fiecare regula consuma toti atomii din care e constituit =>> va fi true
		else if(fnDef()){}
		else if(varDef()){}
		else break;
    }
	if(consume(END)){
		return true; //true daca regula a fost indeplinita si false otherwise
		}else tkerr("Missing end of file\n");
	return false;
}

//STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef()
{
    Token *startToken=iTk;
    if(consume(STRUCT))
    {
        if(consume(ID))
        {
            if(consume(LACC))
            {
                for(;;)
                {
                     if(varDef())
                    {
                        //return true;
                    }
                    else break;
                }

                if(consume(RACC))
                    {
                        if(consume(SEMICOLON))
                        {
                            return true;
                        }
                        else tkerr("Expected semicolon after end of structure\n");
                    }
                    else tkerr("Expected RACC after declaring fields in the struct\n");//incercam sa consumam varDef u
            }iTk=startToken;
            //else {tkerr("Expected LACC before declaring fields in the struct"); iTk=startToken;}
        }else tkerr("Expected name of the structure");
    }
    iTk=startToken;
    return false;
}

//typeBase ID arrayDecl? SEMICOLON
bool varDef()
{
    Token *startToken=iTk;
    if(typeBase())
    {
        if(consume(ID))
        {
            if (arrayDecl()){}
            while(1)
            {
              if(consume(COMMA))
                {
					if(consume(ID))
					{
						if(arrayDecl()){}
					}
					else tkerr("Missing identifier\n");
				}
				else break;
            }
            if(consume(SEMICOLON))
            {
                return true;
            }else tkerr("Expected semicolon after declaring an array\n");

        }else tkerr("Expected variable name\n");
    }//else tkerr("Invalid type name");
    iTk=startToken;
    return false;
}

// typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase()
{
    Token *startToken=iTk;
	if(consume(TYPE_INT))
    {
        return true;
    }

	if(consume(TYPE_DOUBLE))
	{
		return true;
    }

	if(consume(TYPE_CHAR))
    {
		return true;
    }

	if(consume(STRUCT))
    {
		if(consume(ID))
        {
			return true;
        }else tkerr("Missing name of the struct");
    }
    iTk=startToken;
	return false;
}

//LBRACKET INT? RBRACKET
bool arrayDecl()
{
    Token *startToken=iTk; // se salvează poziția inițială a iteratorului
    if(consume(LBRACKET))
    {
      if(expr()){}
            if(consume(RBRACKET))
            {
                return true;
            }else tkerr("Missing RBRACKET after array declaration\n");
    }
    iTk=startToken;
    return false;
}

//( typeBase | VOID ) ID LPAR ( fnParam ( COMMA fnParam )* )? RPAR stmCompound
bool fnDef()
{
    Token *startToken=iTk;
    if(typeBase())
    {
        if(consume(MUL)){}//poate fi pointer
        if(consume(ID))
        {
            if(consume(LPAR))
            {
                if(fnParam())
                {//optional
                    for(;;)
                    {
                      if(consume(COMMA))
                      {
                          if(fnParam())
                          {

                          }
                          else tkerr("Missing argument after ,\n");
                      }
                      else break;
                    }
                 }
                if(consume(RPAR))
                    {
                        if(stmCompound())
                            {
                                return true;
                            }
                        else tkerr("Missing function body\n");

                    }else tkerr("Missing right paranthesis after function parameter declaration\n");
             }else iTk=startToken;//else tkerr("Missing left paranthesis after name of function");
        }else tkerr("Missing name of function\n");
    }

    else if(consume(VOID))
    {
        if(consume(ID))
        {
            if(consume(LPAR))
            {
                if(fnParam())
                {//optional
                    for(;;)
                    {
                      if(consume(COMMA))
                      {
                          if(fnParam())
                          {

                          }
                          else tkerr("Missing argument after ,\n");
                      }
                      else break;
                    }
                 }
                if(consume(RPAR))
                    {
                        if(stmCompound())
                            {
                                return true;
                            }
                        else tkerr("Missing function body.\n");

                    }else tkerr("Missing right paranthesis after function parameter declaration\n");
             }else tkerr("Missing left paranthesis after name of function");
        }else tkerr("Missing name of function\n");
    }
    iTk=startToken;
    return false;
}

//typeBase ID arrayDecl?
bool fnParam()
{
    Token *start=iTk; // se salvează poziția inițială a iteratorului
    if(typeBase())
    {
        if(consume(ID))
        {
            if(arrayDecl()){}
            return true;
        }else ("Missing ID after declaration of variable type\n");
    }
    // se reface poziția inițială a iteratorului, în caz că unele if-uri exterioare au consumat atomi
    return false;
}

/*stmCompound
| IF LPAR expr RPAR stm ( ELSE stm )?
| WHILE LPAR expr RPAR stm
| RETURN expr? SEMICOLON
| expr? SEMICOLON
*/

bool stm()
{
    if(stmCompound())
    {
        return true;
    }
//| IF LPAR expr RPAR stm ( ELSE stm )?
    if(consume(IF))
    {
        if(consume(LPAR))
        {
            if(expr())
            {
                if(consume(RPAR))
                {
                    if(stm())
                    {
                        if(consume(ELSE))
                        {
                            if(stm())
                            {
                                //return true;
                            }else tkerr("Expected ELSE expression\n");
                        }
                        return true;
                    }else tkerr("Missing expression\n");
                } else tkerr("Expected right paranthesis after end of expression in if\n");
            }else tkerr("Expected if expression\n");
        }else tkerr("Expected left paranthesis after if\n");
    }

    //| WHILE LPAR expr RPAR stm

    if(consume(WHILE))
    {
        if(consume(LPAR))
        {
            if(expr())
            {
                if(consume(RPAR))
                {
                    if(stm())
                    {
                        return true;
                    }else tkerr("Expected while body\n");
                }else tkerr("Expected right paranthesis after condition in while\n");
            }else tkerr("Missing while condition\n");
        }else tkerr("Expected left paranthesis after while\n");
    }

    //| RETURN expr? SEMICOLON
     if(consume(RETURN))
     {
       if(expr()){}
       if(consume(SEMICOLON))
           {
               return true;
           }else tkerr("Expected semicolon after return\n");
     }

     //| expr? SEMICOLON
     if(expr())
     {
        if(consume(SEMICOLON))
           {
               return true;
           }else tkerr("Expected semicolon after expression\n");
     }
     if(consume(SEMICOLON))
           {
               return true;
           }//else tkerr("Expected semicolon at the end\n");

    return false;
}

//LACC ( varDef | stm )* RACC
bool stmCompound()
{
    if(consume(LACC))
    {
        while(1)
        {
          if(varDef()){}
          else if(stm()){}
          else break;
        }
        if(consume(RACC))
           {
               return true;
           }else tkerr("Missing right accolade after end of function\n");
    }
    return false;
}

//expr: exprAssign
bool expr()
{
    if(exprAssign())
    {
        return true;
    }
    return false;
}

//exprAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign()
{
   Token *startToken=iTk;
   if(exprUnary())
   {
       if(consume(ASSIGN))
       {
           if(exprAssign())
           {
               return true;
           }else tkerr("Missing right part of assign\n");
       }iTk=startToken;//else tkerr("Mising variable assignation\n");
   }
   iTk=startToken;
   if(exprOr())
   {
       return 1;
   }
   iTk=startToken;
   return false;
}


//exprOrPrim: OR exprAnd exprOrPrim | ε
bool exprOrPrim()
{
   Token* startToken=iTk;
   if(consume(OR))
   {
       if(exprAnd())
       {
           if(exprOrPrim())
           {
               return true;
           }
       }
   }
   iTk=startToken;
   return true;
}

//exprOr: exprOr OR exprAnd | exprAnd
//exprOr: exprAnd exprOrPrim
bool exprOr()
{
   Token* startToken=iTk;
   if(exprAnd())
   {
       if(exprOrPrim())
       {
           return true;
       }
   }
   iTk=startToken;
   return false;
}

bool exprAndPrim()
{
    Token* startToken=iTk;
    if(consume(AND))
    {
        if(exprEq())
        {
            if(exprAndPrim())
            {
                return true;
            }
        }
    }
    iTk=startToken;
    return true;
}
//exprAnd: exprAnd AND exprEq | exprEq
bool exprAnd()
{
    Token* startToken=iTk;
 if(exprEq())
 {
     if(exprAndPrim())
     {
         return true;
     }
 }
 iTk=startToken;
 return false;
}

bool exprEqPrim()
{
    Token* startToken=iTk;
    if(consume(EQUAL) || consume(NOTEQ))
    {
        if(exprRel())
        {
            if(exprEqPrim())
            {
                return true;
            }
        }
    }
    iTk=startToken;
    return true;
}
//exprEq: exprEq ( EQUAL | NOTEQ ) exprRel | exprRel
bool exprEq()
{
    Token* startToken=iTk;
    if(exprRel())
    {
        if(exprRelPrim())
        {
            return true;
        }
    }
    iTk=startToken;
    return false;
}

bool exprRelPrim()
{
    Token* startToken=iTk;
    if(consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
    {
        if(exprAdd())
        {
            if(exprRelPrim())
            {
                return true;
            }
        }
    }
    iTk=startToken;
    return true;
}

//exprRel: exprRel ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd | exprAdd
bool exprRel()
{
  Token* startToken=iTk;
  if(exprAdd())
  {
      if(exprRelPrim())
      {
          return true;
      }
  }
  iTk=startToken;
  return false;
}

bool exprAddPrim()
{
    Token* startToken=iTk;
    if(consume(ADD) || consume(SUB))
    {
        if(exprMul())
        {
            if(exprAddPrim())
            {
                return true;
            }
        }
    }
    iTk=startToken;
    return true;
}

//exprAdd: exprAdd ( ADD | SUB ) exprMul | exprMul
bool exprAdd()
{
    Token* startToken=iTk;
    if(exprMul())
    {
        if(exprAddPrim())
        {
            return true;
        }
    }
    iTk=startToken;
    return false;
}

bool exprMulPrim()
{
    Token* startToken=iTk;
    if(consume(MUL) || consume(DIV))
    {
        if(exprCast())
        {
            if(exprMulPrim())
            {
                return true;
            }
        }
    }
    iTk=startToken;
    return true;
}

//exprMul: exprMul ( MUL | DIV ) exprCast | exprCast
bool exprMul()
{
    Token* startToken=iTk;
  if(exprCast())
  {
      if(exprMulPrim())
      {
          return true;
      }
  }
  iTk=startToken;
  return false;
}

//exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary
bool exprCast()
{
    Token* startToken=iTk;
  if(consume(LPAR))
  {
      if(typeBase())
      {
          if(arrayDecl()){}
               if(consume(RPAR))
                {
                  if(exprCast())
                  {
                    return true;
                  }
               }//tkerr("Expected right paranthesis after ending cast\n");
      }iTk=startToken;
  }
  if(exprUnary())
  {
      return true;
  }
  iTk=startToken;
  return false;
}

//exprUnary: ( SUB | NOT ) exprUnary | exprPostfix

bool exprUnary()
{
  Token* startToken=iTk;
  if(consume(SUB) || consume(NOT))
  {
    if(exprUnary())
    {
        return true;
    }
  }//else tkerr("Missing - or ! before expression");
  if(exprPostfix())
  {
      return true;
  }

  iTk=startToken;
  return false;
}

/*exprPostfix: exprPostfix LBRACKET expr RBRACKET
| exprPostfix DOT ID
| exprPrimary
*/

bool exprPostfixPrim()
{
    Token* startToken=iTk;
    if(consume(LBRACKET))
        {
            if(expr())
            {
                if(consume(RBRACKET))
                {
                    if(exprPostfixPrim())
                    {
                        return true;
                    }
                } //else tkerr("Expected right bracket after expression");
            }iTk=startToken;//else tkerr("Missing postfix expression");
        }//else tkerr("Expected left bracket after postfix expression");
     if(consume(DOT))
       {
           if(consume(ID))
           {
               if(exprPostfixPrim())
               {
                   return true;
               }
           }iTk=startToken;
       }
    return true;
}

bool exprPostfix()
{
    Token* startToken=iTk;
  if(exprPrimary())
  {
      if(exprPostfixPrim())
      {
          return true;
      }
  }
  iTk=startToken;
  return false;
}

/*exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
| INT | DOUBLE | CHAR | STRING | LPAR expr RPAR
*/

bool exprPrimary()
{
   Token* startToken=iTk;
   if(consume(ID))
   {
       if(consume(LPAR))
       {
           if(expr())
           {
               for(;;)
               {
                   if(consume(COMMA))
                   {
                       if(expr())
                       {

                       }else tkerr("Missing expression\n");
                   }
                   else break;
               }
           }//tkerr("Expected left paranthesis before expression");
            if(consume(RPAR))
            {
              return true;
            }//tkerr("Mising right paranthesis after expression");
       }return true;
   }
   if(consume(INT))
   {
       return true;
   }
     if(consume(DOUBLE))
   {
       return true;
   }
    if(consume(CHAR))
   {
       return true;
   }
    if(consume(STRING))
   {
       return true;
   }
    if(consume(INT))
   {
       return true;
   }
    if(consume(LPAR))
   {
       if(expr())
       {
           if(consume(RPAR))
           {
              return true;
           }//else tkerr("Missing right paranthesis after expression");
       }
   }
    iTk=startToken;
    return false;
}

void parse(Token *tokens)
{
	iTk=tokens;//iteratorul in lista de atomi pe care il initializam cu inceputul listei
	if(!unit())tkerr("syntax error");
}
