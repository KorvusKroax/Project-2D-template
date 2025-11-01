#include "canvas.h"

#include "lodepng/lodepng.h"

#include <vector>
#include <cstring> // memset

Canvas::Canvas()
{}

Canvas::Canvas(int width, int height):
    width(width), height(height), pixelBuffer(width * height * 4, 0)
{}

Canvas::Canvas(const char* fileName)
{
    loadImage_PNG(fileName);
}

void Canvas::resize(int newWidth, int newHeight, unsigned char fill)
{
    this->width = newWidth;
    this->height = newHeight;
    this->pixelBuffer.assign(this->width * this->height * 4, fill);
}

void Canvas::clear()
{
    memset(this->pixelBuffer.data(), 0, this->pixelBuffer.size());
}

void Canvas::fill(Color color)
{
    for (int i = 0; i < this->pixelBuffer.size(); i += 4) {
        pixelBuffer[i + 0] = color.r;
        pixelBuffer[i + 1] = color.g;
        pixelBuffer[i + 2] = color.b;
        pixelBuffer[i + 3] = color.a;
    }
}

void Canvas::fastFill(Color color)
{
    const size_t pixelCount = this->width * this->height;
    unsigned char c[4] = { color.r, color.g, color.b, color.a };

    std::memcpy(this->pixelBuffer.data(), c, 4);

    size_t filled = 1;
    while (filled < pixelCount) {
        size_t copyCount = std::min(filled, pixelCount - filled);
        std::memcpy(this->pixelBuffer.data() + filled * 4, this->pixelBuffer.data(), copyCount * 4);
        filled += copyCount;
    }
}

bool Canvas::setPixel(int x, int y, Color color)
{
    if (color.a == 0 || x < 0 || x >= this->width || y < 0 || y >= this->height) return false;

    int index = (x + y * this->width) * 4;
    if (color.a < 255) {
        color = Color(
            this->pixelBuffer[index + 0],
            this->pixelBuffer[index + 1],
            this->pixelBuffer[index + 2],
            this->pixelBuffer[index + 3]).blend(color);
    }
    this->pixelBuffer[index + 0] = color.r;
    this->pixelBuffer[index + 1] = color.g;
    this->pixelBuffer[index + 2] = color.b;
    this->pixelBuffer[index + 3] = color.a;

    return true;
}

bool Canvas::getPixel(int x, int y, Color* color)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height) return false;

    int index = (x + y * width) * 4;
    *color = Color(
        this->pixelBuffer[index + 0],
        this->pixelBuffer[index + 1],
        this->pixelBuffer[index + 2],
        this->pixelBuffer[index + 3]
    );

    return true;
}

bool Canvas::setPixels(int x, int y, const Canvas* canvas)
{
    int destStartX = std::max(0, x);
    int destStartY = std::max(0, y);
    int destEndX = std::min(this->width, x + canvas->width);
    int destEndY = std::min(this->height, y + canvas->height);

    if (destStartX >= destEndX || destStartY >= destEndY) return false;

    int srcOffsetX = destStartX - x;
    int srcOffsetY = destStartY - y;

    for (int i = 0; i < destEndY - destStartY; i++) {
        const int destIndex = (destStartX + (destStartY + i) * this->width) * 4;
        const int srcIndex  = (srcOffsetX + (srcOffsetY + i) * canvas->width) * 4;
        std::memcpy(&this->pixelBuffer[destIndex], &canvas->pixelBuffer[srcIndex], (destEndX - destStartX) * 4);
    }

    return true;
}

bool Canvas::getPixels(int x, int y, int width, int height, Canvas* canvas)
{
    canvas->resize(width, height);

    int srcStartX = std::max(0, x);
    int srcStartY = std::max(0, y);
    int srcEndX   = std::min(this->width,  x + width);
    int srcEndY   = std::min(this->height, y + height);

    if (srcStartX >= srcEndX || srcStartY >= srcEndY)
        return false;

    int destOffsetX = srcStartX - x;
    int destOffsetY = srcStartY - y;

    for (int j = 0; j < srcEndY - srcStartY; ++j) {
        const int srcIndex  = (srcStartX + (srcStartY + j) * this->width) * 4;
        const int destIndex = (destOffsetX + (destOffsetY + j) * width) * 4;
        std::memcpy(&canvas->pixelBuffer[destIndex], &this->pixelBuffer[srcIndex], (srcEndX - srcStartX) * 4);
    }

    return true;
}

bool Canvas::loadImage_PNG(const char* fileName)
{
    unsigned int imageWidth, imageHeight, channelCount = 4;
    unsigned char* image;
    unsigned int error = lodepng_decode32_file(&image, &imageWidth, &imageHeight, fileName);
    if (error) {
        printf("ERROR: Canvas::loadImage_PNG() -- %i\n", error);
        return false;
    }

    resize(imageWidth, imageHeight);

    for (int y = 0; y < this->height; y++) {
        std::memcpy(
            this->pixelBuffer.data() + y * this->width * channelCount,
            image + (this->height - 1 - y) * this->width * channelCount,
            this->width * channelCount
        );
    }
    free(image);

    return true;
}
