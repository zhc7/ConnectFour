//
// Created by zhc7 on 24-5-16.
//

#ifndef CONNECT4_STATE_H
#define CONNECT4_STATE_H


class State {
public:
    static int M;
    static int N;
    static int BAN_X;
    static int BAN_Y;
    int **board;
    int *top;
    int nextTurn;
    int mustWin = 0;

    State(int **board, int *top, int nextTurn);

    State *step(int y) const;

    int simulate() const;

    bool *available() const;

    ~State();

protected:
    void _step(int y);
};


#endif //CONNECT4_STATE_H
