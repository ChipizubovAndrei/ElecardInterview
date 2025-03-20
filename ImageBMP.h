#pragma once

#include <string>
#include <vector>
#include <fstream> 
#include <cstdint>

#include "VideoYUV.h"

constexpr uint16_t BitMapFlag = 0x4d42;

struct PixelRGB
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;;
};

struct ImageRGB
{
    size_t height = 0;
    size_t width = 0;
    std::vector<PixelRGB> data;
};

/*
Формулы перевода из RGB 24bpp -> YUV444 24bpp
Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;
*/
inline uint8_t calcYfromRGB(const PixelRGB& pixel)
{
    return ((66 * pixel.r + 129 * pixel.g + 25 * pixel.b + 128) >> 8) + 16;
}

inline uint8_t calcUfromRGB(const PixelRGB& pixel)
{
    return ((-38 * pixel.r - 74 * pixel.g + 112 * pixel.b + 128) >> 8) + 128;
}

inline uint8_t calcVfromRGB(const PixelRGB& pixel)
{
    return ((112 * pixel.r - 94 * pixel.g - 18 * pixel.b + 128) >> 8) + 128;
}


#pragma pack(push, 1)
struct BMPFileHeader
{
    uint16_t    bfType;         // тип файла                                            0
    uint32_t    bfSize;         // размер файла в байтах                                2
    uint16_t    bfReserved1;    //                                                      6
    uint16_t    bfReserved2;    //                                                      8
    uint32_t    bfOffBits;      // смещение к началу битового массива от начала файла   10
    uint32_t    biSize;             // размер структуры FileInfo                        14
    int32_t     biWidth;            // ширина изображения в пикселях                    18
    int32_t     biHeight;           // высота изображения в пискселях                   22
    uint16_t    biPlanes;           //                                                  26
    uint16_t    biBitCount;         // количество бит на один пиксель                   28
    uint32_t    biCompression;      // тип сжатия                                       30
    uint32_t    biSizeImage;        // размер картинки в байтах (если не сжата - 0)     34
    int32_t     biXPelsPerMeter;    //                                                  38
    int32_t     biYPelsPerMeter;    //                                                  42
    uint32_t    biClrUsed;          //                                                  46
    uint32_t    biClrImportant;     //                                                  50
};
#pragma pack(pop)

void read(const std::string& filepath, ImageRGB& image);
void loadHeader(std::ifstream& stream, BMPFileHeader& file_header);
void loadImageData(std::ifstream& stream, const BMPFileHeader& header, 
                   ImageRGB& image);

void convertParallelRGB2YUV420p(const ImageRGB& rgbImage, FrameYUV420p& yuvFrame, size_t nThreads);
void convertBlockRGB2YUV420p(const ImageRGB& rgbImage, FrameYUV420p& yuvFrame, size_t start, size_t end);