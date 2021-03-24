#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLMS 35 /* can change the buffer size as well */
#define ROWS 100 /* change to accomodate other sizes, change ONCE here */

#define MAX 35
#define SIZE 100

int a[120];
int b[120];

int numcmp(char* s1, char* s2) {
    return atof(s1) - atof(s2);
}


void merging(int low, int mid, int high) {
    int l1, l2, i;

    for (l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
        if (a[l1] <= a[l2])
            b[i] = a[l1++];
        else
            b[i] = a[l2++];
    }

    while (l1 <= mid)
        b[i++] = a[l1++];

    while (l2 <= high)
        b[i++] = a[l2++];

    for (i = low; i <= high; i++)
        a[i] = b[i];
}

void sort(int low, int high) {
    int mid;

    if (low < high) {
        mid = (low + high) / 2;
        sort(low, mid);
        sort(mid + 1, high);
        merging(low, mid, high);
    }
    else {
        return;
    }
}


/*void merge(char* lineptr[], int l, int m, int r, int (comp)(void, void*))
{
    int i = 0,
        j = 0,
        k = 0,
        s1 = m - l + 1,
        s2 = r - m;

    void* left[s1],
        * right[s2];

    for (i = 0; i < s1; i++)
    {
        left[i] = lineptr[l + i];
    }
    for (i = 0; i < s2; i++)
    {
        right[i] = lineptr[m + i + 1];
    }
    i = j = 0;
    k = l;
    while (i < s1 && j < s2) {
        if (comp(left[i], right[j]) < 0)
        {
            lineptr[k++] = left[i++];
        }
        else
        {
            lineptr[k++] = right[j++];
        }
    }
    if (i == s1)
    {
        while (j < s2)
        {
            lineptr[k++] = right[j++];
        }
    }
    else {
        while (i < s1) {
            lineptr[k++] = left[i++];
        }
    }
}

void mergeSort(char* lineptr[], int start, int end, int (comp)(void, void*))
{
    int middle = start + (end - start) / 2;
    if (end > start) {
        mergeSort(lineptr, start, middle, comp);
        mergeSort(lineptr, middle + 1, end, comp);
        merge(lineptr, start, middle, end, comp);
    }
}*/

/*void writeOut(char* file, int flag)
{
    int arraySize = 100;
    char** arrayList = malloc(sizeof(char*) * arraySize);

    printf("%s\n", file);
    FILE* fp = fopen(file, "r");
    if (!fp) {
        printf("Error, Failed to open the file! \n");
    }

    char* line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size;

    line_size = getline(&line_buf, &line_buf_size, fp);
    while (line_size >= 0)
    {
        arrayList[line_count] = malloc(sizeof(char) * line_buf_size);
        sprintf(arrayList[line_count], "%s", line_buf);
        arrayList[line_count][strlen(line_buf) - 1] = 0;
        line_count++;
        line_size = getline(&line_buf, &line_buf_size, fp);
    }

    free(line_buf);
    fclose(fp);

    if (flag == 0) {
        mergeSort(arrayList, 0, arraySize - 1, (int ()(void, void*)) strcmp);
    }
    else if (flag == 1) {
        mergeSort(arrayList, 0, arraySize - 1, (int ()(void, void*)) numcmp);
    }

    FILE* fpOut;
    fpOut = fopen((flag ? "sorted_numbers.txt" : "sorted_strings.txt"), "w");
    if (!fpOut) {
        printf("Error, Failed to open the file! \n");
    }

    for (int i = 0; i < 100; i++)
    {
        fprintf(fpOut, "%s\n", arrayList[i]);
    }
    fclose(fpOut);
}*/

/*void sortStrings() {

    char** line = malloc(sizeof(char*) * SIZE);
    FILE* myFile;
    FILE* fp;
    int i;
    int total = 0;
    char tmp[35];


    myFile = fopen("strings.txt", "r");
    for (i = 0; i < SIZE; i++)
    {
        line[i] = malloc(sizeof(char) * MAX);
        fgets(line[i], 34, myFile);
        line[strcspn(line, "\n")] = 0;
        
    }
    
   /* while (fgets(line[i], COLMS, myFile)) {

        strcpy(tmp, line[i]);
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
*/


    //strcpy(line[i-1], tmp);
    //line[i][strlen(line[i])] = '\0';
/*

    total = i ;
    for (i = 0; i < total; ++i) {
        printf("%s\n", line[i]);
        //fprintf(fp, "%s\n", line[i]);
    }
    MergeSort(line, 0, total - 1);
    printf("\nThe Sorted Array is: ");
    for (i = 0; i < total; i++)printf("%s ->", line[i]);

    fp = fopen("sorted_strings.txt", "w");
    for (i = 0; i < total; ++i) {
        printf("%s\n", line[i]);
        fprintf(fp, "%s\n", line[i]);
    }
        
    return 0;
}*/

void sortNumbers() {

    int i;
    FILE* myFile;
    FILE* fp;

    myFile = fopen("numbers.txt", "r");
    for (i = 0; fscanf(myFile, "%d", &a[i]) != EOF; i++) {
    }
    i--;

    sort(0, i);

    fp = fopen("sorted_numbers.txt", "w");
    for (int j = 0; j <= i; j++) {
        fprintf(fp, "%d\n", a[j]);
    }

    fclose(fp);
    fclose(myFile);
}

int main(int argc, char *argv[]) {

        if (argc == 2 && strstr(argv[1],".txt")!=NULL)
    {

    }
    else if(argc == 3 && strcmp(argv[1], "-n")==0 && strstr(argv[2],".txt")!=NULL)
    {
       sortNumbers();
    }
    else 
    {
        printf("Expected:\n");
        printf("$ ./generic_merge_sort.o -n numbers.txt\n");
        printf("$ ./generic_merge_sort.o strings.txt\n");
        return 0;
    }
}