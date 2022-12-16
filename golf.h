// Declarations for Course, Player and PlayerNode data types
// Course is an ADT that represents one round of golf on a particular course for set of players
// Player represents a single player's score card.
// PlayerNode is used to build a linked list of players.

#ifndef FALL21_GOLF_H
#define FALL21_GOLF_H

typedef struct golfCourse * Course;  // the golfCourse struct is defined in Course.c


// This struct captures the information for one round of one player on the specified course.
struct golfPlayer {
    char name[16];   // player's name (max string length is 15)
    Course course;   // course on which this round is being played
    // note: for the following arrays, info about hole N will be stored in array element N-1
    char * strokes[18];   // sequence of strokes for each hole -- space must be allocated for each string
    int strokeScore[18];  // score (# of strokes) for each hole
    int parScore[18];     // score (rel. to par) for each hole
};

typedef struct golfPlayer Player;

// This struct is used to create a linked list of players
struct golfPlayerNode {
    Player* player;  // pointer to a Player struct  (should never be NULL)
    struct golfPlayerNode * next; // pointer to the next node in the list (may be NULL)
};

typedef struct golfPlayerNode PlayerNode;

// Functions related to a single player
Player * newPlayer(const char *name, Course course);   // create a new player

// scoreHole: record stroke information for one hole for this player, return score (for this hole) relative to par
// this function will update the strokes, stokeScore and parScore data for the player
int scoreHole(Player *p, int hole, const char* strokes);  // hole is between 1 and 18
int totalStrokeScore(const Player *p);  // total number of strokes on all holes played
int totalParScore(const Player *p);     // total score relative to par (e.g., 2 means two-over-par) on all holes played
int greensInReg(const Player *p);       // number of greens reached in (par - 2) strokes, for all holes played
void fairwaysHit(const Player *p, int *hit, int *holes);    // fairways hit (only for par-4 and par-5 holes), for all holes played
int countScores(const Player *p, int parScore);  // returns number of times player achieved a certain par-score on any hole
// for example, caller would set parScore to -1 to count the number of birdies on holes played so far


Course readCourse(const char * filename);  // read course and score info from file
const char * courseName(Course c);   // course name
const int * courseHoles(Course c);   // array of pars for 18 holes
const PlayerNode * coursePlayers(Course c);  // list of all players, alphabetical
PlayerNode * courseLeaders(Course c, int n);  // return top n players plus ties
int numPlayers(Course c);  // number of players
Player * findPlayer(const char * name, Course c);  // find and return a specific player
void addPlayer(Player *p, Course c);  // add a player to course list

double avgTotalScore(Course c);   // return average stroke score for all players in list
double avgParScore(Course c);     // return average par score for all players in list
double avgHoleScore(Course c, int hole);    // return average stroke score for a particular hole



#endif //FALL21_GOLF_H
