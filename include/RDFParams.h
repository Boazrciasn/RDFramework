#ifndef RDFPARAMS_H
#define RDFPARAMS_H

struct  RDFParams
{
    QString trainDir;
    QString testDir;
    int probDistX;
    int probDistY;
    int nTrees;
    int maxDepth;
    int pixelsPerImage;
    int minLeafPixels;
    int labelCount;
    int maxIteration;
};

#endif // RDFPARAMS_H
