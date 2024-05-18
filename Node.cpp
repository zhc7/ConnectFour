//
// Created by zhc7 on 24-5-16.
//

#include <cmath>
#include<algorithm>
#include "Node.h"

Node *Node::root = nullptr;

void Node::handleMustWin(char winner) {
    if (winner == 1) {
        playerWins = 0;
        visits = 1 << 20;
    } else if (winner == 2) {
        playerWins = 1 << 20;
        visits = 1 << 20;
    } else if (winner == 3) {
        playerWins = 1 << 19;
        visits = 1 << 20;
    }
    state.mustWin = winner;
}

double Node::ucbValue(double logParentVisit) const {

    double win_rate = playerWins / visits;
    if (state.nextTurn == 2) {
        // parent's next turn is opponent's turn
        win_rate = 1 - win_rate;
    }
    if (visits == 0) {
        return (double) ((random() % 10) << 10);
    }
    return win_rate + UCB_C * sqrt(2 * logParentVisit / visits);
}

Node *Node::select() {
    Node *selected = nullptr;
    double bestValue = -1;
    char candidateMustWin = -1;
    const double logVisits = log(visits);
    for (Node *child: children) {
        if (child == nullptr) {
            continue;
        }
        // have win -> must win; all lose -> must lose; lose and tie -> must tie
        candidateMustWin = std::max(candidateMustWin, child->state.mustWin);
        if (child->state.mustWin == state.nextTurn) {
            handleMustWin(state.nextTurn);
            return this;
        }
        if (child->state.mustWin != 0) {
            continue;
        }
        double uctValue = child->ucbValue(logVisits);
        if (uctValue > bestValue) {
            bestValue = uctValue;
            selected = child;
        }
    }
    if (selected == nullptr && candidateMustWin != -1) {
        handleMustWin(candidateMustWin);
        return this;
    }
    return selected;
}

Node *Node::expand() {
    const short avail = state.available();
    int j = 0;
    int indexes[12];
    for (int i = 0; i < State::N; i++) {
        if (!(avail & (1 << i)) || children[i] != nullptr) {
            continue;
        }
        indexes[j++] = i;
    }
    if (j == 1) {
        isLeaf = false;
    }
    const int target = indexes[random() % j];
    const auto child = new Node;
    state.step(target, child->state);
    children[target] = child;
    if (child->state.mustWin == state.nextTurn) {
        state.mustWin = state.nextTurn;
    }
    return child;
}

void Node::update(int winner) {
    visits++;
    playerWins += winner == 2;
    playerWins += (winner == 3) * 0.5;
}

Node *Node::pick(int y) {
    // clear other children
    for (int i = 0; i < State::N; i++) {
        if (i != y) {
            delete children[i];
            children[i] = nullptr;
        }
    }
    return children[y];
}

Node::~Node() {
    for (Node *child: children) {
        delete child;
    }
}


