//
// Created by zhc7 on 24-5-16.
//

#include <cmath>
#include <algorithm>
#include "Node.h"

#include "Judge.h"
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

int Node::expand() {
    // find avail children actions
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
    return expandAction(target);
}

int Node::expandAction(const int target) {
    const auto child = getNode();
    const int x = state.top[target] - 1;
    state.step(target, child->state);
    children[target] = child;

    // check if must win
    HeavyBoard b(child->state.board);

    if (win(x, target, b, state.nextTurn)) {
        child->state.mustWin = state.nextTurn;
        state.mustWin = state.nextTurn;
        child->update(state.nextTurn);
        return state.nextTurn;
    }

    const auto turn = child->state.nextTurn;
    char only_child = -1;
    for (int i = 0; i < State::N; i++) {
        const int x = child->state.top[i] - 1;
        if (x >= 0) {
            if (b.mustWin(x, i, turn)) {
                child->state.mustWin = turn;
                break;
            }
            if (b.mustWin(x, i, 3 - turn)) {
                if (only_child == -1) {
                    only_child = i;
                } else {
                    only_child = -1; // use normal simulation branch later
                    child->state.mustWin = 3 - turn;
                }
            }
        }
    }
    int sim_winner;
    if (only_child == -1 || child->state.mustWin == turn) {
        sim_winner = child->state.simulate(b, target);
    } else {
        sim_winner = child->expandAction(only_child);
        child->isLeaf = false;
    }
    child->update(sim_winner);
    if (child->state.mustWin == state.nextTurn) {
        state.mustWin = state.nextTurn;
    }
    return sim_winner;
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
