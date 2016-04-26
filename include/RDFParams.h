#ifndef RDFPARAMS_H
#define RDFPARAMS_H

struct RDFParams
{
    QString m_train_dir;
    int probDistX;
    int probDistY;
    int nTrees;
    int maxDepth;
    int pixelsperImage;
    int minLeafPixels;
    int labelCount;
    int maxIteration;
};

#endif // RDFPARAMS_H
