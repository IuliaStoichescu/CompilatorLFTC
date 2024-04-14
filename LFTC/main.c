#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"
int main()
{
    Token *t;
    char *file;

    char *path="testparser.c";

    file=safeAlloc(strlen(path));
    sprintf(file,"%s",path);

    char *buf=loadFile(file);
    //puts(buf);
    t=tokenize(buf);
    showTokens(t);

    parse(t);

    free(buf);
    return 0;
}
