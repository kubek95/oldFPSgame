#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <cassert>
#include <utility>
#include <array>
#include <cmath>

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
static const auto PI {std::acos(-1.f)};
struct Player
{
    std::size_t x;
    std::size_t y;
    std::size_t size{5};
    float sightDirection {PI*0.35f};
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

auto drawRectangle(std::vector<Pixel>& framebuffer, const Vertex& leftUpperVertex, const Vertex& rightLowerVertex, bool firstHalf = true) -> void
{
    Pixel white{0, 255, 255, 255};
    assert(leftUpperVertex.x <= rightLowerVertex.x);
    assert(leftUpperVertex.y <= rightLowerVertex.y);
    
    auto numberOfLines = rightLowerVertex.y - leftUpperVertex.y;
    auto widthOfLine = rightLowerVertex.x - leftUpperVertex.x;
    if (leftUpperVertex.x == rightLowerVertex.x) widthOfLine = 1;
    for(std::size_t i = 0; i < numberOfLines; ++i) {
        for(std::size_t j = 0; j < widthOfLine; ++j) {
            auto index = coordToIndex(j+leftUpperVertex.x, i+leftUpperVertex.y, 512);
            const auto lineNumber{(i+leftUpperVertex.y)};
            if (firstHalf) {
                index += lineNumber*512;
            } else {
                index += (lineNumber+1)*512;
            }
            framebuffer.at(index) = white;
        }
    }
}

auto drawMap(std::vector<Pixel>& frameBuffer, const std::array<const char, 257>& map) -> void
{
    for (std::size_t i = 0; i < 256; ++i) {
        if (map.at(i) != ' ') {
            auto [upperLeftX, upperLeftY] = indexToCoord(i, 16);
            upperLeftX *= 32;
            upperLeftY *= 32;
            //std::cout << i << "(" << upperLeftX << "," << upperLeftY << ")\n";
            drawRectangle(frameBuffer, {upperLeftX, upperLeftY}, {upperLeftX+32, upperLeftY+32});
        }
    }
}

auto isCollision(const Vertex& vertex, const std::array<const char, 257>& map) -> bool
{
    const auto scaledDownX = vertex.x/32;
    const auto scaledDownY = vertex.y/32;
    const auto mapIndex = coordToIndex(scaledDownX, scaledDownY, 16);
    return map.at(mapIndex) != ' ';
}

auto translateIndexToLeftSidePicture(std::size_t index, std::size_t y_coord) -> std::size_t
{
    return index + (512*y_coord);
}

auto draw3dWall(std::vector<Pixel>& frameBuffer, float distanceToWall, std::size_t xCoord) -> void
{
//    std::cout << "distance to wall from camera: " << distanceToWall << '\n';
    const auto wallHeight{static_cast<std::size_t>((10000/distanceToWall))};
    std::size_t yCoord{(512 - static_cast<std::size_t>(wallHeight))/2};
    std::cout << "wall height: " << wallHeight << '\n';
    drawRectangle(frameBuffer, {xCoord, yCoord}, {xCoord, yCoord+wallHeight}, false);
}

auto drawRay(std::vector<Pixel>& frameBuffer, Player& player, std::array<const char, 257>& map) -> float
{
    float c{0};
    for (; c<512; c+=2.f) {
       std::size_t x = static_cast<std::size_t>(std::cos(player.sightDirection)*c + float(player.x));
       std::size_t y = static_cast<std::size_t>(std::sin(player.sightDirection)*c + float(player.y));
       if (isCollision({x, y}, map)) {
           break;
       }
       const auto frameBufferIndex = coordToIndex(x, y, 512);
       frameBuffer.at(translateIndexToLeftSidePicture(frameBufferIndex, y)) = Pixel{0, 255, 255, 255};
   }
    return c;
}

auto drawFieldOfView(std::vector<Pixel>& frameBuffer, Player& player, std::array<const char, 257>& map) -> void
{
    constexpr int numberOfRays{512};
    constexpr float fieldOfView{3.14f*0.3f};
    constexpr float degreeBetweenRays{fieldOfView/numberOfRays};
    Player tempPlayer = player;
    for (unsigned int i{0}; i < numberOfRays; ++i) {
        tempPlayer.sightDirection += degreeBetweenRays;
        auto distance = drawRay(frameBuffer, tempPlayer, map);
        draw3dWall(frameBuffer, distance, i);
    }
}

auto initlializeFramebuffer(std::size_t w_height, std::size_t w_width, Pixel color) -> std::vector<Pixel>
{
    return std::vector<Pixel>{w_height*w_width, color};
//    for (std::size_t x = 0; x < w_width; ++x) {
//        for (std::size_t y = 0; y < w_height; ++y) {
//            std::uint8_t r = static_cast<std::uint8_t>(float(255*x)/float(w_width));
//            std::uint8_t g = static_cast<std::uint8_t>(float(255*y)/float(w_height));
//            std::uint8_t b = 0;
//            std::uint8_t a = 255;
//            Pixel p{r, g, b, a};
//            frameBuffer.push_back(p);
//        }
//    }
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
    constexpr std::size_t w_height = 512;
    constexpr std::size_t w_width = 1024;
    Pixel backgroundColor{255, 255, 255, 255};
    auto framebuffer = initlializeFramebuffer(w_height, w_width, backgroundColor);

    constexpr std::size_t numOfVerticalBlocks{16};
    constexpr std::size_t numOfHorizontBlocks{16};
    constexpr std::size_t mapSize = numOfHorizontBlocks*numOfVerticalBlocks+1; 
    std::array<const char, mapSize> map {
        "1111111111111111"\
        "1              1"\
        "1      11111   1"\
        "1     1        1"\
        "1     1        1"\
        "1     1  1111111"\
        "1     1        1"\
        "1   1111111    1"\
        "1   1   1      1"\
        "1   1   1      1"\
        "1   1   1111   1"\
        "11      1      1"\
        "1       1  11111"\
        "1  111111      1"\
        "1              1"\
        "1111111111111111"
    };
    drawMap(framebuffer, map);
    Player player{160, 55};
    drawRectangle(framebuffer, {player.x, player.y}, {player.x+player.size, player.y+player.size});
    drawFieldOfView(framebuffer, player, map);
    save_pixelmap(framebuffer, "./out.ppm", w_width, w_height);
    return 0;
}
