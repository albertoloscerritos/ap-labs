#define _CRT_SECURE_NO_WARNINGS
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 250
#define MAX_WORD 50
#define TABLE_SIZE 100
#define MAX_DATE 17

#define ALPM 6
#define PACMAN 8
#define ALPMSCRIPTLET 16

int countALPM = 0;
int countALPMSCRIPTLET = 0;
int countPACMAN = 0;

int installedPackages = 0;
int removedPackages = 0;
int upgradedPackages = 0;

int noUpgrades = 0;

//char * newestPackage; Why this didnt worked
char oldestPackage[MAX_WORD];
char newestPackage[MAX_WORD];

typedef struct entry_t {
    char* key;
    char* value;
    char* installDate;
    int* upgrades;
    char* lastUpgrade;
    char* removalDate;
    struct entry_t* next;
} entry_t;

typedef struct {
    entry_t** entries;
} ht_t;

unsigned int hash(const char* key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    // make sure value is 0 <= value < TABLE_SIZE
    value = value % TABLE_SIZE;

    return value;
}

entry_t* ht_pair(const char* key, const char* value, const char* installDate, const int* upgrades, const char* lastUpgrade, const char* removalDate) {    //--------------
    // allocate the entry
    entry_t* entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);
    entry->installDate = malloc(strlen(installDate) + 1);                             //------------
    entry->upgrades = malloc(5);
    entry->lastUpgrade = malloc(strlen(lastUpgrade) + 1);
    entry->removalDate = malloc(strlen(removalDate) + 5);  //Se bloquea con menos de 5 why??

    // copy the key and value in place
    strcpy(entry->key, key);
    strcpy(entry->value, value);
    strcpy(entry->installDate, installDate);                                                        //-----------
    entry->upgrades = upgrades;
    strcpy(entry->lastUpgrade, lastUpgrade);
    strcpy(entry->removalDate, removalDate);

    // next starts out null but may be set later on
    entry->next = NULL;

    return entry;
}

ht_t* ht_create(void) {
    // allocate table
    ht_t* hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t*) * TABLE_SIZE);

    // set each to null (needed for proper operation)
    int i = 0;
    for (; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t* hashtable, const char* key, const char* value, const char* installDate, const int* upgrades, const char* lastUpgrade, const char* removalDate) {       //----------
    unsigned int slot = hash(key);

    // try to look up an entry set
    entry_t* entry = hashtable->entries[slot];

    // no entry means slot empty, insert immediately
    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value, installDate, upgrades, lastUpgrade, removalDate);                                      //-----------
        return;
    }

    entry_t* prev;

    // walk through each entry until either the end is
    // reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // match found, replace value

            free(entry->value);                                                                         //-------
            entry->value = malloc(strlen(value) + 1);
            strcpy(entry->value, value);

            free(entry->installDate);                                                                         //-------
            entry->installDate = malloc(strlen(installDate) + 1);
            strcpy(entry->installDate, installDate);

            free(entry->upgrades);                                                                         //-------
            entry->upgrades = malloc(strlen(upgrades) + 1);
            strcpy(entry->upgrades, upgrades);

            free(entry->lastUpgrade);                                                                         //-------
            entry->lastUpgrade = malloc(strlen(lastUpgrade) + 1);
            strcpy(entry->lastUpgrade, lastUpgrade);

            free(entry->removalDate);                                                                         //-------
            entry->removalDate = malloc(strlen(removalDate) + 1);
            strcpy(entry->removalDate, removalDate);

            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = ht_pair(key, value, installDate, upgrades, lastUpgrade, removalDate);                                 //-----------
}

entry_t* ht_get(ht_t* hashtable, const char* key) {
    unsigned int slot = hash(key);

    // try to find a valid slot
    entry_t* entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        return NULL;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // return value if found
        if (strcmp(entry->key, key) == 0) {
            return entry;
        }

        // proceed to next key if available
        entry = entry->next;
    }

    // reaching here means there were >= 1 entries but no key match
    return NULL;
}

