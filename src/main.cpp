#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <cassert>
#include <utility>

#include "Pixel.hpp"

//auto unpack_color(std::uint32_t encodedPixel) -> Pixel
//{
//    Pixel p;
//    p.r = encodedPixel & 255;
//    p.g = (encodedPixel >> 8) & 255 ;
//    p.b = (encodedPixel >> 16) & 255;
//    p.a = (encodedPixel >> 24) & 255; 
//    return p;
//}

//auto pack_color(const Pixel& pixel) -> std::uint32_t
//{
//    return (pixel.a << 24) + (pixel.b << 16) + (pixel.g << 8) + pixel.r;
//}
//
struct Player
{
    std::size_t xCoord;
    std::size_t yCoord;
};

struct Vertex
{
    std::size_t x;
    std::size_t y;
};

auto coordToIndex(std::size_t coordX, std::size_t coordY, std::size_t width) -> std::size_t
{
    assert(coordX <= width);
    return coordY*width + coordX;
}

auto indexToCoord(std::size_t index, std::size_t width) -> std::pair<std::size_t, size_t>
{
    return {index % width, index / width};
}

auto drawRectangle(std::vector<Pixel>& framebuffer, const Vertex& leftUpperVertex, const Vertex& rightLowerVertex) -> void
{
    Pixel white{255, 255, 255, 255};
    assert(leftUpperVertex.x <= rightLowerVertex.x);
    assert(leftUpperVertex.y <= rightLowerVertex.y);
    
    auto numberOfLines = rightLowerVertex.y - leftUpperVertex.y;
    auto widthOfLine = rightLowerVertex.x - leftUpperVertex.x;
    for(std::size_t i = 0; i < numberOfLines; ++i) {
        for(std::size_t j = 0; j < widthOfLine; ++j) {
            framebuffer.at(coordToIndex(j+leftUpperVertex.x, i+leftUpperVertex.y, 512)) = white;
        }
    }
}

auto save_pixelmap(const std::vector<Pixel>& pixelMap,
                   const std::string& path,
                   std::size_t width,
                   std::size_t height) -> void
{
    assert(pixelMap.size() == width*height);
    std::ofstream ofs{path, std::ios::binary};
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (const auto& pixel: pixelMap) {
        ofs << static_cast<char>(pixel.r) << static_cast<char>(pixel.g) << static_cast<char>(pixel.b);
    }
    ofs.close();
}

int main()
{
    constexpr std::size_t height = 512;
    constexpr std::size_t width = 512;
    std::vector<Pixel> framebuffer;
    for (std::size_t x = 0; x < width; ++x) {
        for (std::size_t y = 0; y < height; ++y) {
            std::uint8_t r = static_cast<std::uint8_t>(float(255*x)/float(width));
            std::uint8_t g = static_cast<std::uint8_t>(float(255*y)/float(height));
            std::uint8_t b = 0;
            std::uint8_t a = 255;
            Pixel p{r, g, b, a};
            framebuffer.push_back(p);
        }
    }
    std::vector<std::pair<Vertex, Vertex>> walls;
    walls.emplace_back(Vertex{0,0}, Vertex{512,20});
    walls.emplace_back(Vertex{0,20}, Vertex{20,512});
    walls.emplace_back(Vertex{492,20}, Vertex{512,512});
    walls.emplace_back(Vertex{20,492}, Vertex{492,512}); // boarder
    walls.emplace_back(Vertex{200,70}, Vertex{362,90});
    walls.emplace_back(Vertex{180,90}, Vertex{200,200});
    walls.emplace_back(Vertex{270,135}, Vertex{492,155});
    walls.emplace_back(Vertex{130,200}, Vertex{270,220});
    walls.emplace_back(Vertex{130,220}, Vertex{150,270});
    walls.emplace_back(Vertex{250,220}, Vertex{270,420});
    walls.emplace_back(Vertex{70,420}, Vertex{270,440});
    walls.emplace_back(Vertex{270,270}, Vertex{380,290});
    walls.emplace_back(Vertex{360,350}, Vertex{492,370});
    for (const auto& el: walls) {
        drawRectangle(framebuffer, el.first, el.second);
    }
    save_pixelmap(framebuffer, "./out.ppm", width, height);
    std::cout << "hello from the other side\n";
    return 0;
}
