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
char State::randomTable[30];
int State::randomIndex = 0;

State::State() {
    avail = (1 << N) - 1;
}

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
    mustWin = state.mustWin;
    avail = state.avail;
}


void State::_step(int y) {
    top[y]--;
    int x = (int) top[y];
    board.set(x, y, nextTurn);
    if (BAN_X == x - 1 && BAN_Y == y) {
        top[y]--;
    }
    avail &= ~((top[y] <= 0) << y);
    if (avail == 0) {
        mustWin = 3;
    } else {
        mustWin = win(x, y, M, N, board, nextTurn) ? nextTurn : (char) 0;
    }
    nextTurn = 3 - nextTurn;
}

int State::simulate() const {
    if (mustWin != 0) {
        return mustWin;
    }
    BoardSlanted b(board);
    char turn = nextTurn;
    short avail = this->avail;
    char top[12];
    std::memcpy(top, this->top, sizeof(top));
    while (true) {
        int y;
        bool selected = false;
        for (y = 0; y < N; y++) {
            if (avail & (1 << y)) {
                const char x = top[y] - 1;
                b.set(x, y, turn);
                if (win(x, y, b, turn)) return turn;
                b.unset(x, y);
                b.set(x, y, 3 - turn);
                const bool enemyWin = win(x, y, b, 3 - turn);
                b.unset(x, y);
                if (enemyWin) {
                    selected = true;
                    break;
                }
            }
        }
        // rollout
        if (!selected) {
            y = randomTable[random() % randomIndex];
            while (!(avail & (1 << y))) {
                y = randomTable[random() % randomIndex];
            }
        }
        // step
        top[y]--;
        const char x = top[y];
        b.set(x, y, turn);
        if (BAN_X == x - 1 && BAN_Y == y) {
            top[y]--;
        }
        avail &= ~((top[y] <= 0) << y);
        if (avail == 0) {
            return 3;
        }
        turn = 3 - turn;
    }
}
