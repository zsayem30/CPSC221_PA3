/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 *
 */

#ifndef _TWODTREE_H_
#define _TWODTREE_H_

#include "cs221util/HSLAPixel.h"
#include "cs221util/PNG.h"
#include "stats.h"

#include <limits>
#include <utility>

using namespace std;
using namespace cs221util;

/**
 * twoDtree: This is a structure used in decomposing an image
 * into rectangles of similarly colored pixels.
 */

class twoDtree {
private:
    /**
     * The Node class is private to the tree class via the principle of
     * encapsulation---the end user does not need to know our node-based
     * implementation details.
     */
    class Node {
    public:
        Node(pair<int, int> ul, pair<int, int> lr,
             HSLAPixel a); // Node constructor

        pair<int, int> upLeft;
        pair<int, int> lowRight;
        HSLAPixel avg;
        Node *LT; // left or top child rectangle
        Node *RB; // right or bottom child rectangle
    };

public:
    /**
     * twoDtree destructor.
     * Destroys all of the memory associated with the
     * current twoDtree. This function should ensure that
     * memory does not leak on destruction of a twoDtree.
     */
    ~twoDtree();

    /**
     * Copy constructor for a twoDtree.
     *
     * @param other the twoDtree we are copying.
     */
    twoDtree(const twoDtree &other);

    /**
     * Constructor that builds a twoDtree out of the given PNG.
     * Every leaf in the tree corresponds to a pixel in the PNG.
     * Every non-leaf node corresponds to a rectangle of pixels
     * in the original PNG, represented by an (x,y) pair for the
     * upper left corner of the rectangle and an (x,y) pair for
     * lower right corner of the rectangle. In addition, the Node
     * stores a pixel representing the average color over the
     * rectangle.
     *
     * Every node's left and right children correspond to a partition
     * of the node's rectangle into two smaller rectangles. The node's
     * rectangle is split by the horizontal or vertical line that
     * results in the two smaller rectangles whose Information Gain
     * is as large as possible. (or correspondingly, whose weighted
     * sum of entropies is as small as possible. see online spec.)
     *
     * Note that splits will alternate between vertical and
     * horizontal, at every level of the tree, beginning with a
     * vertical split. A 1xw rectangle will only have vertical
     * splits remaining, and a hx1 rectangle will only have
     * horizontal splits remaining.
     *
     * The LT child of the node will contain the upper left corner
     * of the node's rectangle, and the RB child will contain the
     * lower right corner. (see illustrations within the spec.)
     *
     * This function will build the stats object used to score the
     * splitting lines. It will also call helper function buildTree.
     *
     * @param imIn the image to be constructed into a twoDtree.
     */
    twoDtree(PNG &imIn);

    /**
     * Overloaded assignment operator for twoDtrees.
     *
     * @param rhs the right hand side of the assignment statement.
     */
    twoDtree &operator=(const twoDtree &rhs);

    /**
     * Render returns a PNG image consisting of the pixels
     * stored in the tree. may be used on pruned trees. Draws
     * every leaf node's rectangle onto a PNG canvas using the
     * average color stored in the node.
     */
    PNG render();

    /**
     * Prune function trims subtrees as high as possible in the tree.
     * A subtree is pruned (cleared) if all of the subtree's leaves are within
     * tol of the average color stored in the root of the subtree.
     * Pruning criteria should be evaluated on the original tree, not
     * on any pruned subtree. (we only expect that trees would be pruned once.)
     */
    void prune(double tol);

private:
    Node *root; // ptr to the root of the twoDtree

    int height; // height of PNG represented by the tree
    int width;  // width of PNG represented by the tree

    /**
     * Destroys all dynamically allocated memory associated with the
     * current twoDtree class. Complete for PA3.
     * You may want a recursive helper function for this one.
     */
    void clear();

    /**
     * Destroys all dynamically allocated memory in the descendants of subRoot.
     * Private helper function for the clear function.
     *
     * @param subRoot the root of the twoDtree to be deleted.
     */
    void clear(Node *subRoot);

    /**
     * Copies the parameter other twoDtree into the current twoDtree.
     * Does not free any memory. Called by copy constructor and op=.
     *
     * @param other the twoDtree to be copied.
     */
    void copy(const twoDtree &other);

    /**
     * Returns the root ptr of the copied twoDtree. Prvate helper function for
     * the copy function.
     *
     * @param other the root of the twoDtree to be copied.
     */
    Node *copy(const Node *other);

    /**
     * Recursively builds the twoDtree according to the specification of the
     * constructor. Private helper function for the constructor.
     *
     * @param s contains the data used to split the rectangles.
     * @param ul upper left point of current node's rectangle.
     * @param lr lower right point of current node's rectangle.
     * @param vert indicates if the split should be vertical or not.
     */
    Node *buildTree(stats &s, pair<int, int> ul, pair<int, int> lr, bool vert);

    /**
     * Draws every leaf node's rectangle, of the given node root, onto the given
     * image img. Private helper function for the render function.
     *
     * @param root node of the twoDtree to be rendered.
     * @param img image on which the twoDtree is rendered.
     */
    void render(Node *root, PNG &img);

    /**
     * Prunes the twoDtree at the given node if all of the subtree's leaves
     * are within tol of the average color stored in the root of the subtree.
     * Private helper function for the prune function.
     *
     * @param root node of the twoDtree to be pruned.
     * @param tol tolerance factor of pruning.
     */
    void prune(Node *root, double tol);

    /**
     * Returns true if the twoDtree should be pruned at the given node. Private
     * helper function for the prune function.
     *
     * @param root node of the twoDtree to be pruned.
     * @param col average color to be compared with leaves.
     * @param tol tolerance factor of pruning.
     */
    bool toPrune(Node *root, const HSLAPixel col, double tol);
};

#endif
