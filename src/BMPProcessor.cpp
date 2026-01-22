#include "../include/BMPProcessor.h"
#include <fstream>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <algorithm>

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

    //3. set vector (pixels)
    pixels.resize(getWidth()*getHeight()*3);

    //4. jump to pixel data using offset
    infile.seekg(fileHeader.offset, std::ios::beg);

    //5. find padding
    uint8_t padding = (4-(3*getWidth() % 4)) % 4;

    //6. assign the pixel data and skip padding
    for (uint32_t i=0; i<getHeight(); i++){
        infile.read(reinterpret_cast<char*>(&pixels[i*getWidth()*3]), 3*getWidth());
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
    uint8_t padding = (4-(3*getWidth() % 4)) % 4;
    uint8_t zero = 0x00;

    for (int i=0; i<getHeight(); i++){
        outfile.write(reinterpret_cast<const char*>(&pixels[i*3*getWidth()]), 3*getWidth());
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
    if (x<0 || x>=getWidth() || y<0 || y>=getHeight()) return 0;

    //2. return gray inside image
    return pixels[(getWidth()*y+x)*3];
}



bool BMPProcessor::applySobelEdgeDetection(){
    //0. start the loop
    std::vector<uint8_t> result;
    result.resize(getWidth()*getHeight()*3);
    for (int i=0; i<getHeight(); i++){
    //1. compute the x-gradient & y-gradient by using getGray(x,y)
        for (int j=0; j<getWidth(); j++){

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
            for (size_t k=3*(getWidth()*i+j); k<3*(getWidth()*i+j)+3; k++){
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

void BMPProcessor::SeperateBGR(std::vector<uint8_t> &B, std::vector<uint8_t> &G, std::vector<uint8_t> &R){

    for (int i=0; i<getHeight(); i++){

        for (int j=0; j<getWidth(); j++){
            B[i*getWidth()+j] = pixels[i*getWidth()*3+j*3];
            G[i*getWidth()+j] = pixels[i*getWidth()*3+j*3+1];
            R[i*getWidth()+j] = pixels[i*getWidth()*3+j*3+2];
        }
    }

}


uint8_t BMPProcessor::getSingleBGR(const std::vector<uint8_t> &colour, int x, int y) const{

    if (x<0){x=0;}
    if (x>=getWidth()){x=getWidth()-1;}
    if (y<0){y=0;}
    if (y>=getHeight()){y=getHeight()-1;}

    return colour[y*getWidth()+x];

}


bool BMPProcessor::applyBlur(){

    std::vector<uint8_t> B, G, R, result;
    B.resize(getWidth()*getHeight());
    G.resize(getWidth()*getHeight());
    R.resize(getWidth()*getHeight());
    result.resize(getWidth()*getHeight()*3);
    
    SeperateBGR(B, G, R);
    
    float addBrightness = 1.01;
    for (int y=0; y<getHeight(); y++){

        for (int x=0; x<getWidth(); x++){
            int index = y*getWidth()+x; 
            B[index] =  getSingleBGR(B,x-1,y-1)/16 + getSingleBGR(B,x,y-1)/8 + getSingleBGR(B,x+1,y-1)/16 +
                        getSingleBGR(B,x-1,y)/8    + getSingleBGR(B,x,y)/4   + getSingleBGR(B,x+1,y)/8 +
                        getSingleBGR(B,x-1,y+1)/16 + getSingleBGR(B,x,y+1)/8 + getSingleBGR(B,x+1,y+1)/16;
            B[index] *= addBrightness;
            if (B[index]>255) B[index]=255;

            G[index] =  getSingleBGR(G,x-1,y-1)/16 + getSingleBGR(G,x,y-1)/8 + getSingleBGR(G,x+1,y-1)/16 +
                        getSingleBGR(G,x-1,y)/8    + getSingleBGR(G,x,y)/4   + getSingleBGR(G,x+1,y)/8 +
                        getSingleBGR(G,x-1,y+1)/16 + getSingleBGR(G,x,y+1)/8 + getSingleBGR(G,x+1,y+1)/16;
            G[index] *= addBrightness;
            if (G[index]>255) G[index]=255;

            R[index] =  getSingleBGR(R,x-1,y-1)/16 + getSingleBGR(R,x,y-1)/8 + getSingleBGR(R,x+1,y-1)/16 +
                        getSingleBGR(R,x-1,y)/8    + getSingleBGR(R,x,y)/4   + getSingleBGR(R,x+1,y)/8 +
                        getSingleBGR(R,x-1,y+1)/16 + getSingleBGR(R,x,y+1)/8 + getSingleBGR(R,x+1,y+1)/16;
            R[index] *= addBrightness;
            if (R[index]>255) R[index]=255; 
        }
    }

    for (int i=0; i<getHeight(); i++){

        for (int j=0; j<getWidth(); j++){
            result[i*getWidth()*3 + j*3] = B[i*getWidth()+j];
            result[i*getWidth()*3 + j*3 + 1] = G[i*getWidth()+j];
            result[i*getWidth()*3 + j*3 + 2] = R[i*getWidth()+j];
        }
    }

    pixels = result;
    return true;

}

bool BMPProcessor::applySharpen(){

    std::vector<uint8_t> B, G, R, b, g, r, result; //b, g, r: store the new pixel value
    int t1, t2, t3; // temp for storing computation result
    B.resize(getWidth()*getHeight()); b.resize(getWidth()*getHeight());
    G.resize(getWidth()*getHeight()); g.resize(getWidth()*getHeight());
    R.resize(getWidth()*getHeight()); r.resize(getWidth()*getHeight());
    result.resize(getWidth()*getHeight()*3);
    
    SeperateBGR(B, G, R);
    b = B; g = G; r = R;
    
    for (int y=0; y<getHeight(); y++){

        for (int x=0; x<getWidth(); x++){
            int index = y*getWidth()+x; 
            t1 =  getSingleBGR(B,x-1,y-1)*0 + getSingleBGR(B,x,y-1)*-1 + getSingleBGR(B,x+1,y-1)*0 +
                        getSingleBGR(B,x-1,y)*-1  + getSingleBGR(B,x,y)*5    + getSingleBGR(B,x+1,y)*-1 +
                        getSingleBGR(B,x-1,y+1)*0 + getSingleBGR(B,x,y+1)*-1 + getSingleBGR(B,x+1,y+1)*0;
            if (t1>255) t1=255;
            if (t1<0) t1=0;
            b[index] = t1;

            t2 =  getSingleBGR(G,x-1,y-1)*0 + getSingleBGR(G,x,y-1)*-1 + getSingleBGR(G,x+1,y-1)*0 +
                        getSingleBGR(G,x-1,y)*-1  + getSingleBGR(G,x,y)*5    + getSingleBGR(G,x+1,y)*-1 +
                        getSingleBGR(G,x-1,y+1)*0 + getSingleBGR(G,x,y+1)*-1 + getSingleBGR(G,x+1,y+1)*0;
            if (t2>255) t2=255;
            if (t2<0) t2=0;
            g[index] = t2;

            t3 =  getSingleBGR(R,x-1,y-1)*0 + getSingleBGR(R,x,y-1)*-1 + getSingleBGR(R,x+1,y-1)*0 +
                        getSingleBGR(R,x-1,y)*-1  + getSingleBGR(R,x,y)*5    + getSingleBGR(R,x+1,y)*-1 +
                        getSingleBGR(R,x-1,y+1)*0 + getSingleBGR(R,x,y+1)*-1 + getSingleBGR(R,x+1,y+1)*0;
            if (t3>255) t3=255; 
            if (t3<0) t3=0;
            r[index] = t3;
        }
    }

    for (int i=0; i<getHeight(); i++){

        for (int j=0; j<getWidth(); j++){
            result[i*getWidth()*3 + j*3] = b[i*getWidth()+j];
            result[i*getWidth()*3 + j*3 + 1] = g[i*getWidth()+j];
            result[i*getWidth()*3 + j*3 + 2] = r[i*getWidth()+j];
        }
    }

    pixels = result;
    std::cout << "......Image is successfully sharpened!" << std::endl;
    return true;

}

