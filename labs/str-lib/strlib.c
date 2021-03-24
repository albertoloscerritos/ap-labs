#include <stdlib.h>

int mystrlen(char *str){
    int count = 0;
    while (str[count] != '\0')
    {
        count++;
    }
    return count;
}

char *mystradd(char *dest, char *addition){
    int count       = mystrlen(dest),
        additionlen = mystrlen(addition);
    for(int i = 0; i < additionlen; i++)
    {
        *(dest+count++) = *(addition+i);
    }
    *(dest+count) = '\0';
    return dest;
}

int mystrfind(char *dest, char *substr){
    int count       = 0,
        limit       = mystrlen(substr),
        destlen   = mystrlen(dest);
    for(int i = 0; i < destlen; i++)
    {
        if(*(dest+i) == *(substr+count))
        {
            count++;
        }
        else 
        {
            i = i - count + 1;
            count = 0;
        }
        if(count==limit){
            return i - count + 1;
        }
    }
    return -1;
}