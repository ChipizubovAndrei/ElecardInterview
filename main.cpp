#include <iostream>
#include <fstream>
#include <cstdint>

#include "ImageBMP.h"
#include "VideoYUV.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
    if (argc < 7) 
    {
        std::cerr << "Ожидается 6 параметров:\n" 
        << "\t1. Имя файла изображения BMP\n"
        << "\t2. Имя файла YUV\n"
        << "\t3. Высота кадра видео\n"
        << "\t4. Ширина кадра видео\n" 
        << "\t5. Имя файла выходного видео YUV\n"
        << "\t6. Количество потоков для функции конвертации\n"
        << std::endl;
        return 1;
    }

    try
    {
        std::string bmp_filepath{argv[1]};
        std::string yuv_video_filepath{argv[2]};
        size_t height = std::atoi(argv[3]);
        size_t width = std::atoi(argv[4]);
        std::string output_video_filepath{argv[5]};
        size_t n_threads = std::atoi(argv[6]);

        auto bmpImage = ImageRGB{};
        read(bmp_filepath, bmpImage);

        auto yuvConvertedFrame = FrameYUV420p{};
        convertParallelRGB2YUV420p(bmpImage, yuvConvertedFrame, n_threads);

        auto videoFrame = FrameYUV420p{};
        auto reader = VideoYUVReader(yuv_video_filepath, height, width);
        auto writer = VideoYUVWriter(output_video_filepath);

        while(reader.next(videoFrame))
        {
            overlayFramesYUV420p(yuvConvertedFrame, videoFrame);
            if(!writer.writeFrame(videoFrame)) std::cerr << "Failed write frame\n";
        }
        std::cout << "Видео YUV успешно сохранено в файл: " << output_video_filepath << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}