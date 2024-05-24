//
// Created by zhc7 on 24-5-16.
//

#include <random>
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
        mustWin = win(x, y, M, N, board, nextTurn) ? nextTurn : (char) 0;
    }
    nextTurn = 3 - nextTurn;
}

int hot(const int row) {
    return row & (row << 1) & (row << 2);
}

int jump(const int row) {
    /*
     *1011000
     *0001011
     *0010110
     *0101100
     *1011000
     */
    const int match = row & (row << 3);
    const int A = match & (row << 2);
    const int B = match & (row << 1);
    return (A >> 1) | (B >> 2);
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

int State::simulate() const {
    if (mustWin != 0) {
        return mustWin;
    }
    BoardSlanted b(board);
    char turn = nextTurn;
    short avail = this->avail;
    char top[16];
    std::memcpy(top, this->top, sizeof(this->top));
    top[12] = -1;
    top[15] = -1;
    int charge_r[12]{};
    int charge_c[12]{};
    int jump_r[12]{};
    int jump_c[12]{};
    int charge_sl[23]{};
    int jump_sl[23]{};
    int charge_sr[23]{};
    int jump_sr[23]{};
    for (int i = 0; i < N; i++) {
        charge_r[i] = hot(b.rows[i]);
        charge_c[i] = hot(b.cols[i]);
        jump_r[i] = jump(b.rows[i]);
        jump_c[i] = jump(b.cols[i]);
    }
    for (int i = 3; i < N + M - 4; i++) {
        charge_sl[i] = hot(b.slanted_left[i]);
        charge_sr[i] = hot(b.slanted_right[i]);
        jump_sl[i] = jump(b.slanted_left[i]);
        jump_sr[i] = jump(b.slanted_right[i]);
    }
    int y;
    int last_y = -1;
    int selected_y = -1;
    while (true) {
        bool selected = false;
        // check must win places
        constexpr int mask = 0b10001;
        const int off1 = (turn - 1) * 16;
        const int px = top[last_y] - 1;
        if (
            px >= 0 && last_y != -1 && (
            charge_r[px] & (mask << (last_y + off1) >> 1)
            || charge_c[last_y] & (mask << (px + off1) >> 1)
            || charge_sl[px + last_y] & (mask << (last_y + off1) >> 1)
            || charge_sr[px - last_y + 11] & (mask << (last_y + off1) >> 1)
            || jump_r[px] & (1 << (last_y + off1))
            || jump_c[last_y] & (1 << (px + off1))
            || jump_sl[px + last_y] & (1 << (last_y + off1))
            || jump_sr[px - last_y + 11] & (1 << (last_y + off1))
        )) {
            // b.set(px, last_y, turn);
            // assert(win(px, last_y, M, N, b, turn));
            return turn;
        }
        for (y = 0; y < N; y++) {
            const int x = top[y] - 1;
            if (x < 0) continue;
            const int off2 = (2 - turn) * 16;
            if (
                last_y == -1 && (
                charge_r[x] & (mask << (y + off1) >> 1)
                || charge_c[y] & (mask << (x + off1) >> 1)
                || charge_sl[x + y] & (mask << (y + off1) >> 1)
                || charge_sr[x - y + 11] & (mask << (y + off1) >> 1)
                || jump_r[x] & (1 << (y + off1))
                || jump_c[y] & (1 << (x + off1))
                || jump_sl[x + y] & (1 << (y + off1))
                || jump_sr[x - y + 11] & (1 << (y + off1))
            )) {
                // b.set(x, y, turn);
                // assert(win(x, y, M, N, b, turn));
                return turn;
            }
            if (
                charge_r[x] & (mask << (y + off2) >> 1)
                || charge_c[y] & (mask << (x + off2) >> 1)
                || charge_sl[x + y] & (mask << (y + off2) >> 1)
                || charge_sr[x - y + 11] & (mask << (y + off2) >> 1)
                || jump_r[x] & (1 << (y + off2))
                || jump_c[y] & (1 << (x + off2))
                || jump_sl[x + y] & (1 << (y + off2))
                || jump_sr[x - y + 11] & (1 << (y + off2))
            ) {
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
        b.set(x, y, turn);
        if (BAN_X == x - 1 && BAN_Y == y) {
            top[y]--;
        }
        avail &= ~((top[y] <= 0) << y);
        if (avail == 0) {
            return 3;
        }
        // assert(!win(x, y, M, N, b, turn));
        // update charge and jump
        charge_r[x] = hot(b.rows[x]);
        charge_c[y] = hot(b.cols[y]);
        charge_sl[x + y] = hot(b.slanted_left[x + y]);
        charge_sr[x - y + 11] = hot(b.slanted_right[x - y + 11]);
        jump_r[x] = jump(b.rows[x]);
        jump_c[y] = jump(b.cols[y]);
        jump_sl[x + y] = jump(b.slanted_left[x + y]);
        jump_sr[x - y + 11] = jump(b.slanted_right[x - y + 11]);
        turn = 3 - turn;
        last_y = y;
    }
}
