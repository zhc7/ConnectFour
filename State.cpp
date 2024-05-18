//
// Created by zhc7 on 24-5-16.
//

#include <random>
#include "State.h"
#include "Judge.h"

int State::M = -1;
int State::N = -1;
int State::BAN_X = -1;
int State::BAN_Y = -1;

State::State(Board board, char *top, char nextTurn) : board(board), top(top), nextTurn(nextTurn) {}

State* State::step(int y) const {
//    int *newBoard = new int[M * N];
    auto newTop = new char[N];
//    for (int i = 0; i < M * N; i++) {
//        newBoard[i] = board[i];
//    }
    auto newBoard = Board(board);
    for (int i = 0; i < N; i++) {
        newTop[i] = top[i];
    }
    auto *newState = new State(newBoard, newTop, nextTurn);
    newState->_step(y);
    return newState;
}

void State::_step(int y) {
    top[y]--;
    int x = (int) top[y];
//    board[x * N + y] = nextTurn;
    board.set(x, y, nextTurn);
    if (BAN_X == x - 1 && BAN_Y == y) {
        top[y]--;
    }
//    if (userWin(x, y, M, N, board)) {
//        mustWin = 1;
//    } else if (machineWin(x, y, M, N, board)) {
//        mustWin = 2;
//    } else if (isTie(N, top)) {
//        mustWin = 3;
//    }
    if (isTie(N, top)) {
        mustWin = 3;
    } else if (nextTurn == 1 && userWin(x, y, M, N, board)) {
        mustWin = 1;
    } else if (nextTurn == 2 && machineWin(x, y, M, N, board)) {
        mustWin = 2;
    }
    nextTurn = 3 - nextTurn;
}

short State::available() const {
    short avail = 0;
    for (int i = 0; i < N; i++) {
        avail |= (top[i] > 0) << i;
    }
    return avail;
}

int State::simulate() const {
    if (mustWin != 0) {
        return mustWin;
    }
//    int *backupBoard = new int[M * N];
    char *backupTop = new char[N];
//    for (int i = 0; i < M * N; i++) {
//        backupBoard[i] = board[i];
//    }
    auto backupBoard = Board(board);
    for (int i = 0; i < N; i++) {
        backupTop[i] = top[i];
    }
    State newState(backupBoard, backupTop, nextTurn);
    while (true) {
        short avail = newState.available();
        int y = (int) random() % N;
        while (!(avail & (1 << y))) {
            y = (y + 1) % N;
        }
        newState._step(y);
        if (newState.mustWin != 0) {
            return newState.mustWin;
        }
    }
}

State::~State() {
    delete[] top;
}


