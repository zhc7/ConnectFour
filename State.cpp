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

State::State(const Board &board, char *top, char nextTurn) : board(board), top(top), nextTurn(nextTurn) {}

State *State::step(int y) const {
    auto newTop = new char[N];
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
    board.set(x, y, nextTurn);
    if (BAN_X == x - 1 && BAN_Y == y) {
        top[y]--;
    }
    if (available() == 0) {
        mustWin = 3;
    } else {
        mustWin = win(x, y, M, N, board) ? nextTurn : (char) 0;
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
    auto backupTop = new char[N];
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


