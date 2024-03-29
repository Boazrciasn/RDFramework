#ifndef RDFPARAMS_H
#define RDFPARAMS_H

struct RDFParams
{
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

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(probDistX, probDistY, nTrees, maxDepth, pixelsPerImage, minLeafPixels,
                labelCount, maxIteration);
    }

    ~RDFParams()
    {
    }

};

#endif // RDFPARAMS_H
