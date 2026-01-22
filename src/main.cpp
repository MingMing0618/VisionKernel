#include "../include/BMPProcessor.h"
#include <iostream>
#include <string>


int main() {

    std::cout << "\033[2J\033[1;1H"; //clear screen

    while (1){
    BMPProcessor image;
    std::string file; std::string num;
    std::cout << " Welcome! The program is for editing bmp file" << std::endl;
    std::cout << " Here are three functions: " << std::endl 
    << " 1. Edge detect" << std::endl 
    << " 2. Blur" << std::endl
    << " 3. Sharpen" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << " Enter 'end' to end the program" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl << std::endl; 
    
    do{
        std::cout << " Enter the function number: ";
        std::cin >> num;
        if (num=="end") {std::cout << std::endl << " Program Ended" << std::endl; return 1;}
        if (num!="1" && num!="2" && num!="3") std::cout << " Invalid number, please enter again." << std::endl << std::endl;   
    } while (num!="1" && num!="2" && num!="3");

    do{
        std::cout << " Enter the file name: ";
        std::cin >> file;
        if (file.compare("end")==0)  { std::cout << std::endl << " Program Ended"; return 1;}
    } while (!image.load(file));

    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" <<std:: endl;
    std::cout << " Width: " << image.getWidth() << std::endl;
    std::cout << " Height " << image.getHeight() << std::endl;

    if (num=="1"){
        image.applyGrayscale(); 
        image.applySobelEdgeDetection();}
    else if (num=="2"){
        std::cout << std::endl << " Choose the scale of blurring (1-4) #1: less blur  #4: more blur" << std::endl 
        << " Scale: ";
        std::string scale;
        do{
            std::cin >> scale;
            if (scale=="end"){std::cout << std::endl << " Program Ended" << std::endl; return 1;}
            if (scale!="1" && scale!="2" && scale!="3" && scale!="4"){std::cout << " Invalid input! Please enter again." << std::endl << "Scale: ";}
        } while(scale!="1" && scale!="2" && scale!="3" && scale!="4");
        
        for (int i=0; i<std::stoi(scale)*5; i++){
            image.applyBlur();
        }
        std::cout << "......BGR values is seperated successfully!" << std::endl;
        std::cout << "......Image is successfully blurred!" << std::endl;
    }
    else if (num=="3"){
        image.applySharpen();
        std::cout << "......BGR values is seperated successfully!" << std::endl;

    }


    std::cout << std::endl << " Everything is done, please enter a new file address to show the processed image" << std::endl << std::endl;
    std::string newfile;
    do{
        std::cout << " New file name: ";
        std::cin >> newfile;
        if (newfile.compare("end")==0) { std::cout << std::endl << " Program Ended"; return 1;}
    } while(!image.save(newfile));

    std::cout << std::endl << " Processed image is saved to the new file, please check. Thank you for using!" 
    << std::endl           << "------------------------------------------------------------------------------"
    << std::endl << std::endl << std::endl;
    }
    return 0;
}


// g++ -O3 src/main.cpp src/BMPProcessor.cpp -I include -o VisionKernel
// ./VisionKernel.exe