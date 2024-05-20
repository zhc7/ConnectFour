//
// Created by zhc7 on 24-5-16.
//

#ifndef CONNECT4_STATE_H
#define CONNECT4_STATE_H


struct Board {
    int rows[12];
    int cols[12];

    Board() = default;

    void copyFrom(const Board &board) {
        for (int i = 0; i < 12; i++) {
            rows[i] = board.rows[i];
            cols[i] = board.cols[i];
        }
    }

    void set(int i, int j, char value) {
        rows[i] |= ((value == 1) << j) | ((value == 2) << (j + 16));
        cols[j] |= ((value == 1) << i) | ((value == 2) << (i + 16));
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
