/***********
 ID: 214267601
 NAME: Idan Fueguel
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_CELL 0
#define UNINITIALIZED -1

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;

// Checks if alloc returned NULL
void *nullCheck(void*, int);
char *getString();
int getInt();

int validLength(char *s);

void shrinkDB(int);
void expandDB();

void sortDB();
int isDBSorted();
int countShows();

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

TVShow *findShow(char *name);
Season *findSeason(TVShow *show, char *name);
Episode *findEpisode(Season *season, char *name);

void addMenu();
void deleteMenu();
void printMenuSub();
void mainMenu();

void addShow();
void insertShow(int, int, void*);
void addSeason();
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();


/*
        ---MENU FUNCS---
*/
void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}


char *getString() {
    char ch = '\0';
    char *tempName = NULL;
    int count = 0;

    while (ch != '\n') {
        scanf("%c", &ch);
        tempName = (char*)nullCheck(tempName, (++count)*sizeof(char));
        tempName[count - 1] = ch;
    }

    // Replaces \n
    tempName[count - 1] = '\0';

    if (tempName == NULL) {
        tempName[FIRST_CELL] = '\0';
    }
    
    return tempName;
}

void *nullCheck(void *origin, int size) {
    void *temp = realloc(origin, size);
    if (temp == NULL) {
        printf("\n---FAILED IN ALLOCATING MEMORY---\n");
        freeAll();
        exit(1);
    }
    return temp;
}


/*
        ---ADD FUNCS---
*/
void addShow() {
    // Part of the sorting algorithm
    int row, col = UNINITIALIZED;
    
    printf("Enter the name of the show:\n");
    char *tempName = NULL;
    tempName = getString();
    expandDB();

    // Checks if show exists
    if (findShow(tempName) != NULL) {
        printf("Show already exists.\n");
        free(tempName);
        sortDB();
        shrinkDB(FIRST_CELL);
        return;
    }

    // If not sets the right Row&Col to insert the show in the DB
    if (database[FIRST_CELL] != NULL) {
        for (int i = 0; i < dbSize; i++) {
            for (int j = 0; j < dbSize; j++) {
                if (database[i][j] != NULL) {
                    if (col == UNINITIALIZED && strcmp(database[i][j]->name, tempName) > 0) {
                        row = i;
                        col = j;
                    }
                } else if (col == UNINITIALIZED) {
                    row = i;
                    col = j;
                }
            }
        }
    } else {
        row = FIRST_CELL;
        col = FIRST_CELL;
    }

    TVShow *show = NULL;
    show = (TVShow*)nullCheck(show, sizeof(TVShow));
    show->name = tempName;
    show->seasons = NULL;
    insertShow(row, col, show);


    //        SOMETHING NOT WORKING WITH STRCMP ON LINUX SERVER ONLY WHEN RUN WITH .TXT
}

void insertShow(int row, int col, void *choice) {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] == NULL) {
                if (i == row && j == col) {
                    database[i][j] = choice;
                    return;
                }
                
                if (!j) {
                    database[i][j] = database[i - 1][dbSize - 1];
                    database[i - 1][dbSize - 1] = NULL;
                } else {
                    database[i][j] = database[i][j - 1];
                    database[i][j - 1] = NULL;
                }
                insertShow(row, col, choice);
                return;
            }
        }
    }
}

void addSeason() {
    printf("Enter the name of the show:\n");
    char *tempName = getString();
    TVShow *show = findShow(tempName);

    if (show == NULL) {
        printf("Show not found\n");
        free(tempName);
        return;
    }

    printf("Enter the name of the season:\n");
    tempName = getString();
    
    if (findSeason(show, tempName) != NULL) {
        printf ("Season already exists.");
        free(tempName);
        return;
    }

    Season *s = NULL;
    s = (Season*)nullCheck(s, sizeof(Season));
    s->name = tempName;
    s->episodes = NULL;
    s->next = NULL;

    printf("Enter the position:\n");
    int pos;
    scanf("%d", &pos);

    if (!pos || show->seasons == NULL) {
        s->next = show->seasons;
        show->seasons = s;
        return;
    }

    Season *temp = show->seasons;
    for (int i = 0; i < pos - 1; i++) {
        if (temp->next == NULL) {
            break;
        }
        temp = temp->next;
    }
    s->next = temp->next;
    temp->next = s->next;
}