void ht_del(ht_t* hashtable, const char* key) {
    unsigned int bucket = hash(key);

    // try to find a valid bucket
    entry_t* entry = hashtable->entries[bucket];

    // no bucket means no entry
    if (entry == NULL) {
        return;
    }

    entry_t* prev;
    int idx = 0;

    // walk through each entry until either the end is reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // first item and no next entry
            if (entry->next == NULL && idx == 0) {
                hashtable->entries[bucket] = NULL;
            }

            // first item with a next entry
            if (entry->next != NULL && idx == 0) {
                hashtable->entries[bucket] = entry->next;
            }

            // last item
            if (entry->next == NULL && idx != 0) {
                prev->next = NULL;
            }

            // middle item
            if (entry->next != NULL && idx != 0) {
                prev->next = entry->next;
            }

            // free the deleted entry
            free(entry->key);                                           //---------
            free(entry->value);
            free(entry->installDate);
            free(entry->upgrades);
            free(entry->lastUpgrade);
            free(entry->removalDate);
            free(entry);

            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;

        ++idx;
    }
}

void ht_dump(ht_t* hashtable) {

    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        for (;;) {

            printPackages(entry->key, entry->value, entry->installDate, entry->upgrades, entry->lastUpgrade, entry->removalDate);
            if (entry->next == NULL) {
                break;
            }

            printf("\n");
            entry = entry->next;
        }
    }
}


void readFile() {

    ht_t* ht = ht_create();

    FILE* file = fopen("pacman.txt", "r");
    char input[MAX_LINE];

    fgets(input, 4, file);	//Esto es para desasernos de un caracter raro q aparece al inicio de la primer linea

    while (fgets(input, MAX_LINE, file)) {
        //printf("%s", input);
        splitLines(input, ht);
    }
    fclose(file);

    //ht_dump(ht);
    printCount(ht);
    writeToTXT(ht);
    
}

int splitLines(char* string, ht_t* ht) {

    int pacmanType = 0;
    char* token = strtok(string, "]");	//Separamos la linea por espacios, token recive el primer valor
    char* value;
    char date[MAX_DATE];
    entry_t* tmpGet;
    int tokenLen;

    for (int i = 0; i < 4; i++) {

        if (token == NULL) continue;
        if (i == 0) {
            dateToString(token, date);
        }
        else if (i == 1) {
            tokenLen = strlen(token);
            if (tokenLen == ALPM) {
                countALPM++;
                pacmanType = ALPM;
            }
            else if (tokenLen == ALPMSCRIPTLET) {
                countALPMSCRIPTLET++;
                pacmanType = ALPMSCRIPTLET;
            }
            else if (tokenLen == PACMAN) {
                countPACMAN++;
                pacmanType = PACMAN;
            }
        }
        else if (i == 2) {
            if (strcmp(token, "installed") == 0 || strcmp(token, "upgraded") == 0 || strcmp(token, "removed") == 0) {
                value = token;
            }
            else {
                return 0;
            }
        }
        else if (i == 3) {

            tmpGet = ht_get(ht, token);

            if (installedPackages == 0) {

                strcpy(newestPackage, token);   //Why this didnt worked with the char* array, fue porque no la inicialize??
                strcpy(oldestPackage, token);
            }
            if (tmpGet == NULL) {
                //printf(" %s\n", token);
                ht_set(ht, token, value, date, 0, date, "-");
                installedPackages++;
            }
            else if (strcmp(value, "upgraded") == 0) {
               
                //tmpGet->lastUpgrade = gets(date); ??why this didnt worked?? 
                strcpy(tmpGet->lastUpgrade, date);
                
                int val = (tmpGet->upgrades);
                tmpGet->upgrades = (val / 4) + 1;
                upgradedPackages++;
            }
            else if (strcmp(value, "removed") == 0) {

                strcpy(tmpGet->removalDate, date);
                removedPackages++;
            }
            strcpy(newestPackage, token);
        }

        token = strtok(NULL, " ");
    }
    return 0;
}

printPackageWitNoUpgrades(ht_t* hashtable) {

    printf("- Package with no upgrades     :");
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];
        if (entry == NULL) {
            continue;
        }

        for (;;) {
            if (entry->upgrades == 0) {
                printf("                                %s", entry->key);
                printf("\n");
                noUpgrades++;
            }
            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }
    }
}

