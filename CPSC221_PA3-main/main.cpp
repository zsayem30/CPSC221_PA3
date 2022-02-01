// File:        main.cpp
// Author:      cheeren
// Date:        2018-02-25
// Description: Partial test of PA3 functionality
//              Reads Point data from external files
//              Produces PNG images of the point sets

#include "cs221util/HSLAPixel.h"
#include "cs221util/PNG.h"
#include "twoDtree.h"

using namespace cs221util;
using namespace std;

int main() {
    // read in an image
    PNG origIm1;
    origIm1.readFromFile("images/canadaPlace.png");

    // use it to build a twoDtree
    twoDtree t1(origIm1);

    // make some copies for pruning tests
    twoDtree tCopy1(t1);
    twoDtree tCopy2(t1);
    twoDtree tCopy3(t1);
    twoDtree tCopy4(t1);

    // prune the twoDtree
    // where all subtree pixels are within Y of mean

    tCopy1.prune(.2);
    tCopy2.prune(.1);
    tCopy3.prune(.05);
    tCopy4.prune(.025);

    // render the twoDtree
    PNG ppic1 = t1.render();
    PNG ppiccopy1 = tCopy1.render();
    PNG ppiccopy2 = tCopy2.render();
    PNG ppiccopy3 = tCopy3.render();
    PNG ppiccopy4 = tCopy4.render();

    // write the pngs to files.
    ppic1.writeToFile("images/output-CP.png");
    ppiccopy1.writeToFile("images/output-CP.2.png");
    ppiccopy2.writeToFile("images/output-CP.1.png");
    ppiccopy3.writeToFile("images/output-CP.05.png");
    ppiccopy4.writeToFile("images/output-CP.025.png");

    return 0;
}
