#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

struct FrameYUV420p
{
    size_t height = 0;
    size_t width = 0;

    std::vector<uint8_t> y, u, v;
};

void overlayFramesYUV420p(const FrameYUV420p& upFrame, 
                          FrameYUV420p& downFrame);

class VideoYUVReader
{
public:
    VideoYUVReader(const std::string& filename, size_t height, size_t width);
    bool next(FrameYUV420p& frame) noexcept;

private:
    std::ifstream mVideoFileStream;
    size_t mWidth;
    size_t mHeight;
};

class VideoYUVWriter
{
public:
    VideoYUVWriter(const std::string& filename);
    bool writeFrame(FrameYUV420p& frame) noexcept;

private:
    std::ofstream mVideoFileStream;
};