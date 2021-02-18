//Alberto Rodriguez
#include <stdio.h>
#include <stdlib.h>

#define   LOWER  0       
#define   UPPER  300     
#define   STEP   20      

int main(int argc, char **argv)
{
    int fahrenheit;

    if(argc == 2){
        fahrenheit = atoi(argv[1]);
        printf("Fahrenheit: %3d, Celsius: %6.1f\n", fahrenheit, (5.0/9.0)*(fahrenheit-32));
    }

    if(argc < 2){
        for (fahrenheit = LOWER; fahrenheit <= UPPER; fahrenheit = fahrenheit + STEP)
	    printf("Fahrenheit: %3d, Celsius: %6.1f\n", fahrenheit, (5.0/9.0)*(fahrenheit-32));
    }

    else{
        int f;       
        
         int inicio = atoi(argv[1]);
         int final = atoi(argv[2]);
         int inc = atoi(argv[3]);

         for(f = inicio; f <= final; f = f + inc){
             printf("Fahrenheit: %3d, Celsius: %6.1f\n", f, (5.0/9.0)*(f-32));
         }

    }

    return 0;
}