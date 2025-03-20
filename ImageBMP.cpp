#include "ImageBMP.h"
#include "Utils.h"

#include <iostream>
#include <thread>


void read(const std::string& filepath, ImageRGB& image)
{
    std::cout << "read\n";
    image.data.clear();
    auto header = BMPFileHeader{};
    auto in = std::ifstream(filepath, std::ios::binary);
    loadHeader(in, header);
    if(header.biHeight <= 0 || header.biWidth <= 0)
        throw Exception("Высота или ширина изображения равны нулю");
    loadImageData(in, header, image);
    std::cout << "Загружено изображение " << filepath << std::endl;
    std::cout << "Размер: " << image.width << "x" << image.height << std::endl;
}

void loadHeader(std::ifstream& stream, BMPFileHeader& file_header)
{
    std::cout << "loadHeader\n";
    stream.seekg(0, std::ios_base::beg);
    stream.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
    if(!stream.good())
        throw Exception("Ошибка считывания заголовка BMP файла");
    if(file_header.bfType != BitMapFlag)
        throw Exception("В заголовке установлен флаг не BMP файла");
    if(file_header.biBitCount != 24) 
        throw Exception("Изображения не с 24 bpp не поддерживаются");
}

void loadImageData(std::ifstream& stream, const BMPFileHeader& header, ImageRGB& image)
{
    auto height = static_cast<size_t>(header.biHeight);
    auto width = static_cast<size_t>(header.biWidth);

    image.height = height;
    image.width = width;

    stream.seekg(header.bfOffBits, std::ios_base::beg);
    image.data.resize(height * width);

    const int32_t row_size = width * 3 + width  % 4;
    std::vector<uint8_t> line(row_size);
    for(int32_t y = height - 1; y >= 0; --y) 
    {
        stream.read(reinterpret_cast<char*>(line.data()), line.size());
        if(!stream.good()) 
            throw Exception("Ошибка считывания BMP изображения");
        size_t i = 0;
        for (int32_t x = 0; x < width; ++x) 
        {
            PixelRGB pixel{};
            pixel.b = line[i++];
            pixel.g = line[i++];
            pixel.r = line[i++];
            image.data[convert_2d_to_1d_coordinates(x, y, width)] = pixel;
        }
    }
}

void convertBlockRGB2YUV420p(const ImageRGB& rgbImage, FrameYUV420p& yuvFrame, size_t startY, size_t endY)
{
    for (size_t y = startY; y < endY; ++y)
    {
        for (size_t x = 0; x < rgbImage.width; ++x) 
        {
            size_t index = convert_2d_to_1d_coordinates(x, y, rgbImage.width);
            // Вычисляет Y
            yuvFrame.y[index] = calcYfromRGB(rgbImage.data[index]);
        }
    }
    size_t i = (rgbImage.width * startY) / 4;
    for (size_t y = startY; y < endY; y+=2)
    {
        for (size_t x = 0; x < rgbImage.width; x+=2) 
        {
            if(convert_2d_to_1d_coordinates(x+1, y+1, rgbImage.width) > rgbImage.width*rgbImage.height)
            {
                break;
            }

            // Вычисляет U и V
            int uSum = 0, vSum = 0;
            size_t index = convert_2d_to_1d_coordinates(x, y, rgbImage.width);
            uSum += calcUfromRGB(rgbImage.data[index]);
            uSum += calcUfromRGB(rgbImage.data[index + 1]);
            uSum += calcUfromRGB(rgbImage.data[index + rgbImage.width]);
            uSum += calcUfromRGB(rgbImage.data[index + rgbImage.width + 1]);

            vSum += calcVfromRGB(rgbImage.data[index]);
            vSum += calcVfromRGB(rgbImage.data[index + 1]);
            vSum += calcVfromRGB(rgbImage.data[index + rgbImage.width]);
            vSum += calcVfromRGB(rgbImage.data[index + rgbImage.width + 1]);

            yuvFrame.u[i] = static_cast<uint8_t>(uSum / 4);
            yuvFrame.v[i] = static_cast<uint8_t>(vSum / 4);
            ++i;
        }
    }
}

void convertParallelRGB2YUV420p(const ImageRGB& rgbImage, FrameYUV420p& yuvFrame, size_t nThreads)
{
    yuvFrame.height = rgbImage.height;
    yuvFrame.width = rgbImage.width;
    int lumaSize = rgbImage.width * rgbImage.height;
    int chromaSize = lumaSize / 4;
    yuvFrame.y.resize(lumaSize);
    yuvFrame.u.resize(chromaSize);
    yuvFrame.v.resize(chromaSize);

    std::vector<std::thread> threads;
    size_t yStep = (rgbImage.height / nThreads) + (rgbImage.height / nThreads) % 2;
    size_t remainderY = rgbImage.height - yStep * nThreads;
    for(size_t t = 0; t < nThreads; ++t)
    {
        size_t start = t * yStep;
        size_t end = (t == nThreads - 1) ? rgbImage.height : start + yStep;
        threads.emplace_back(convertBlockRGB2YUV420p, std::ref(rgbImage), std::ref(yuvFrame), start, end);
    }

    for( auto& t : threads)
    {
        t.join();
    }
    std::cout << "Конвертировано изображение RGB -> YUV\n";
}
