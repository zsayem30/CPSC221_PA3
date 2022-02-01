#include "stats.h"

stats::stats(PNG &im) {
    // resize all private vectors
    sumHueX.resize(im.width());
    sumHueY.resize(im.width());
    sumSat.resize(im.width());
    sumLum.resize(im.width());
    hist.resize(im.width());
    for (unsigned x = 0; x < im.width(); x++) {
        sumHueX[x].resize(im.height(), 0);
        sumHueY[x].resize(im.height(), 0);
        sumSat[x].resize(im.height(), 0);
        sumLum[x].resize(im.height(), 0);
        hist[x].resize(im.height());
        for (unsigned y = 0; y < im.height(); y++) {
            hist[x][y].resize(36, 0);
        }
    }

    // initialize HSL channels and histogram of hues
    for (unsigned x = 0; x < im.width(); x++) {
        for (unsigned y = 0; y < im.height(); y++) {
            HSLAPixel *currPixel = im.getPixel(x, y);

            // initialize cumulative sum of hueX
            double currHueX = cos(currPixel->h * PI / 180);
            double aboveSumHueX = (y > 0) ? sumHueX[x][y - 1] : 0;
            double leftSumHueX = (x > 0) ? sumHueX[x - 1][y] : 0;
            double aboveLeftSumHueX =
                (x > 0 && y > 0) ? sumHueX[x - 1][y - 1] : 0;
            sumHueX[x][y] =
                currHueX + aboveSumHueX + leftSumHueX - aboveLeftSumHueX;

            // initialize cumulative sum of hueY
            double currHueY = sin(currPixel->h * PI / 180);
            double aboveSumHueY = (y > 0) ? sumHueY[x][y - 1] : 0;
            double leftSumHueY = (x > 0) ? sumHueY[x - 1][y] : 0;
            double aboveLeftSumHueY =
                (x > 0 && y > 0) ? sumHueY[x - 1][y - 1] : 0;
            sumHueY[x][y] =
                currHueY + aboveSumHueY + leftSumHueY - aboveLeftSumHueY;

            // initialize cumulative sum of saturation
            double currSat = currPixel->s;
            double aboveSumSat = (y > 0) ? sumSat[x][y - 1] : 0;
            double leftSumSat = (x > 0) ? sumSat[x - 1][y] : 0;
            double aboveLeftSat = (x > 0 && y > 0) ? sumSat[x - 1][y - 1] : 0;
            sumSat[x][y] = currSat + aboveSumSat + leftSumSat - aboveLeftSat;

            // initialize cumulative sum of luminance
            double currLum = currPixel->l;
            double aboveSumLum = (y > 0) ? sumLum[x][y - 1] : 0;
            double leftSumLum = (x > 0) ? sumLum[x - 1][y] : 0;
            double aboveLeftLum = (x > 0 && y > 0) ? sumLum[x - 1][y - 1] : 0;
            sumLum[x][y] = currLum + aboveSumLum + leftSumLum - aboveLeftLum;

            // initialize histogram of hue
            int k = currPixel->h / 10;
            vector<int> aboveHist =
                (y > 0) ? hist[x][y - 1] : vector<int>(36, 0);
            vector<int> leftHist =
                (x > 0) ? hist[x - 1][y] : vector<int>(36, 0);
            vector<int> aboveLeftHist =
                (x > 0 && y > 0) ? hist[x - 1][y - 1] : vector<int>(36, 0);
            hist[x][y][k]++;
            transform(hist[x][y].begin(), hist[x][y].end(), aboveHist.begin(),
                      hist[x][y].begin(), plus<int>());
            transform(hist[x][y].begin(), hist[x][y].end(), leftHist.begin(),
                      hist[x][y].begin(), plus<int>());
            transform(hist[x][y].begin(), hist[x][y].end(),
                      aboveLeftHist.begin(), hist[x][y].begin(), minus<int>());
        }
    }
}

long stats::rectArea(pair<int, int> ul, pair<int, int> lr) {
    int x0 = ul.first, y0 = ul.second;
    int x1 = lr.first, y1 = lr.second;
    return (x1 - x0 + 1) * (y1 - y0 + 1);
}

