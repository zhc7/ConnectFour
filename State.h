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

struct HeavyBoard : Board {
    int slanted_left[23];
    int slanted_right[23];
    int charge_r[12]{};
    int charge_c[12]{};
    int jump_r[12]{};
    int jump_c[12]{};
    int charge_sl[23]{};
    int jump_sl[23]{};
    int charge_sr[23]{};
    int jump_sr[23]{};

    HeavyBoard() = default;

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

    explicit HeavyBoard(Board const &board) {
        copyFrom(board);
        for (int i = 0; i < 12; i++) {
            charge_r[i] = hot(rows[i]);
            charge_c[i] = hot(cols[i]);
            jump_r[i] = jump(rows[i]);
            jump_c[i] = jump(cols[i]);
        }
        for (int i = 3; i < 20; i++) {
            charge_sl[i] = hot(slanted_left[i]);
            charge_sr[i] = hot(slanted_right[i]);
            jump_sl[i] = jump(slanted_left[i]);
            jump_sr[i] = jump(slanted_right[i]);
        }
    }

    static int hot(const int row) {
        return row & (row << 1) & (row << 2);
    }

    static int jump(const int row) {
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

    bool mustWin(const int x, const int y, const char p) const {
        constexpr int mask = 0b10001;
        const int off = (p - 1) * 16;
        return charge_r[x] & (mask << (y + off) >> 1)
               || charge_c[y] & (mask << (x + off) >> 1)
               || charge_sl[x + y] & (mask << (y + off) >> 1)
               || charge_sr[x - y + 11] & (mask << (y + off) >> 1)
               || jump_r[x] & (1 << (y + off))
               || jump_c[y] & (1 << (x + off))
               || jump_sl[x + y] & (1 << (y + off))
               || jump_sr[x - y + 11] & (1 << (y + off));
    }

    void update(const int x, const int y, const char p) {
        set(x, y, p);
        charge_r[x] = hot(rows[x]);
        charge_c[y] = hot(cols[y]);
        charge_sl[x + y] = hot(slanted_left[x + y]);
        charge_sr[x - y + 11] = hot(slanted_right[x - y + 11]);
        jump_r[x] = jump(rows[x]);
        jump_c[y] = jump(cols[y]);
        jump_sl[x + y] = jump(slanted_left[x + y]);
        jump_sr[x - y + 11] = jump(slanted_right[x - y + 11]);
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
    short avail = 0;

    State();

    void step(int y, State &target) const;

    void copyFrom(const State &state);

    int simulate(HeavyBoard &b, int last_y) const;

protected:
    void _step(int y);
};


#endif //CONNECT4_STATE_H
