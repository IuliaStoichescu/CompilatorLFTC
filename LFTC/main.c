#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"
int main()
{
    Token *t,*t2;
    char *file,*file2;

    char *path="testlex.c";
    char *path2="testparser.c";

    file=safeAlloc(strlen(path));
    sprintf(file,"%s",path);
    file2=safeAlloc(strlen(path2));
    sprintf(file2,"%s",path2);

    char *buf=loadFile(file);
    char *buf2=loadFile(file2);
    //puts(buf);
    t=tokenize(buf);
    showTokens(t);

    t2=tokenize(buf2);
    parse(t2);

    free(buf);
    free(buf2);
    return 0;
}
