#include "game.h"
#include <stdlib.h>
#include <stddef.h>

/*
Takes as input a pointer to a connect4 game board, a player (0/1), and a int column.
Places a new piece in `col` on the game board.
*/
int make_move(struct game* g, int player, int col) {
    if (col < 0 || col > 6) {
        return -1;
    }
    if (player != g->turn) {
        return -1;
    }
    for (int row = 5; row >= 0; row--) {
        if (g->board.cells[row][col] == -1) {
            g->board.cells[row][col] = player;
            g->turn = 1 - player;
            return 0;
        }
    }
    return -1;
    
}
 
/*
Takes as input a pointer to a connect4 game board.
Returns 0 or 1 if player 0 or 1 has won respectively.
Returns -1 if there is no winner.
*/
int check_win(struct game* g) {
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            int player = g->board.cells[row][col];

            if (player == -1) {
                continue;
            }
            if (col <= 3) {
                if (player == g->board.cells[row][col+1] &&
                    player == g->board.cells[row][col+2] &&
                    player == g->board.cells[row][col+3]) {
                    return player;
                }
            }
            if (row <= 2) {
                if (player == g->board.cells[row+1][col] &&
                    player == g->board.cells[row+2][col] &&
                    player == g->board.cells[row+3][col]) {
                    return player;
                }
            }
            if (row <= 2 && col <= 3) {
                if (player == g->board.cells[row+1][col+1] &&
                    player == g->board.cells[row+2][col+2] &&
                    player == g->board.cells[row+3][col+3]) {
                    return player;
                }
            }
            if (row <= 2 && col >= 3) {
                if (player == g->board.cells[row+1][col-1] &&
                    player == g->board.cells[row+2][col-2] &&
                    player == g->board.cells[row+3][col-3]) {
                    return player;
                }
            }
        }
    }

    return -1;
}
 
/*
Initializes an existing game with an empty board.
*/
void new_game(struct game* g, int gameId) {
    g->gameId = gameId;
    g->turn = 0;
    g->players = 1;
    g->finished = 0;

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            g->board.cells[i][j] = -1;
        }
    }
   
}


