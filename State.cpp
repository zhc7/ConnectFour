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

void State::step(int y, State &target) const {
    target.copyFrom(*this);
    target._step(y);
}

void State::copyFrom(const State &state) {
    for (int i = 0; i < N; i++) {
        top[i] = state.top[i];
    }
    board.copyFrom(state.board);
    nextTurn = state.nextTurn;
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
        mustWin = win(x, y, M, N, board, nextTurn) ? nextTurn : (char) 0;
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
    State newState;
    newState.copyFrom(*this);
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
