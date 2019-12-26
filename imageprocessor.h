#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include "matrix.h"

using namespace MatAlg;

class ImageProcessor
{
public:
    static QImage applyEffect(QString effect, QImage image, uint threadNumber, qint64& time);

private:
    ImageProcessor(){}
    static void processBlock(QImage block, QImage& output, int blockDimX, int blockDimY, Matrix<int> matrix);
};

#endif // IMAGEPROCESSOR_H
