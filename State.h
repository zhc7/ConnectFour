//
// Created by zhc7 on 24-5-16.
//

#ifndef CONNECT4_STATE_H
#define CONNECT4_STATE_H


struct BoardRow {
    int row = 0;

    char operator[](int i) const {
        return (row >> (i << 1)) & 3;
    }

    void set(int i, char value) {
        row &= ~(3 << (i << 1));
        row |= value << (i << 1);
    }
};

struct Board {
    BoardRow rows[12];

    Board() = default;

    const BoardRow &operator[](int i) const {
        return rows[i];
    }

    void copyFrom(const Board &board) {
        for (int i = 0; i < 12; i++) {
            rows[i].row = board.rows[i].row;
        }
    }

    void set(int i, int j, char value) {
        rows[i].set(j, value);
    }

    Board(Board const &board) {
        copyFrom(board);
    }

    explicit Board(char **board, int M, int N) {
        for (auto & row : rows) {
            row.row = 0;
        }
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                set(i, j, board[i][j]);
            }
        }
    }
};


class State {
public:
    static int M;
    static int N;
    static int BAN_X;
    static int BAN_Y;
    Board board;
    char top[12]{};
    char nextTurn = 0;
    char mustWin = 0;

    State() = default;

    void step(int y, State &target) const;

    void copyFrom(const State &state);

    int simulate() const;

    short available() const;

protected:
    void _step(int y);
};


#endif //CONNECT4_STATE_H
