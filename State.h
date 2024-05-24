//
// Created by zhc7 on 24-5-16.
//

#ifndef CONNECT4_STATE_H
#define CONNECT4_STATE_H

#include <cstring>

struct Board {
    int rows[12];
    int cols[12];

    Board() = default;

    void copyFrom(const Board &board) {
        std::memcpy(rows, board.rows, sizeof(rows));
        std::memcpy(cols, board.cols, sizeof(cols));
    }

    void set(int i, int j, char value) {
        rows[i] |= ((value == 1) << j) | ((value == 2) << (j + 16));
        cols[j] |= ((value == 1) << i) | ((value == 2) << (i + 16));
    }

    void unset(int i, int j) {
        rows[i] &= ~((1 << j) | (1 << (j + 16)));
        cols[j] &= ~((1 << i) | (1 << (i + 16)));
    }

    Board(Board const &board) {
        copyFrom(board);
    }

    explicit Board(char **board, int M, int N) {
        for (int i = 0; i < 12; i++) {
            rows[i] = 0;
            cols[i] = 0;
        }
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                set(i, j, board[i][j]);
            }
        }
    }
};

struct BoardSlanted : Board {
    int slanted_left[23];
    int slanted_right[23];

    BoardSlanted() = default;

    void copyFrom(const Board &board) {
        Board::copyFrom(board);
        std::memset(slanted_left, 0, sizeof(slanted_left));
        std::memset(slanted_right, 0, sizeof(slanted_right));
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 12; j++) {
                slanted_left[i + j] |= board.rows[i] & (0x10001 << j);
                slanted_right[i - j + 11] |= board.rows[i] & (0x10001 << j);
            }
        }
    }

    void set(int i, int j, char value) {
        Board::set(i, j, value);
        slanted_left[i + j] |= ((value == 1) << j) | ((value == 2) << (j + 16));
        slanted_right[i - j + 11] |= ((value == 1) << j) | ((value == 2) << (j + 16));
    }

    void unset(int i, int j) {
        Board::unset(i, j);
        slanted_left[i + j] &= ~((1 << j) | (1 << (j + 16)));
        slanted_right[i - j + 11] &= ~((1 << j) | (1 << (j + 16)));
    }

    explicit BoardSlanted(Board const &board) {
        copyFrom(board);
    }
};


class State {
public:
    static int M;
    static int N;
    static int BAN_X;
    static int BAN_Y;
    static char randomTable[30];
    static int randomIndex;
    Board board{};
    char top[12]{};
    char nextTurn = 0;
    char mustWin = 0;
    bool won = false;
    short avail = 0;

    State();

    void step(int y, State &target) const;

    void copyFrom(const State &state);

    int simulate() const;

protected:
    void _step(int y);
};


#endif //CONNECT4_STATE_H