void addEpisode() {
    
}


/*
        ---FIND FUNCS---
*/
TVShow *findShow(char *name) {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL && !strcmp(database[i][j]->name, name)) {
                return database[i][j];
            }
        }
    }
    return NULL;
}

Season *findSeason(TVShow *show, char *name) {
    if (show->seasons == NULL) {
        return NULL;
    }

    Season *s = show->seasons;
    while (s->name != name) {
        if (s->next == NULL) {
            return NULL;
        }
        s = s->next;
    }
    return s;
}

Episode *findEpisode(Season *season, char *name) {
    if (season->episodes == NULL) {
        return NULL;
    }

    Episode *e = season->episodes;
    while (e->name != name) {
        if (e->next == NULL) {
            return NULL;
        }
        e = e->next;
    }
    return e;
}


/*
        ---DELETE FUNCS---
*/
void deleteShow() {
    printf("Enter the name of the show:\n");
    char *tempName = getString();

    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) {
                //printf("\n%s\n%s\n", database[i][j]->name, tempName);
                if (!strcmp(database[i][j]->name, tempName)) {
                    freeShow(database[i][j]);
                    database[i][j] = NULL;
                    sortDB();
                    shrinkDB(FIRST_CELL);
                    return;
                }
            }
        }
    }
    printf("Show not found.\n");
}

void deleteSeason() {

}

void deleteEpisode() {

}


/*
        ---FREE FUNCS---
*/
void freeAll() {
    if (database == NULL) {
        return;
    }
    
    for (int i = dbSize - 1; i >= 0; i--) {
        for (int j = dbSize - 1; j >= 0; j--) {
            if (database[i][j] != NULL) {
                freeShow(database[i][j]);
            }
        }
        if (database[i] != NULL) {
            free(database[i]);
        }
    }
    free(database);
}

void freeShow(TVShow *show) {
    if (show == NULL) {
        return;
    }
    
    if (show->seasons == NULL) {
        free(show->name);
        free(show);
        return;
    }

    if (show->seasons->next == NULL) {
        freeSeason(show->seasons);
        free(show->name);
        free(show);
        return;
    }

    Season *temp1 = show->seasons->next;
    Season *temp2 = show->seasons;

    while (temp1->next != NULL) {
        temp2 = temp1;
        temp1 = temp1->next;
    }
    freeSeason(temp1);
    temp2->next = NULL;
    freeShow(show);
}

void freeSeason(Season *s) {
    //NOT DONE
    if (s == NULL) {
        return;
    }
    
    if (s->episodes == NULL) {
        free(s->name);
        free(s);
        return;
    }
    
    if (s->episodes->next == NULL) {
        freeEpisode(s->episodes);
        free(s->name);
        free(s);
        return;
    }

    Episode *temp1 = s->episodes->next;
    Episode *temp2 = s->episodes;

    while (temp1->next != NULL) {
        temp2 = temp1;
        temp1 = temp1->next;
    }
    freeEpisode(temp1);
    temp2->next = NULL;
    freeSeason(s);
}

void freeEpisode(Episode *e) {
    //NOT DONE
    if (e == NULL) {
        return;
    }
    
    free(e->name);
    free(e->length);
    free(e);
    return;
}


/*
        ---EXPAND AND SHRINK FUNCS---
*/
void expandDB() {
    if (database == NULL) {
        database = (TVShow***)nullCheck(database, sizeof(TVShow**)*++dbSize);
        database[FIRST_CELL] = NULL;
        database[FIRST_CELL] = (TVShow**)nullCheck(database[FIRST_CELL], sizeof(TVShow*)*dbSize);
        database[FIRST_CELL][FIRST_CELL] = NULL;
    } else if (database[dbSize - 1][dbSize - 1] != NULL) {
        database = (TVShow***)nullCheck(database, sizeof(TVShow**)*++dbSize);
        database[dbSize - 1] = NULL;
        for (int i = 0; i < dbSize; i++) {
            database[i] = (TVShow**)nullCheck(database[i], sizeof(TVShow*)*dbSize);
            database[i][dbSize - 1] = NULL;
        }
        
        for (int i = 0; i < dbSize; i++) {
            database[dbSize - 1][i] = NULL;
        }
    }
    sortDB();
}

