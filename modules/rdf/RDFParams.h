#ifndef RDFPARAMS_H
#define RDFPARAMS_H

#include "precompiled.h"
struct RDFParams
{
    std::map<quint16,quint16> pixelsPerLabelImage;
    QString trainImagesDir;
    QString trainAnnotationsDir;
    QString testDir;
    int probDistX;
    int probDistY;
    int nTrees;
    int maxDepth;
    int pixelsPerImage;
    int minLeafPixels;
    int labelCount;
    int maxIteration;
    int tauRange;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(probDistX, probDistY, nTrees, maxDepth, pixelsPerImage, minLeafPixels,
                labelCount, maxIteration);
    }
};

#endif // RDFPARAMS_H
