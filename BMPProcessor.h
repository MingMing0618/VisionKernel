#ifndef BMP_PROCESSOR_H
#define BMP_PROCESSOR_H

#include <string>
#include <vector>
#include <cstdint>

#pragma pack(push, 1) // Ensure no padding

struct BMPFileHeader {
    uint16_t filetype{0};       // 0-1: 'BM' (0x4D42)
    uint32_t filesize{0};       // 2-5: Total size of the file in bytes
    uint16_t reserved1{0};      // 6-7: Reserved; must be 0
    uint16_t reserved2{0};      // 8-9: Reserved; must be 0
    uint32_t offset{0};         // 10-13: The byte offset where the pixel array begins
};

struct BMPInfoHeader {
    uint32_t size{0};               // 14-17: Size of this header (40 bytes)
    int32_t width{0};               // 18-21: Width of the image in pixels
    int32_t height{0};              // 22-25: Height of the image in pixels
    uint16_t planes{1};             // 26-27: Number of color planes (1)
    uint16_t bitcount{0};           // 28-29: Bits per pixel (24)
    uint32_t compression{0};        // 30-33: 0 for no compression
    uint32_t sizeimage{0};          // 34-37: Size of the raw bitmap data
    int32_t xpixelspermeter{0};     // 38-41: Horizontal resolution (pixels per meter)
    int32_t ypixelspermeter{0};     // 42-45: Vertical resolution (pixels per meter)
    uint32_t colorsused{0};         // 46-49: Number of colors in the color palette
    uint32_t colorsimportant{0};    // 50-53: 0 means all colors are important
};

#pragma pack(pop) // Return to default packing (padding)

class BMPProcessor {
private:
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    
    /**
     * This vector stores the raw B, G, R values.
     * Since it's 24-bit, every 3 elements in the vector represent ONE pixel.
     * Format: [B, G, R, B, G, R, ...]
     */
    std::vector<uint8_t> pixels;

public:
    BMPProcessor() = default;

    bool load(const std::string& filename);
    bool save(const std::string& filename);

    void applyGrayscale();
    bool applySobelEdgeDetection();

    uint8_t getGray(int x, int y) const;
    int32_t getWidth() const { return infoHeader.width; }
    int32_t getHeight() const { return infoHeader.height; }
};

#endif