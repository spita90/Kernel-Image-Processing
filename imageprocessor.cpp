#include <iostream>
#include <thread>
#include <vector>
#include <QElapsedTimer>
#include <QPainter>
#include "imageprocessor.h"

using namespace MatAlg;

QImage ImageProcessor::applyEffect(QString effect, QImage image, uint threadNumber, qint64& time){
    Matrix<int> matrix(3,3,0);

    if(effect=="Box Blur"){
        matrix = {{1, 1, 1},
                  {1, 1, 1},
                  {1, 1, 1}};
    }
    else if(effect=="Gaussian Blur"){
        matrix = {{1, 2, 1},
                  {2, 4, 2},
                  {1, 2, 1}};
    }
    else{
        matrix = {{0, 0, 0},
                  {0, 1, 0},
                  {0, 0, 0}};
    }

    uint imageWidth=static_cast<uint>(image.width());
    uint imageHeight=static_cast<uint>(image.height());
    uint blockDimX;
    uint blockDimY;
    uint rows=0;
    uint columns=0;

    QString numThreads = threadNumber==1?("SINGLE THREAD"):(QString::number(threadNumber)+" THREADS");
    std::cout <<numThreads.toStdString()<<std::endl;
    std::cout <<"IMAGE ORIGINAL WIDTH: "<<imageWidth<<" AND HEIGHT: "<<imageHeight<<std::endl;

    if(threadNumber==1){
        rows=columns=1;
        blockDimX=imageWidth+2;
        blockDimY=imageHeight+2;
    }
    else{
        if(threadNumber<=4){
            if(imageWidth>imageHeight){
                rows = 1;
                columns = threadNumber;
            }else{
                rows = threadNumber;
                columns = 1;
            }
        }
        else if(threadNumber>=6){
            if(imageWidth>imageHeight){
                rows = 2;
                columns = threadNumber/2;
            }else{
                rows = threadNumber/2;
                columns = 2;
            }
        }
        if(imageWidth%columns!=0)
            imageWidth = (static_cast<uint>(imageWidth/columns)+1) * columns;
        if(imageHeight%rows!=0)
            imageHeight = (static_cast<uint>(imageHeight/rows)+1) * rows;
        blockDimX=(imageWidth/columns);
        blockDimY=(imageHeight/rows);
    }
    /* i blockDim si riferiscono alla suddivisione dell'immagine originale
     */

    std::cout <<"IMAGE ROUND UP WIDTH: "<<imageWidth<<" AND HEIGHT: "<<imageHeight<<std::endl;
    std::cout <<"ROWS: "<<rows<<" AND COLUMNS: "<<columns<<std::endl;
    std::cout <<"BLOCK SIZE: "<<blockDimX<<"x"<<blockDimY<<std::endl;

    /* aggiungo una cornice di un pixel intorno all'immagine
     * per i casi di bordo
     */

    QImage boxedImage(image.width()+2, image.height()+2, image.format());
    QColor null(0,0,0,0);
    boxedImage.fill(null);
    QPainter* painter = new QPainter(&boxedImage);
    painter->drawImage(QPoint(1,1),image);
    painter->end();
    delete painter;

    std::vector<std::thread> threads(threadNumber);
    std::vector<QImage> blocks(threadNumber);

    QElapsedTimer timer;
    timer.start();

    for(uint i =0; i<threadNumber;i++){
        QImage block = boxedImage.copy((i%columns)*blockDimX, (i/columns)*blockDimY, blockDimX+2, blockDimY+2);
        threads.at(i)=std::thread(processBlock, block, std::ref(blocks.at(i)), blockDimX, blockDimY, matrix);
    }
    for(uint i =0; i<threadNumber;i++){
        threads.at(i).join();
    }

    time=timer.elapsed();

    //riassembla qui l'immagine (QPainter non supporta il multi-threading)
    painter = new QPainter(&image);
    for(uint i=0; i<threadNumber;i++){
        painter->drawImage(static_cast<int>((i%columns)*blockDimX),static_cast<int>((i/columns)*blockDimY),blocks.at(i));
    }
    painter->end();
    delete painter;

    return image;
}

void ImageProcessor::processBlock(QImage block, QImage& output, int blockDimX, int blockDimY, Matrix<int> matrix) {
    //QImage block = boxedImage.copy((id%columns)*blockDimX, static_cast<int>(id/columns)*blockDimY, blockDimX+2, blockDimY+2);
    QImage temp(blockDimX,blockDimY,block.format());

    for(int i =1;i<block.width()-1;i++){
        for(int j=1;j<block.height()-1;j++){
            float red = 0;
            float green = 0;
            float blue = 0;
            uint matX=0;
            uint matY=0;
            int divider=0;
            for(int k = i-1;k<=i+1;k++){
                for(int l = j-1; l<=j+1;l++){
                    QColor pixelColor = block.pixel(k,l);
                    int multiplier = matrix.at(matX,matY);
                    // if multiplier is not zero, or if pixel is not at border
                    if(multiplier!=0&&(pixelColor.red()!=0||pixelColor.green()!=0||pixelColor.blue()!=0)){
                        red+=pixelColor.red()*multiplier;
                        green+=pixelColor.green()*multiplier;
                        blue+=pixelColor.blue()*multiplier;
                        divider+=multiplier;
                    }
                    matX++;
                }
                matX=0;
                matY++;
            }
            if(divider>1){
                red/=divider;
                red+=.5f;
                if(red>255)
                    red=255;
                else if(red<0)
                    red=0;

                green/=divider;
                green+=.5f;
                if(green>255)
                    green=255;
                else if(green<0)
                    green=0;

                blue/=divider;
                blue+=.5f;
                if(blue>255)
                    blue=255;
                else if(blue<0)
                    blue=0;
            }
            temp.setPixel(i-1,j-1, qRgb(static_cast<int>(red),static_cast<int>(green),static_cast<int>(blue)));
        }
    }
    output=temp.copy();
}
