#include "BMPProcessor.h"
#include <fstream>
#include <cstdint>
#include <cmath>
#include <iostream>

bool BMPProcessor::load(const std::string& filename){
    //0. open file
    std::fstream infile(filename, std::ios::in | std::ios::binary);
    if (!infile.is_open()) {std::cout << "[ The file is not opened! ]" << std::endl; return false;}

    //1. assign all the 54 byte to BMPFileHeader and BMPInfoHeader
    infile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    infile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    
    //2. check filetype=BM and bitcount=24 and compression=0
    if (fileHeader.filetype != 0x4D42) {std::cout << "[ It is not a bmp! ]" << std::endl; return false;}
    if (infoHeader.bitcount != 24) {std::cout << "[ bitcount is not 24! ]" << std::endl; return false;}
    if (infoHeader.compression != 0) {std::cout << "[ compression is not 0! ]" << std::endl; return false;}

    int32_t absheight = std::abs(infoHeader.height);
    //3. set vector (pixels)
    pixels.resize(infoHeader.width*absheight*3);

    //4. jump to pixel data using offset
    infile.seekg(fileHeader.offset, std::ios::beg);

    //5. find padding
    uint8_t padding = (4-(3*infoHeader.width % 4)) % 4;

    //6. assign the pixel data and skip padding
    for (uint32_t i=0; i<absheight; i++){
        infile.read(reinterpret_cast<char*>(&pixels[i*infoHeader.width*3]), 3*infoHeader.width);
        infile.seekg(padding, std::ios::cur);
    }

    //7. close the file
    infile.close(); 

    std::cout << "......bmp file is sucessfully loaded" << std::endl;
    return true;
}



bool BMPProcessor::save(const std::string& filename){
    //0. open file
    std::fstream outfile(filename, std::ios::out | std::ios::binary);
    if (!outfile.is_open()) {std::cout << "[ The file is not opened! ]" << std::endl; return false;}

    //1. write 54 bytes header
    outfile.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    outfile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

    //2. write pixel and add padding
    uint8_t padding = (4-(3*infoHeader.width % 4)) % 4;
    int32_t absheight = std::abs(infoHeader.height);
    uint8_t zero = 0x00;

    for (int i=0; i<absheight; i++){
        outfile.write(reinterpret_cast<const char*>(&pixels[i*3*infoHeader.width]), 3*infoHeader.width);
        for (int j=0; j<padding; j++)
            outfile.write(reinterpret_cast<const char*>(&zero), sizeof(zero));
    }

    //3. close file
    outfile.close();

    std::cout << "......image is successfully saved to a new file" << std::endl;
    return true;
}



void BMPProcessor::applyGrayscale(){
    //0. start the loop
    uint8_t b, g, r;
    for (size_t i=0; i<pixels.size()/3; i++){
    //1. attach the BGR value of each pixel
        b = pixels[i*3];
        g = pixels[i*3+1];
        r = pixels[i*3+2];
    //2. calculate the grayscale (0.299, 0.587, 0.114)
        uint8_t gray = static_cast<uint8_t>(0.114*b + 0.587*g + 0.299*r);
    //3. replace the BGR with grayscale
        for (int j=0; j<3; j++){
            pixels[i*3+j] = gray;
        }
    }
    //4. ensure function is successfully compiled
    std::cout << "......Grayscale is calculated and saved!" << std::endl;
}



uint8_t BMPProcessor::getGray(int x, int y) const{
    //1. check out of image
    if (x<0 || x>=infoHeader.width || y<0 || y>=std::abs(infoHeader.height)) return 0;

    //2. return gray inside image
    return pixels[(infoHeader.width*y+x)*3];
}



bool BMPProcessor::applySobelEdgeDetection(){
    //0. start the loop
    std::vector<uint8_t> result;
    result.resize(infoHeader.width*std::abs(infoHeader.height)*3);
    for (int i=0; i<std::abs(infoHeader.height); i++){
    //1. compute the x-gradient & y-gradient by using getGray(x,y)
        for (int j=0; j<infoHeader.width; j++){

            int Gx{0}, Gy{0};
            Gx = -1*getGray(j-1, i-1) + 1*getGray(j+1, i-1)
               - 2*getGray(j-1, i)     + 2*getGray(j+1, i)
               - 1*getGray(j-1, i+1) + 1*getGray(j+1, i+1);

            Gy = -1*getGray(j-1, i-1) - 2*getGray(j, i-1) - 1*getGray(j+1, i-1)
               + 1*getGray(j-1, i+1) + 2*getGray(j, i+1) + 1*getGray(j+1, i+1);

    //2. calculate the mangitude
            double mag = std::sqrt(Gx*Gx + Gy*Gy);
            if (mag > 255) mag = 255;
            uint8_t G_magnitude = static_cast<uint8_t>(mag);

    //3. assign mangitude into the pixels vector (BGR)
            for (size_t k=3*(infoHeader.width*i+j); k<3*(infoHeader.width*i+j)+3; k++){
                result[k] = G_magnitude;
            }
        }
    }

    //4. turn result to pixels
    pixels = result;

    //5. return statement
    std::cout << "......edge detection is finished!" << std::endl;
    return true;

}