/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"

twoDtree::Node::Node(pair<int, int> ul, pair<int, int> lr, HSLAPixel a)
    : upLeft(ul), lowRight(lr), avg(a), LT(NULL), RB(NULL) {}

twoDtree::~twoDtree() {
    clear();
}

twoDtree::twoDtree(const twoDtree &other) {
    copy(other);
}

twoDtree::twoDtree(PNG &imIn) {
    stats s(imIn);
    pair<int, int> ul(0, 0);
    pair<int, int> lr(imIn.width() - 1, imIn.height() - 1);
    width = imIn.width();
    height = imIn.height();
    root = buildTree(s, ul, lr, true);
}

twoDtree &twoDtree::operator=(const twoDtree &rhs) {
    if (this != &rhs) {
        clear();
        copy(rhs);
    }
    return *this;
}

PNG twoDtree::render() {
    PNG img(width, height);
    render(root, img);
    return img;
}

void twoDtree::render(Node *root, PNG &img) {
    if (root != NULL) {
        if (root->LT == NULL && root->RB == NULL) {
            // leaf, upLeft == lowRight
            int x0 = root->upLeft.first, y0 = root->upLeft.second;
            int x1 = root->lowRight.first, y1 = root->lowRight.second;
            for (int x = x0; x <= x1; x++) {
                for (int y = y0; y <= y1; y++) {
                    img.getPixel(x, y)->h = root->avg.h;
                    img.getPixel(x, y)->s = root->avg.s;
                    img.getPixel(x, y)->l = root->avg.l;
                    img.getPixel(x, y)->a = root->avg.a;
                }
            }
        } else {
            // not leaf, upLeft != lowRight
            render(root->LT, img);
            render(root->RB, img);
        }
    }
}

/**
 * prune function modifies tree by cutting off
 * subtrees whose leaves are all within tol of
 * the average pixel value contained in the root
 * of the subtree
 */
// FIXME: not working
void twoDtree::prune(double tol) {
    prune(root, tol);
}

void twoDtree::prune(Node *root, double tol) {
    if (root != NULL) {
        if (toPrune(root->LT, root->avg, tol)) {
            clear(root->LT);
        } else {
            prune(root->LT, tol);
        }
        if (toPrune(root->RB, root->avg, tol)) {
            clear(root->RB);
        } else {
            prune(root->RB, tol);
        }
    }
}

bool twoDtree::toPrune(Node *root, const HSLAPixel col, double tol) {
    if (root == NULL) {
        return false;
    } else if (root->LT == NULL && root->RB == NULL) {
        return col.dist(root->avg) < tol;
    }
    return toPrune(root->LT, col, tol) && toPrune(root->RB, col, tol);
}

void twoDtree::clear() {
    clear(root);
    root = NULL;
}

void twoDtree::clear(Node *subRoot) {
    if (subRoot != NULL) {
        clear(subRoot->LT);
        clear(subRoot->RB);
        subRoot = NULL;
        delete subRoot;
    }
}

void twoDtree::copy(const twoDtree &other) {
    width = other.width;
    height = other.height;
    root = copy(other.root);
}

twoDtree::Node *twoDtree::copy(const Node *other) {
    if (other == NULL) {
        return NULL;
    }
    Node *curr = new Node(other->upLeft, other->lowRight, other->avg);
    curr->LT = copy(other->LT);
    curr->RB = copy(other->RB);
    return curr;
}

twoDtree::Node *twoDtree::buildTree(stats &s, pair<int, int> ul,
                                    pair<int, int> lr, bool vert) {
    int x0 = ul.first, y0 = ul.second;
    int x1 = lr.first, y1 = lr.second;
    if (x0 < 0 || y0 < 0 || x1 >= width || y1 > height) {
        // FIXME: should not be called
        // outside of image
        return NULL;
    } else if (x0 > x1 || y0 > y1) {
        // FIXME: should not be called
        // invalid rectangle
        return NULL;
    }

    Node *curr = new Node(ul, lr, s.getAvg(ul, lr));

    if ((x1 == x0) && (y1 == y0)) {
        // no split (leaf node)
        curr->LT = NULL;
        curr->RB = NULL;
    } else if ((y1 == y0) || vert) {
        // vertical split
        double minSumEntropy = numeric_limits<double>::max();
        int xk = x0;
        for (int xi = x0; xi < x1; xi++) {
            pair<int, int> lul = ul;        // left, upper-left
            pair<int, int> llr(xi, y1);     // left, lower-right
            pair<int, int> rul(xi + 1, y0); // right, upper-left
            pair<int, int> rlr = lr;        // right, lower-right
            double sumEntropy = s.weightedSumEntropy(lul, llr, rul, rlr);
            if (sumEntropy <= minSumEntropy) {
                minSumEntropy = sumEntropy;
                xk = xi;
            }
        }
        curr->LT = buildTree(s, ul, pair<int, int>(xk, y1), false);
        curr->RB = buildTree(s, pair<int, int>(xk + 1, y0), lr, false);
    } else if ((x1 == x0) || !vert) {
        // horizontal spilt
        double minSumEntropy = numeric_limits<double>::max();
        int yk = y0;
        for (int yi = y0; yi < y1; yi++) {
            pair<int, int> uul = ul;        // upper, upper-left
            pair<int, int> ulr(x1, yi);     // upper, lower-right
            pair<int, int> lul(x0, yi + 1); // lower, upper-left
            pair<int, int> llr = lr;        // lower, lower-right
            double sumEntropy = s.weightedSumEntropy(uul, ulr, lul, llr);
            if (sumEntropy <= minSumEntropy) {
                minSumEntropy = sumEntropy;
                yk = yi;
            }
        }
        curr->LT = buildTree(s, ul, pair<int, int>(x1, yk), true);
        curr->RB = buildTree(s, pair<int, int>(x0, yk + 1), lr, true);
    }

    return curr;
}