writeToTXT(ht_t* hashtable) {

    FILE* fp;
    fp = fopen("packages_report.txt", "w");

    //------------------ Print Count (General Report) ----------------//
    fprintf(fp, "Pacman Packages Report\n");
    fprintf(fp, "----------------------\n");
    fprintf(fp, "- Installed packages : %d\n", installedPackages);
    fprintf(fp, "- Removed packages   : %d\n", removedPackages);
    fprintf(fp, "- Upgraded packages  : %d\n", upgradedPackages);
    fprintf(fp, "- Current installed  : %d\n", (installedPackages - removedPackages));

    fprintf(fp, "-------------\n");
    fprintf(fp, "General Stats\n");
    fprintf(fp, "-------------\n");


    fprintf(fp, "- Oldest package               : %s\n", oldestPackage);
    fprintf(fp, "- Newest package               : %s\n", newestPackage);
    fprintf(fp, "- Package with no upgrades     :");

    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];
        if (entry == NULL) {
            continue;
        }

        for (;;) {
            if (entry->upgrades == 0) {
                fprintf(fp, " %s", entry->key);
                fprintf(fp, " ");
                noUpgrades++;
            }
            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }
    }
    fprintf(fp, "\n");

    fprintf(fp, "- [ALPM-SCRIPTTLET] type count : %d\n", countALPMSCRIPTLET);
    fprintf(fp, "- [ALPM] count                 : %d\n", countALPM);
    fprintf(fp, "- [PACMAN] count               : %d\n", countPACMAN);

    fprintf(fp, "-------------\n");
    fprintf(fp, "List of packages\n");
    fprintf(fp, "-------------\n");
    //-----------------------------------------------------------//


    //----------------------- Print Packages --------------------//
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        for (;;) {

            fprintf(fp, "- Package Name        : %s\n", entry->key);
            fprintf(fp, "  - Install date      : %s\n", entry->installDate);
            fprintf(fp, "  - Last update date  : %s\n", entry->lastUpgrade);
            fprintf(fp, "  - How many updates  : %d\n", entry->upgrades);
            fprintf(fp, "  - Removal date      : %s\n", entry->removalDate);
            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }
    }
    //-----------------------------------------------------------//

    fclose(fp);
    return 0;
}

dateToString(char* token, char* date) {
   
    int tokenLen = strlen(token);

    date[0] = token[1];
    date[1] = token[2];
    date[2] = token[3];
    date[3] = token[4];
    date[4] = token[5];
    date[5] = token[6];
    date[6] = token[7];
    date[7] = token[8];
    date[8] = token[9];
    date[9] = token[10];
    date[10] = ' ';
    date[11] = token[12];
    date[12] = token[13];
    date[13] = token[14];
    date[14] = token[15];
    date[15] = token[16];
    date[16] = '\0';

}

printCount(ht_t* ht) {

    printf("Pacman Packages Report\n");
    printf("----------------------\n");
    printf("- Installed packages : %d\n", installedPackages);
    printf("- Removed packages   : %d\n", removedPackages);
    printf("- Upgraded packages  : %d\n", upgradedPackages);
    printf("- Current installed  : %d\n", (installedPackages - removedPackages));

    printf("-------------\n");
    printf("General Stats\n");
    printf("-------------\n");

    
    printf("- Oldest package               : %s\n", oldestPackage);
    printf("- Newest package               : %s\n", newestPackage);
    //printPackageWitNoUpgrades(ht);
    printf("- [ALPM-SCRIPTTLET] type count : %d\n", countALPMSCRIPTLET);
    printf("- [ALPM] count                 : %d\n", countALPM);
    printf("- [PACMAN] count               : %d\n", countPACMAN);

    printf("-------------\n");
    printf("List of packages\n");
    printf("-------------\n");
}

printPackages(const char* key, const char* value, const char* installDate, const int* upgrades, const char* lastUpgrade, const char* removalDate) {

    printf("- Package Name        : %s\n", key);
    printf("  - Install date      : %s\n", installDate);
    printf("  - Last update date  : %s\n", lastUpgrade);
    printf("  - How many updates  : %d\n", upgrades);
    printf("  - Removal date      : %s\n", removalDate);
}

int main(int argc, char** argv) {
    
    readFile();
    return 0;
}