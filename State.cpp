//
// Created by zhc7 on 24-5-16.
//

#include <random>
// #include <cassert>
#include "State.h"
#include <iostream>

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
        // mustWin = win(x, y, M, N, board, nextTurn) ? nextTurn : (char) 0;
        // assert(!mustWin);
    }
    nextTurn = 3 - nextTurn;
}

void printBoard(const Board &board) {
    std::cout << "  ";
    for (int i = 0; i < State::N; i++) {
        std::cout << i % 10 << ' ';
    }
    std::cout << std::endl;
    for (int i = 0; i < State::N; i++) {
        std::cout << i % 10 << ' ';
        for (int j = 0; j < State::M; j++) {
            if (State::BAN_X == i && State::BAN_Y == j) {
                std::cout << 'X';
            } else {
                if (board.rows[i] & (1 << j)) {
                    std::cout << 'A';
                } else if (board.rows[i] & (1 << (j + 16))) {
                    std::cout << 'O';
                } else {
                    std::cout << '.';
                }
            }
            std::cout << ' ';
        }
        std::cout << std::endl;
    }
}

int State::simulate(HeavyBoard &b, int last_y) const {
    if (mustWin != 0) {
        return mustWin;
    }
    char turn = nextTurn;
    short avail = this->avail;
    char top[12];
    std::memcpy(top, this->top, sizeof(this->top));
    int y;
    int selected_y = -1;
    while (true) {
        bool selected = false;
        // check must win places
        const int px = top[last_y] - 1;
        if (px >= 0 && last_y != -1 && b.mustWin(px, last_y, turn)) {
            // b.set(px, last_y, turn);
            // assert(win(px, last_y, M, N, b, turn));
            return turn;
        }
        for (y = 0; y < N; y++) {
            const int x = top[y] - 1;
            if (x < 0) continue;
            const int off2 = (2 - turn) * 16;
            // if (last_y == -1 && b.mustWin(x, y, turn)) {
            //     b.set(x, y, turn);
            //     assert(win(x, y, M, N, b, turn));
            //     return turn;
            // }
            if (b.mustWin(x, y, 3 - turn)) {
                // b.set(x, y, 3 - turn);
                // assert(win(x, y, M, N, b, 3 - turn));
                // b.unset(x, y);
                if (selected) {
                    // enemy has two must win places
                    return 3 - turn;
                }
                selected = true;
                selected_y = y;
            }
        }
        // rollout
        if (!selected) {
            do {
                y = randomTable[random() % randomIndex];
            } while (!(avail & (1 << y)));
        } else {
            y = selected_y;
        }
        // step
        top[y]--;
        const char x = top[y];
        if (BAN_X == x - 1 && BAN_Y == y) {
            top[y]--;
        }
        avail &= ~((top[y] <= 0) << y);
        if (avail == 0) {
            return 3;
        }
        // assert(!win(x, y, M, N, b, turn));
        // update charge and jump
        b.update(x, y, turn);
        turn = 3 - turn;
        last_y = y;
    }
}
