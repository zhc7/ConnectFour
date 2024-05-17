//
// Created by zhc7 on 24-5-16.
//

#include <cmath>
#include<algorithm>
#include "Node.h"

Node *Node::root = nullptr;

Node::Node(const State &state) : state(state), playerWins(0), visits(0) {
    if (state.mustWin != 0) {
        handleMustWin(state.mustWin);
    }
}

void Node::handleMustWin(int winner) {
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

double Node::ucbValue(int parentVisit) const {

    double win_rate = (double) playerWins / visits;
    if (state.nextTurn == 2) {
        // parent's next turn is opponent's turn
        win_rate = 1 - win_rate;
    }
    if (visits == 0) {
        return (double) ((random() % 10) << 10);
    }
    return win_rate + UCB_C * sqrt(2 * log(parentVisit) / visits);
}

Node *Node::select() {
    Node *selected = nullptr;
    double bestValue = -1;
    int candidateMustWin = -1;
    for (Node *child: children) {
        if (child == nullptr) {
            continue;
        }
        // have win -> must win; all lose -> must lose; lose and tie -> must tie
        candidateMustWin = std::max(candidateMustWin, child->state.mustWin);
        if (child->state.mustWin == state.nextTurn) {
            handleMustWin(state.nextTurn);
            return this;
        } else if (child->state.mustWin != 0) {
            continue;
        }
        double uctValue = child->ucbValue(visits);
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
    bool *avail = state.available();
    bool all = true;
    Node* selected = nullptr;
    for (int i = 0; i < State::N; i++) {
        if (!avail[i] || children[i] != nullptr) {
            continue;
        }
        if (selected == nullptr) {
            State *newState = state.step(i);
            Node *child = new Node(*newState);
            children[i] = child;
            if (child->state.mustWin == state.nextTurn) {
                state.mustWin = state.nextTurn;
            }
            selected = child;
        } else {
            all = false;
        }
    }
    if (all) {
        isLeaf = false;
    }
    return selected;
}

void Node::update(int winner) {
    visits++;
    playerWins += winner == 2;
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


