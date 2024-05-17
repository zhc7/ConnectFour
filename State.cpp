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

int **copyBoard(int **board, int M, int N) {
    int **newBoard = new int*[M];
    for (int i = 0; i < M; i++) {
        newBoard[i] = new int[N];
        for (int j = 0; j < N; j++) {
            newBoard[i][j] = board[i][j];
        }
    }
    return newBoard;
}

State::State(int **board, int *top, int nextTurn) : board(board), top(top), nextTurn(nextTurn) {}

State* State::step(int y) const {
//    int *newBoard = new int[M * N];
    int *newTop = new int[N];
//    for (int i = 0; i < M * N; i++) {
//        newBoard[i] = board[i];
//    }
    int **newBoard = copyBoard(board, M, N);
    for (int i = 0; i < N; i++) {
        newTop[i] = top[i];
    }
    auto *newState = new State(newBoard, newTop, nextTurn);
    newState->_step(y);
    return newState;
}

void State::_step(int y) {
    top[y]--;
    int x = top[y];
//    board[x * N + y] = nextTurn;
    board[x][y] = nextTurn;
    if (BAN_X == x - 1 && BAN_Y == y) {
        top[y]--;
    }
    nextTurn = 3 - nextTurn;
    if (userWin(x, y, M, N, board)) {
        mustWin = 1;
    } else if (machineWin(x, y, M, N, board)) {
        mustWin = 2;
    } else if (isTie(N, top)) {
        mustWin = 3;
    }
}

bool *State::available() const {
    auto *avail = new bool[N];
    for (int i = 0; i < N; i++) {
        avail[i] = top[i] > 0;
    }
    return avail;
}

int State::simulate() const {
    if (mustWin != 0) {
        return mustWin;
    }
//    int *backupBoard = new int[M * N];
    int *backupTop = new int[N];
//    for (int i = 0; i < M * N; i++) {
//        backupBoard[i] = board[i];
//    }
    int **backupBoard = copyBoard(board, M, N);
    for (int i = 0; i < N; i++) {
        backupTop[i] = top[i];
    }
    State newState(backupBoard, backupTop, nextTurn);
    while (true) {
        bool *avail = newState.available();
        int y = (int) random() % N;
        while (!avail[y]) {
            y = (y + 1) % N;
        }
        newState._step(y);
        if (newState.mustWin != 0) {
            delete avail;
            return newState.mustWin;
        }
    }
}

State::~State() {
    for (int i = 0; i < M; i++) {
        delete[] board[i];
    }
    delete[] board;
    delete[] top;
}


