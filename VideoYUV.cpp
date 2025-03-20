#include "VideoYUV.h"

#include <iostream>
#include <algorithm>

#include "Utils.h"

void overlayFramesYUV420p(const FrameYUV420p& upFrame, 
                          FrameYUV420p& downFrame)

{
    if(upFrame.height > downFrame.height || upFrame.width > downFrame.width)
        throw Exception("Размеры изображения больше размеров видео");

    for(size_t y = 0; y < upFrame.height - 1; ++y)
    {
        std::copy(upFrame.y.begin() + y * upFrame.width, 
                  upFrame.y.begin() + (y + 1) * upFrame.width,
                  downFrame.y.begin() + y * downFrame.width);
        if(y % 2 == 0)
        {
            size_t yUV = y / 2;
            size_t widthUpUV = upFrame.width / 2;
            size_t widthDownUV = downFrame.width / 2;
            std::copy(upFrame.u.begin() + yUV * widthUpUV, 
                      upFrame.u.begin() + (yUV + 1) * widthUpUV,
                      downFrame.u.begin() + yUV * widthDownUV);
            std::copy(upFrame.v.begin() + yUV * widthUpUV, 
                      upFrame.v.begin() + (yUV + 1) * widthUpUV,
                      downFrame.v.begin() + yUV * widthDownUV);
        }
    }
}

VideoYUVReader::VideoYUVReader(const std::string& filepath, 
                               size_t height, size_t width )
        : mHeight(height), mWidth(width)
{
    mVideoFileStream = std::ifstream(filepath, std::ios::binary);
    if(!mVideoFileStream.is_open())
        throw Exception("Ошибка открытия файла YUV:"  + filepath);
    std::cout << "Открыт файл YUV для чтения: " << filepath << std::endl;
}

bool VideoYUVReader::next(FrameYUV420p& frame) noexcept
{
    frame.height = mHeight;
    frame.width = mWidth;
    size_t lumaSize = mHeight*mWidth;
    size_t chromaSize = static_cast<size_t>(lumaSize / 4);
    frame.y.resize(lumaSize);
    frame.u.resize(chromaSize);
    frame.v.resize(chromaSize);

    mVideoFileStream.read(reinterpret_cast<char*>(frame.y.data()), lumaSize);
    mVideoFileStream.read(reinterpret_cast<char*>(frame.u.data()), chromaSize);
    mVideoFileStream.read(reinterpret_cast<char*>(frame.v.data()), chromaSize);

    return mVideoFileStream.good();
}

VideoYUVWriter::VideoYUVWriter(const std::string& filepath)
{
    mVideoFileStream = std::ofstream(filepath, std::ios::binary);
    if(!mVideoFileStream.is_open())
        throw Exception("Ошибка записи в YUV файл: " + filepath);
    std::cout << "Открыт файл YUV для для записи: " << filepath << std::endl;
}

bool VideoYUVWriter::writeFrame(FrameYUV420p& frame) noexcept
{
    mVideoFileStream.write(reinterpret_cast<char*>(frame.y.data()), frame.y.size());
    mVideoFileStream.write(reinterpret_cast<char*>(frame.u.data()), frame.u.size());
    mVideoFileStream.write(reinterpret_cast<char*>(frame.v.data()), frame.v.size());
    return mVideoFileStream.good();
}