#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dia(int year, int diaDef, int* pmonth, int* pday){
    static char daytab[2][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                                 {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
    
    int salto = saltoFunction(year);

    if (year < 1 || diaDef <= 0 || diaDef >((salto == 0) ? 365 : 366)){
        printf("Fecha no valida\n");
        return;
    }

    int sumatoriaDias = 0;
    int i = 0;
    int tmpSumatoria = 0;
    while (sumatoriaDias < diaDef){
        tmpSumatoria = sumatoriaDias;
        sumatoriaDias += daytab[salto][i];
        ++i;
    }

    *pmonth = --i;
    *pday = diaDef - tmpSumatoria;
}

int saltoFunction(int year) {
   return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

char* mes(int n) {
    static char* name[] = {
    "N/A",                           
    "Jan", 
    "Feb", 
    "Mar",                         
    "Apr", 
    "May", 
    "Jun",                          
    "Jul", 
    "Aug", 
    "Sep",                          
    "Oct", 
    "Nov", 
    "Dec"};

    if (n < 1 || n > 12) {
        return name[0];
    }
    else {
        return name[n];
    }
    
}

int main(int argc, char* argv[]) {

    if (argc != 3){
        printf("Formato esperado -> ./month_day <year> <yearday>\n");
    }
    else {
        int year = atoi(argv[1]);
        int day_of_year = atoi(argv[2]);
        int* pmonth = malloc(sizeof(int));
        int* pday = malloc(sizeof(int));
        dia(year, day_of_year, pmonth, pday);
        char* month = mes(*pmonth);

        if (strcmp(month, "N/A") != 0){
            printf("%s %d, %d \n", mes(*pmonth), *pday, year);
        }
    }
    return 0;
}
