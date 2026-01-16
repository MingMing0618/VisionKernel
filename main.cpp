#include "BMPProcessor.h"
#include <iostream>
#include <string>

int main() {
    BMPProcessor edge_detect;
    std::string file;
    std::cout << " Welcome! The program is for edge detection of bmp file" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " Enter 'end' to end the program" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl << std::endl; 
    do{
        std::cout << " Enter the file name: ";
        std::cin >> file;
        if (file.compare("end")==0)  { std::cout << std::endl << "Program Ended"; return 1;}
    } while (!edge_detect.load(file));

    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" <<std:: endl;
    std::cout << " Width: " << edge_detect.getWidth() << std::endl;
    std::cout << " Height " << edge_detect.getHeight() << std::endl;
    edge_detect.applyGrayscale(); 
    edge_detect.applySobelEdgeDetection();
    std::cout << std::endl << " Everything is done, please enter a new file address to show the processed image" << std::endl << std::endl;
    std::string newfile;
    do{
        std::cout << "New file name: ";
        std::cin >> newfile;
        if (newfile.compare("end")==0) { std::cout << std::endl << "Program Ended"; return 1;}
    } while(!edge_detect.save(newfile));

    std::cout << std::endl << " Processed image is saved to the new file, please check. Thank you for using!" << std::endl << std::endl;
    
    return 0;
}


//g++ -O3 main.cpp BMPProcessor.cpp -o VisionKernel
//./VisionKernel.exe