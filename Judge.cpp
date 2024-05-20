#include "Judge.h"


static bool rowWin(const short row, const int y) {
    const short row1 = row & (row << 1);
    const short row2 = row1 & (row1 << 2);
    return (row2 >> y) & 0b1111;
}

static bool get(const Board& board, const int x, const int y, const char p) {
    return (board.rows[x] >> ((p - 1) * 16)) & (1 << y);
}


bool win(const int x, const int y, const int M, const int N, const Board &board, const char p) {
    //横向检测
    int i, j;

    const short row = board.rows[x] >> ((p - 1) * 16);
    if (rowWin(row, y)) {
        return true;
    }
    /*
     * 0000ABCXDEF0
     * 000ABCXDEF00
     * 00ABCXDEF000
     * 0ABCXDEF0000
     */

    //纵向检测
    const short col = board.cols[y] >> ((p - 1) * 16);
    if (rowWin(col, x)) {
        return true;
    }

    //左下-右上
    int count = 0;
    for (i = x, j = y; i < M && j >= 0; i++, j--)
        if (!get(board, i, j, p))
            break;
    count += (y - j);
    for (i = x, j = y; i >= 0 && j < N; i--, j++)
        if (!get(board, i, j, p))
            break;
    count += (j - y - 1);
    if (count >= 4)
        return true;

    //左上-右下
    count = 0;
    for (i = x, j = y; i >= 0 && j >= 0; i--, j--)
        if (!get(board, i, j, p))
            break;
    count += (y - j);
    for (i = x, j = y; i < M && j < N; i++, j++)
        if (!get(board, i, j, p))
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