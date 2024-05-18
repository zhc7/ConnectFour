#include "Judge.h"

bool win(const int x, const int y, const int M, const int N, const Board &board) {
    char p = board[x][y];
    //横向检测
    int i, j;
    int count = 0;
    auto row = board[x];
    for (i = y; i >= 0; i--)
        if (row[i] != p)
            break;
    count += (y - i);
    for (i = y; i < N; i++)
        if (row[i] != p)
            break;
    count += (i - y - 1);
    if (count >= 4)
        return true;

    //纵向检测
    count = 0;
    for (i = x; i < M; i++)
        if (board[i][y] != p)
            break;
    count += (i - x);
    if (count >= 4)
        return true;

    //左下-右上
    count = 0;
    for (i = x, j = y; i < M && j >= 0; i++, j--)
        if (board[i][j] != p)
            break;
    count += (y - j);
    for (i = x, j = y; i >= 0 && j < N; i--, j++)
        if (board[i][j] != p)
            break;
    count += (j - y - 1);
    if (count >= 4)
        return true;

    //左上-右下
    count = 0;
    for (i = x, j = y; i >= 0 && j >= 0; i--, j--)
        if (board[i][j] != 2)
            break;
    count += (y - j);
    for (i = x, j = y; i < M && j < N; i++, j++)
        if (board[i][j] != 2)
            break;
    count += (j - y - 1);
    if (count >= 4)
        return true;

    return false;
}

bool isTie(const int N, const char *top) {
    bool tie = true;
    for (int i = 0; i < N; i++) {
        if (top[i] > 0) {
            tie = false;
            break;
        }
    }
    return tie;
}