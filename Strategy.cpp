#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include "Point.h"
#include "Strategy.h"

#include "mem.h"
#include "Node.h"
#include "State.h"

using namespace std;

#ifdef QUICKER
constexpr double TIME_LIMIT = 1.5;
#elif defined(SLOWER)
constexpr double TIME_LIMIT = 2.7;
#else
constexpr double TIME_LIMIT = 2.5;
#endif
static int step = 0;
static int rounds = 0;


void log(Node *node) {
    std::cerr
            << "nextTurn: " << (int) node->state.nextTurn << "\n"
            << "wins: "
            << Node::root->playerWins << " / "
            << Node::root->visits << " = "
            << (double) Node::root->playerWins / Node::root->visits
            << "\n"
            << "mustWin: " << (int) node->state.mustWin << "\n"
            << "children: \n";
    for (int i = 0; i < 12; i++) {
        std::cerr
                << "  " << i << ": ";
        if (node->children[i] == nullptr) {
            continue;
        }
        std::cerr
                << node->children[i]->playerWins << " / "
                << node->children[i]->visits << " = "
                << (double) node->children[i]->playerWins / node->children[i]->visits
                << "M" << (int) node->children[i]->state.mustWin
                << ";";
    }
    std::cerr << std::endl;
}

void log(int **board) {
    for (int i = 0; i < State::M; i++) {
        for (int j = 0; j < State::N; j++) {
            if (State::BAN_X == i && State::BAN_Y == j)
                std::cerr << 'X';
            else
                std::cerr << board[i][j];
            std::cerr << " ";
        }
        std::cerr << std::endl;
    }
}