HSLAPixel stats::getAvg(pair<int, int> ul, pair<int, int> lr) {
    int x0 = ul.first, y0 = ul.second;
    int x1 = lr.first, y1 = lr.second;
    double hue = 0.0, hueX = 0.0, hueY = 0.0;
    double sat = 0.0;
    double lum = 0.0;

    if (x0 > 0 && y0 > 0) {
        // not at edge
        hueX = sumHueX[x1][y1] - sumHueX[x1][y0 - 1] - sumHueX[x0 - 1][y1] +
               sumHueX[x0 - 1][y0 - 1];
        hueY = sumHueY[x1][y1] - sumHueY[x1][y0 - 1] - sumHueY[x0 - 1][y1] +
               sumHueY[x0 - 1][y0 - 1];
        sat = sumSat[x1][y1] - sumSat[x1][y0 - 1] - sumSat[x0 - 1][y1] +
              sumSat[x0 - 1][y0 - 1];
        lum = sumLum[x1][y1] - sumLum[x1][y0 - 1] - sumLum[x0 - 1][y1] +
              sumLum[x0 - 1][y0 - 1];
    } else if (x0 == 0 && y0 > 0) {
        // at left edge
        hueX = sumHueX[x1][y1] - sumHueX[x1][y0 - 1];
        hueY = sumHueY[x1][y1] - sumHueY[x1][y0 - 1];
        sat = sumSat[x1][y1] - sumSat[x1][y0 - 1];
        lum = sumLum[x1][y1] - sumLum[x1][y0 - 1];
    } else if (x0 > 0 && y0 == 0) {
        // at upper edge
        hueX = sumHueX[x1][y1] - sumHueX[x0 - 1][y1];
        hueY = sumHueY[x1][y1] - sumHueY[x0 - 1][y1];
        sat = sumSat[x1][y1] - sumSat[x0 - 1][y1];
        lum = sumLum[x1][y1] - sumLum[x0 - 1][y1];
    } else {
        // at upper-left corner
        hueX = sumHueX[x1][y1];
        hueY = sumHueY[x1][y1];
        sat = sumSat[x1][y1];
        lum = sumLum[x1][y1];
    }

    hueX /= rectArea(ul, lr);
    hueY /= rectArea(ul, lr);
    hue = atan2(hueY, hueX) * 180 / PI;
    if (hue < 0) {
        hue += 360;
    } else if (hue >= 360) {
        hue -= 360;
    }
    sat /= rectArea(ul, lr);
    lum /= rectArea(ul, lr);

    return HSLAPixel(hue, sat, lum, 1.0);
}

double stats::entropy(pair<int, int> ul, pair<int, int> lr) {
    int x0 = ul.first, y0 = ul.second;
    int x1 = lr.first, y1 = lr.second;
    long area = rectArea(ul, lr);
    double entropy = 0.0;

    vector<int> distn(36, 0);
    if (x0 > 0 && y0 > 0) {
        // not at edge
        transform(distn.begin(), distn.end(), hist[x1][y1].begin(),
                  distn.begin(), plus<int>());
        transform(distn.begin(), distn.end(), hist[x1][y0 - 1].begin(),
                  distn.begin(), minus<int>());
        transform(distn.begin(), distn.end(), hist[x0 - 1][y1].begin(),
                  distn.begin(), minus<int>());
        transform(distn.begin(), distn.end(), hist[x0 - 1][y0 - 1].begin(),
                  distn.begin(), plus<int>());
    } else if (x0 == 0 && y0 > 0) {
        // at left edge
        transform(distn.begin(), distn.end(), hist[x1][y1].begin(),
                  distn.begin(), plus<int>());
        transform(distn.begin(), distn.end(), hist[x1][y0 - 1].begin(),
                  distn.begin(), minus<int>());
    } else if (x0 > 0 && y0 == 0) {
        // at upper edge
        transform(distn.begin(), distn.end(), hist[x1][y1].begin(),
                  distn.begin(), plus<int>());
        transform(distn.begin(), distn.end(), hist[x0 - 1][y1].begin(),
                  distn.begin(), minus<int>());
    } else {
        // at upper-left corner
        transform(distn.begin(), distn.end(), hist[x1][y1].begin(),
                  distn.begin(), plus<int>());
    }

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0) {
            entropy += ((double)distn[i] / (double)area) *
                       log2((double)distn[i] / (double)area);
        }
    }
    return -1 * entropy;
}

double stats::weightedSumEntropy(pair<int, int> ulul, pair<int, int> ullr,
                                 pair<int, int> lrul, pair<int, int> lrlr) {
    long area = rectArea(ulul, lrlr);
    long ulArea = rectArea(ulul, ullr);
    long lrArea = rectArea(lrul, lrlr);
    return (entropy(ulul, ullr) * ulArea / area) +
           (entropy(lrul, lrlr) * lrArea / area);
}
