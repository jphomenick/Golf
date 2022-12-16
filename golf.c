// This program reads information about a golf tournament
// and allows a user to view information about the course,
// the overall scores, and the individual players.
//
// Implementation of Course ADT and functions for Player and PlayerNode types
//
// Joseph Homenick

#include "golf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct golfCourse {
    char courseName[42];                            //name of golf course
    int par[18];                                    //array for par for each hole in course
    struct golfPlayerNode * playerList;             //pointer to list of players
    struct golfPlayerNode * leadersList;
};


Player * newPlayer(const char *name, Course course){
    Player * playerptr = malloc(sizeof(Player));

    strcpy(playerptr->name, name);                      //set player name
    playerptr->course = course;                         //set course on which game is being played

    for (int i = 0; i < 18; i++) {
        playerptr->strokes[i] = 0;                      //initialize strokes to zero
        playerptr->strokeScore[i] = 0;                  //initialize stroke score to zero
        playerptr->parScore[i] = 0;                     //initialize par score to zero
    }

    addPlayer(playerptr, course);

    return playerptr;
}

int scoreHole(Player *p, int hole, const char* strokes) {
    int numStrokes = 0;                                     //number of strokes for the hole
    int par = p->course->par[hole - 1];                         //par score for the hole
    char * holeStroke = malloc(11 * sizeof(char));

    strcpy(holeStroke, strokes);
    p->strokes[hole - 1] = holeStroke;

    numStrokes = strlen(holeStroke);

    p->strokeScore[hole - 1] = numStrokes;

    p->parScore[hole - 1] = numStrokes - par;

    return p->parScore[hole - 1];
}

int totalStrokeScore(const Player *p) {
    int score = 0;

    for (int i = 0; i < 18; i++) {
        if (p->strokeScore[i] == 0) break;
        score += p->strokeScore[i];
    }

    return score;
}

int totalParScore(const Player *p) {
    int score = 0;
    for (int i = 0; i < 18; i++) {
        if (p->strokeScore[i] == 0) break;
        score += p->parScore[i];
    }
    return score;
}

int greensInReg(const Player *p) {
    int count = 0;
    char strokes[11];

    for (int i = 0; i < 18; i++) {
        int target = (p->course->par[i]) - 2;
        strcpy(strokes, p->strokes[i]);
        for (int j = 0; j < target; j++) {
            if (strokes[j] =='g' || strokes[j] == 'h') {
                count ++;
                break;
            }
        }

    }
    return count;
}

void fairwaysHit(const Player *p, int *hit, int *holes) {
    int f_hits = 0;                       //number of fairways hit
    int par_count = 0;                    // total number of par-4 and par-5 holes
    char strokes[11];

    for (int i = 0; i < 18; i++) {
        int par = p->course->par[i];
        if (par == 4 || par == 5) {
            par_count++;
            strcpy(strokes, p->strokes[i]);
            if (strokes[0] == 'f' || strokes[0] == 'g') f_hits++;
        }
    }

    *hit = f_hits;
    *holes = par_count;
}

int countScores(const Player *p, int parScore) {
    int count = 0;
    for (int i = 0; i < 18; i++) {
        if (p->parScore[i] == parScore) count++;
    }
    return count;
}


Course readCourse(const char * filename) {
    int parnum = 0;                                     //par for a hole
    int hole = 0;                                       //hole number read from file
    Course gcptr;                                       //pointer to golfCourse struct
    FILE * courseFile = fopen(filename, "r");           //Stream to read txt file
    char temp[28];
    char playerName[16];
    char holestr[3];
    char strokes[11];


    if (!courseFile) {
        return NULL;
    }

    else {
        gcptr = malloc(sizeof(struct golfCourse));      //pointer to golfCourse struct
        fgets(gcptr->courseName, 41, courseFile);       //get name of golf course and put in golfCourse struct
        *strchr(gcptr->courseName, '\n') = 0;           //remove linefeed

        gcptr->playerList = NULL;                       //set the player list for the golf struct to NULL
        gcptr->leadersList = NULL;

        //Loop gets the par for each hole and stores to an array in the golfCourse struct
        for (int i = 0; i < 18; i++) {
            fscanf(courseFile, "%d", &parnum);
            gcptr->par[i] = parnum;
        }

        while ((fscanf(courseFile,"%s", temp)) != EOF) {
            char *tempptr = temp;
            char *ptr;

            ptr = strchr(temp, ':');
            *ptr = 0;
            strcpy(playerName, tempptr);
            tempptr = ptr + 1;

            ptr = strchr(tempptr, ':');
            *ptr = 0;
            strcpy(holestr, tempptr);
            tempptr = ptr + 1;

            strcpy(strokes, tempptr);


            if (findPlayer(playerName, gcptr) == NULL) {
                newPlayer(playerName, gcptr);

            }

            hole = atoi(holestr);

            scoreHole(findPlayer(playerName, gcptr), hole, strokes);

        }


        return gcptr;
    }
}

