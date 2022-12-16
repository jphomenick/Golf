
// This program reads information about a golf tournament
// and allows a user to view information about the course,
// the overall scores, and the individual players.
//


#include <stdio.h>
#include <stdlib.h>

#include "golf.h"

void printPlayerScores(const PlayerNode* list);
void printLeaderboard(const PlayerNode * list);
void printPlayerStats(const Player* player);
void printStats(Course);

// The main function implements the user interface.
// This main function has arguments that come from the command line,
// which allows you to run using the same input file without
// typing the file name over and over again.  See the spec document.
int main(int argc, char **argv) {
    setbuf(stdout,0);   // remove the need for fflush
    char * filename = "";
    if (argc == 2) filename = argv[1];  // file name passed on command line
    else {
        filename = calloc(81, sizeof(char));
        printf("Enter a file name: ");
        scanf("%80s%*c", filename);  // also read and discard linefeed character
    }

    // open the file and read the info
    Course crs = readCourse(filename);
    if (!crs) {
        printf("ERROR: Course file %s cannot be opened.\n", filename);
        return EXIT_FAILURE;
    }
    printf("Welcome to today's golf tournament.\n");
    printf("Course: %s\n", courseName(crs));
    printf("Number of players: %d\n\n", numPlayers(crs));

    // This is the command prompt loop.  User enters a command after each prompt.
    // Continue until command is quit (q).
    printf("After each prompt (?), enter a command.\n");
    char command = 0;
    // variables for reading information from user
    int numLeaders = 0;
    char player[16] = "";   // player name
    Player * p = NULL;
    while (command != 'q') {
        printf("? ");
        scanf("%c", &command);
        // Using a switch statement to process command, similar to a series of if-else statements.
        // If you're interested, see zyBook 3.11.
        switch (command) {
            case 'q':
                break;  // quit, nothing to do
            case 'p':
                // print scores for all players
                printPlayerScores(coursePlayers(crs));
                break;
            case 'l':
                // print scores for leaders only
                scanf("%d", &numLeaders);
                printLeaderboard(courseLeaders(crs, numLeaders));
                break;
            case 'f':
                // find a particular player and print statistics
                scanf("%15s", player);
                p = findPlayer(player, crs);
                if (!p) printf("Player \"%s\" not found.\n", player);
                else printPlayerStats(p);
                break;
            case 's':
                // print stats for the entire round
                printStats(crs);
                break;
            default:
                printf("Unknown command\n");
                break;
        }
        scanf("%*c");  // remove the linefeed
    }
    return EXIT_SUCCESS;
}

// Given player list, print each name and score
void printPlayerScores(const PlayerNode* list) {
    const PlayerNode* n;
    for (n = list; n; n = n->next) {
        const Player * p = n->player;
        printf("%-15s %3d (%+d)\n", p->name, totalStrokeScore(p), totalParScore(p));
    }
}

// Given player list, print position, name, and score
void printLeaderboard(const PlayerNode * list) {
    const PlayerNode* n;
    int place = 0;
    int score = 0, lastScore = 0;
    int count = 0;
    for (n = list; n; n = n->next) {
        count += 1;
        score = totalStrokeScore(n->player);
        if (score > lastScore) {
            place = count;
            lastScore = score;
        }
        printf("%3d. %-15s %3d (%+d)\n", place, n->player->name, score, totalParScore(n->player));
    }
}

// Given a player, print scorecard
void printPlayerStats(const Player* player) {
    int i;
    int score = 0;
    printf("--- Scorecard for %s ---\n", player->name);
    printf("Hole:    ");
    for (i=1; i<=18; i++) printf("%4d", i);
    printf("\n");
    printf("Par:     ");
    const int *pars = courseHoles(player->course);
    for (i=0; i<18; i++) printf("%4d", pars[i]);
    printf("\n");
    printf("Strokes: ");
    for (i=0; i<18; i++) {
        int strokes = player->strokeScore[i];
        if (strokes == 0) break;  // no more scores to print
        printf("%4d", strokes);
    }
    printf("\n");
    printf("Score:   ");
    for (i=0; i<18 && (player->strokeScore[i] > 0); i++) {
        score += player->parScore[i];
        if (score == 0) printf("   E");
        else printf("%+4d", score);
    }
    printf("\n");
    int greens = greensInReg(player);
    printf("Greens in regulation: %d/18 (%.2f%%)\n", greens, (greens/18.0)*100.0);
    int fairways = 0, fHoles = 0;
    fairwaysHit(player, &fairways, &fHoles);
    if (fHoles > 0) printf("Fairways hit: %d/%d (%.2f%%)\n", fairways, fHoles, (((double)fairways)/fHoles)*100.0);
    else printf("Fairways hit: 0/0 (0.00%%)\n");
    printf("Eagles: %d, ", countScores(player, -2));
    printf("Birdies: %d, ", countScores(player, -1));
    printf("Pars: %d, ", countScores(player, 0));
    printf("Bogies: %d, ", countScores(player, 1));
    printf("Double-bogies: %d\n", countScores(player, 2));
}

// print course-wide statistics
void printStats(Course crs) {
    int i;
    printf("--- Course statistics --- \n");
    printf("Avg strokes: %.2f\n", avgTotalScore(crs));
    printf("Avg score: %.2f\n", avgParScore(crs));
    printf("Hole:        ");
    for (i=1; i<=18; i++) printf("%6d", i);
    printf("\n");
    printf("Par:         ");
    const int *pars = courseHoles(crs);
    for (i=0; i<18; i++) printf("%6d", pars[i]);
    printf("\n");
    printf("Avg Strokes: ");
    for (i=1; i<=18; i++) {
        double strokes = avgHoleScore(crs, i);
        if (strokes == 0) break;  // no more scores to print
        printf("%6.2f", strokes);
    }
    printf("\n");
    printf("Diff:        ");
    for (i=1; i<=18; i++) {
        double strokes = avgHoleScore(crs, i);
        if (strokes == 0) break;  // no more scores to print
        printf("%+6.2f", strokes - pars[i-1]);
    }
    printf("\n");
}