// Sorts by pushing TVShows whithout going back over sorted rows before shrinking
void shrinkDB(int lim) {
    if (countShows() <= (dbSize - 1)*(dbSize - 1)) {
        if (database[lim][dbSize - 1] != NULL) {
            for (int i = lim; i < dbSize - 1; i++) {
                for (int j = 0; j < dbSize; j++) {
                    if (database[i][j] == NULL) {
                        if (!j && i - 1 >= 0) {
                            database[i][j] = database[i - 1][dbSize - 1];
                            database[i - 1][dbSize - 1] = NULL;
                            shrinkDB(lim);
                            return;
                        } else {
                            database[i][j] = database[i][j - 1];
                            database[i][j - 1] = NULL;
                            shrinkDB(lim);
                            return;
                        }
                    }
                }
            }
        } else if (lim + 1 < dbSize - 1) {
            shrinkDB(lim + 1);
            return;
        }

        free(database[dbSize - 1]);
        if (dbSize - 1){
            database = (TVShow***)nullCheck(database, sizeof(TVShow**)*--dbSize);
        } else {
            dbSize--;
            free(database);
            database = NULL;
        }

        if (database != NULL) {
            for (int i = 0; i < dbSize; i++) {
                database[i] = (TVShow**)nullCheck(database[i], sizeof(TVShow*)*dbSize);
                
                /*
                MIGHT NOT FREE MEMORY
                */
            
            }
        }
    }
    sortDB();
}


/*
        ---SORT AND COUNT FUNCS---
*/
void sortDB() {
    if (!isDBSorted()) {
        for (int i = dbSize - 1; i >= 0; i--) {
            for (int j = dbSize - 1; j >= 0; j--) {
                if (database[i][j] != NULL) {
                    if (!j && i - 1 >= 0 ) {
                        if (database[i - 1][dbSize - 1] == NULL) {
                            database[i - 1][dbSize - 1] = database[i][j];
                            database[i][j] = NULL;
                            sortDB();
                            return;
                        }
                    } else if (database[i][j - 1] == NULL) {
                        database[i][j - 1] = database[i][j];
                        database[i][j] = NULL;
                        sortDB();
                        return;
                    }
                }
            }
        }
    }
}

int isDBSorted() {
    if (database != NULL) {
        int count = 0;
        for (int i = 0; i < dbSize; i++) {
            for (int j = 0; j < dbSize; j++) {
                if (database[i][j] != NULL) {
                    count++;
                } else if (count == countShows()) {
                    return 1;
                } else {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int countShows() {
    int count = 0;
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) {
                count++;
            }
        }
    }
    return count;
}


/*
        ---PRINT FUNCS---
*/
void printEpisode() {
    printf("Enter the name of the show:\n");
    char *tempName = getString();
    TVShow *show = findShow(tempName);

    if (show != NULL) { 
        printf("Enter the name of the season:\n");
        tempName = getString();
        Season *se = findSeason(show, tempName);

        if (se != NULL) {
            printf("Enter the name of the episode:\n");
            tempName = getString();
            Episode *ep = findEpisode(se, tempName);
            if (ep != NULL) {
                printf("Name: %s\n", ep->name);
                printf("Length: %s\n", ep->length);
                free(tempName);
            } else {
                printf("Episode not found\n");
                free(tempName);
            }
        } else {
            printf("Season not found\n");
            free(tempName);
        }
    } else {
        printf("Show not found\n");
        free(tempName);
    }
}

void printShow() {
    printf("Enter the name of the show:\n");
    char *tempName = getString();
    TVShow *show = findShow(tempName);
    free(tempName);

    if (show == NULL) {
        printf("Show not found.\n");
        return;
    }
    printf("Name: %s\n", show->name);
    printf("Seasons:\n");
    Season *s = show->seasons;
    for (int i = 0; s != NULL; i++) {
        printf("    Season %d: %s\n", i, s->name);
        if (s->episodes != NULL) {
            Episode *ep = s->episodes;
            for (int j = 0; ep != NULL; j++) {
                printf("        Episode %d: %s (%s)\n", j, ep->name, ep->length);
                ep = ep->next;
            }
        }
        s = s->next;
    }
}

void printArray() {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL){
                printf("[%s] ", database[i][j]->name);
            } else {
                printf("[NULL] ");
            }
        }
        printf("\n");
    }
}




/*
for (int i = 0; i < dbSize; i++) {
    for (int j = 0; j < dbSize; j++) {
        
    }
}
*/