const char * courseName(Course c) {
    return c->courseName;

}

const int * courseHoles(Course c) {
    return c->par;
}

const PlayerNode * coursePlayers(Course c) {
    return c->playerList;
}

PlayerNode * courseLeaders(Course c, int n) {
    //Create copy of existing golfPlayer node linked list
    struct golfPlayerNode * origptr = c->playerList;        //Pointer to original linked list
    struct golfPlayerNode * listCopy = NULL;
    struct golfPlayerNode * tail = NULL;                    //pointer to the last node in the copied list

    while (origptr != NULL) {
        //Create first node of copied list
        if (listCopy == NULL) {
            listCopy = malloc(sizeof(struct golfPlayerNode));
            listCopy->player = origptr->player;
            listCopy->next = NULL;
            tail = listCopy;
        }
        else {
            tail->next = malloc(sizeof(struct golfPlayerNode));
            tail = tail->next;
            tail->player = origptr->player;
            tail->next = NULL;
        }
        origptr = origptr->next;
    }

    //Sort list in ascending order based on score (lowest score = best player)

    struct golfPlayerNode * ptr = listCopy;         //pointer to list to be sorted
    struct golfPlayerNode * sorted = NULL;

    while (listCopy != NULL) {
        ptr = listCopy;
        listCopy = listCopy->next;
        ptr->next = NULL;

        struct golfPlayerNode * sp = sorted;
        struct golfPlayerNode * sprev = NULL;

        while (sp != NULL && (totalStrokeScore(sp->player) <= totalStrokeScore(ptr->player))) {
            sprev = sp;
            sp = sp->next;
        }

        if (sprev != NULL) {
            sprev->next = ptr;
        }
        else {
            sorted = ptr;
        }

        ptr->next = sp;
    }

    //sorted is the head of the sorted list
    //Now make list only have the top n players (including ties)

    struct golfPlayerNode * leaders = sorted;

    int count = 1;

    while (sorted != NULL && count < n){
        count++;
        sorted = sorted->next;
    }

    if (sorted != NULL && sorted->next == NULL) {
        return leaders;
    }

    if (sorted != NULL && (totalParScore(sorted->player) != totalParScore(sorted->next->player))){
        sorted->next = NULL;
        return leaders;
    }

    //If there are ties, inlcude those players in the leaders list
    while (sorted != NULL && (totalParScore(sorted->player) == totalParScore(sorted->next->player))) {
        sorted = sorted->next;
    }
    if (sorted != NULL) sorted->next = NULL;

    return leaders;
}



int numPlayers(Course c) {
    int players = 0;
    struct golfPlayerNode * ptr = c->playerList;

    if (ptr == NULL) players = 0;

    while (ptr != NULL) {
        players++;
        ptr = ptr->next;
    }
    return players;
}

Player * findPlayer(const char * name, Course c) {
    struct golfPlayerNode * p = c->playerList;
    int found = 0;

    if (p == NULL) return NULL;

    while (p != NULL) {
        if ((strcmp(p->player->name, name) == 0)) {
            found = 1;
            break;
        }
        p = p->next;
    }

    if (found) return p->player;
    else return NULL;
}

void addPlayer(Player *p, Course c) {
    struct golfPlayerNode * head = c->playerList;
    struct golfPlayerNode * ptr = head;
    struct golfPlayerNode * newPlayer = malloc(sizeof(struct golfPlayerNode));
    newPlayer->player = p;
    newPlayer->next = NULL;
    struct golfPlayerNode * prev = NULL;

//Following code adds player to the linked list in alphabetical order using strcmp as condition
    while ((ptr!=NULL) && ((strcmp(ptr->player->name, p->name)) < 0)) {
        prev = ptr;
        ptr = ptr->next;
    }
    if (prev != NULL) {
        prev->next = newPlayer;
    }
    else {
        head = newPlayer;
        newPlayer->next = NULL;
    }
    newPlayer->next = ptr;


    c->playerList = head;

}

double avgTotalScore(Course c) {
    int players = numPlayers(c);
    int score = 0;

    struct golfPlayerNode * ptr = c->playerList;

    while (ptr != NULL) {
        score += totalStrokeScore(ptr->player);
        ptr = ptr->next;
    }


    return (double) score / players;
}

double avgParScore(Course c) {
    int players = numPlayers(c);
    int score = 0;

    struct golfPlayerNode * ptr = c->playerList;

    while (ptr != NULL) {
        score += totalParScore(ptr->player);
        ptr = ptr->next;
    }

    return (double) score / players;

}

double avgHoleScore(Course c, int hole) {
    int players = numPlayers(c);
    int score = 0;

    struct golfPlayerNode * ptr = c->playerList;

    while (ptr != NULL) {
        score += ptr->player->strokeScore[hole - 1];
        ptr = ptr->next;
    }

    return (double) score / players;


}