/*
	策略函数接口,该函数被对抗平台调用,每次传入当前状态,要求输出你的落子点,该落子点必须是一个符合游戏规则的落子点,不然对抗平台会直接认为你的程序有误

	input:
		为了防止对对抗平台维护的数据造成更改，所有传入的参数均为const属性
		M, N : 棋盘大小 M - 行数 N - 列数 均从0开始计， 左上角为坐标原点，行用x标记，列用y标记
		top : 当前棋盘每一列列顶的实际位置. e.g. 第i列为空,则_top[i] == M, 第i列已满,则_top[i] == 0
		_board : 棋盘的一维数组表示, 为了方便使用，在该函数刚开始处，我们已经将其转化为了二维数组board
				你只需直接使用board即可，左上角为坐标原点，数组从[0][0]开始计(不是[1][1])
				board[x][y]表示第x行、第y列的点(从0开始计)
				board[x][y] == 0/1/2 分别对应(x,y)处 无落子/有用户的子/有程序的子,不可落子点处的值也为0
		lastX, lastY : 对方上一次落子的位置, 你可能不需要该参数，也可能需要的不仅仅是对方一步的
				落子位置，这时你可以在自己的程序中记录对方连续多步的落子位置，这完全取决于你自己的策略
		noX, noY : 棋盘上的不可落子点(注:涫嫡饫锔?龅膖op已经替你处理了不可落子点，也就是说如果某一步
				所落的子的上面恰是不可落子点，那么UI工程中的代码就已经将该列的top值又进行了一次减一操作，
				所以在你的代码中也可以根本不使用noX和noY这两个参数，完全认为top数组就是当前每列的顶部即可,
				当然如果你想使用lastX,lastY参数，有可能就要同时考虑noX和noY了)
		以上参数实际上包含了当前状态(M N _top _board)以及历史信息(lastX lastY),你要做的就是在这些信息下给出尽可能明智的落子点
	output:
		你的落子点Point
*/
extern "C" Point *getPoint(const int M, const int N, const int *top, const int *_board,
                           const int lastX, const int lastY, const int noX, const int noY) {
    /*
        不要更改这段代码
    */
    int x = -1, y = -1; //最终将你的落子点存到x,y中
    char **board = new char *[M];
    for (int i = 0; i < M; i++) {
        board[i] = new char[N];
        for (int j = 0; j < N; j++) {
            board[i][j] = (char) _board[i * N + j];
        }
    }

    /*
        根据你自己的策略来返回落子点,也就是根据你的策略完成对x,y的赋值
        该部分对参数使用没有限制，为了方便实现，你可以定义自己新的类、.h文件、.cpp文件
    */
    //Add your own code below

    auto start = chrono::system_clock::now();

    step++;
    if (Node::root == nullptr || lastY < 0 || Node::root->children[lastY] == nullptr) {
        freeNode(Node::root);
        step = 1;
        rounds++;

        // init State
        State::M = M;
        State::N = N;
        State::BAN_X = noX;
        State::BAN_Y = noY;
        State::randomIndex = 0;
        for (int i = 0; i < State::N; i++) {
            const int weight = round(std::min(i, State::N - i - 1) / 3.) + 1;
            for (int j = 0; j < weight; j++) {
                State::randomTable[State::randomIndex++] = i;
            }
        }

        // init root node
        const auto node = getNode();
        node->state.board = Board(board, M, N);
        node->state.nextTurn = 2;
        for (int i = 0; i < N; i++) {
            node->state.top[i] = (char) top[i];
        }
        Node::root = node;

        // more strict time for first move
        start -= chrono::milliseconds(500);
    } else {
        Node::root = Node::root->pick(lastY);
    }

    // root must be expanded
    for (int i = 0; i < N; i++) {
        if (Node::root->children[i] == nullptr) {
            Node::root->update(Node::root->expandAction(i));
        }
        Node::root->isLeaf = false;
    }

    int actualSearches = 0;
    bool mustWin = Node::root->state.mustWin != 0;
#ifdef DEBUG
    while (actualSearches < 250000
#else
    while (
        chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start).count() < TIME_LIMIT * 1000
#endif
        && MAX_SIZE - poolSize + freeStackTop > 100
        ) {
        std::vector<Node *> path;
        Node *node = Node::root;
        path.push_back(node);
        while (!node->isLeaf) {
            node = node->select();
            if (node->state.mustWin != 0) {
                // here node does not change when selecting, pop this node, let its parent select again
                path.pop_back();
                if (path.empty()) {
                    mustWin = true;
                    break;
                }
                node = path.back();
            } else {
                path.push_back(node);
            }
        }
        if (mustWin) {
            break;
        }
        const int result = node->expand();
        for (Node *n: path) {
            n->update(result);
        }
        actualSearches++;
    }

// #ifdef LOG
    std::cerr << "Enemy Select: " << lastX << " " << lastY << std::endl;
    std::cerr << "\n########----------#########" << std::endl;
    std::cerr << "Round " << rounds << " Step " << step << std::endl;
    std::cerr << "Actual searches: " << actualSearches << std::endl;
    log(Node::root);
// #endif

    double bestValue = -1;
    if (mustWin) {
        char winner = Node::root->state.mustWin;
        if (winner == 2) {
            for (int i = 0; i < N; i++) {
                auto child = Node::root->children[i];
                if (child != nullptr && child->state.mustWin == 2) {
                    y = i;
                    break;
                }
            }
        } else {
            double best = -1;
            for (int i = 0; i < N; i++) {
                auto child = Node::root->children[i];
                if (child != nullptr) {
                    if (child->visits > best) {
                        y = i;
                        best = child->visits;
                    }
                    if (child->state.mustWin == 3) {
                        break;
                    }
                }
            }
        }
    } else {
        y = -1;
        int best_y = -1;
        for (Node *child: Node::root->children) {
            y++;
            if (child == nullptr || child->state.mustWin == 1) {
                continue;
            }
            if (child->state.mustWin == 2) {
                // seems impossible to reach
                best_y = y;
                break;
            }
            double value = (double) child->playerWins / child->visits;
            if (value > bestValue) {
                bestValue = value;
                best_y = y;
            }
        }
        y = best_y;
    }

    x = top[y] - 1;

    if (Node::root->children[y] == nullptr) {
        Node::root->children[y] = getNode();
        Node::root->state.step(y, Node::root->children[y]->state);
    }
    Node::root = Node::root->pick(y);

// #ifdef LOG
    std::cerr << "Selected: " << x << " " << y << std::endl;
    std::cerr << "Best Value: " << bestValue << std::endl;
    log(Node::root);

    auto now = chrono::system_clock::now();
    std::cerr << "Actual Time: " << chrono::duration_cast<chrono::milliseconds>(now - start).count() << "ms" << std::endl;
    std::cerr << "Used Nodes: " << poolSize - freeStackTop << std::endl;
// #endif

    /*
        不要更改这段代码
    */
//	clearArray(M, N, board);
    return new Point(x, y);
}

/*
	getPoint函数返回的Point指针是在本so模块中声明的，为避免产生堆错误，应在外部调用本so中的
	函数来释放空间，而不应该在外部直接delete
*/
extern "C" void clearPoint(Point *p) {
    delete p;
    return;
}

/*
	清除top和board数组
*/
void clearArray(int M, int N, int **board) {
    for (int i = 0; i < M; i++) {
        delete[] board[i];
    }
    delete[] board;
}

/*
	添加你自己的辅助函数，你可以声明自己的类、函数，添加新的.h .cpp文件来辅助实现你的想法
*/
