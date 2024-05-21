//
// Created by zhc7 on 24-5-16.
//

#include <cmath>
#include<algorithm>
#include "Node.h"
#include "mem.h"

Node *Node::root = nullptr;

void Node::handleMustWin(char winner) {
    state.mustWin = winner;
}

float Node::ucbValue(const float sqrtLogParentVisit) const {
    return winRate + sqrtLogParentVisit * revSqrtVisit;
}

Node *Node::select() {
    Node *selected = nullptr;
    float bestValue = -1;
    char candidateMustWin = -1;
    const float sqrtLogVisit = UCB_C * sqrtf(2 * log(visits));
    for (Node *child: children) {
        if (child != nullptr) {
            // have win -> must win; all lose -> must lose; lose and tie -> must tie
            candidateMustWin = std::max(candidateMustWin, child->state.mustWin);
            if (child->state.mustWin == state.nextTurn) {
                handleMustWin(state.nextTurn);
                return this;
            }
            if (child->state.mustWin != 0) {
                continue;
            }
            const float uctValue = child->ucbValue(sqrtLogVisit);
            if (uctValue > bestValue) {
                bestValue = uctValue;
                selected = child;
            }
        }
    }
    if (selected == nullptr && candidateMustWin != -1) {
        handleMustWin(candidateMustWin);
        return this;
    }
    return selected;
}

Node *Node::expand() {
    const short avail = state.avail;
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
    const auto child = getNode();
    state.step(target, child->state);
    children[target] = child;
    if (child->state.mustWin == state.nextTurn) {
        state.mustWin = state.nextTurn;
    }
    return child;
}

void Node::update(const int winner) {
    visits++;
    playerWins += winner == 2;
    playerWins += (winner == 3) * 0.5;
    winRate = playerWins / visits;
    revSqrtVisit = 1 / sqrtf(visits);
    if (state.nextTurn == 2) {
        // parent's next turn is opponent's turn
        winRate = 1 - winRate;
    }
}

Node *Node::pick(int y) {
    // clear other children
    for (int i = 0; i < State::N; i++) {
        if (i != y) {
            freeNode(children[i]);
            children[i] = nullptr;
        }
    }
    return children[y];
}
