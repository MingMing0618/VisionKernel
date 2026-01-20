# VisionKernel: Sobel Edge Detection 

A C++ tool designed to process 24-bit BMP images using custom filters. This project performs grayscale conversion followed by Sobel Edge Detection to create a "Neon Blueprint" effect.



## Features
* **BMP Header Parsing**: Manually reads and validates BMP file structures without external libraries.
* **Grayscale Filter**: Converts BGR pixel data into luminance values using the formula: 0.114B + 0.587G + 0.299R.
* **Sobel Operator**: Implements $3 \times 3$ convolution kernels ($G_x$ and $G_y$) to detect horizontal and vertical edges.
* **Dynamic Padding**: Correctly handles BMP row padding to ensure image alignment.

## How to Compile and Run
**Compile**: g++ -O3 main.cpp BMPProcessor.cpp -o VisionKernel
**Run**: ./VisionKernel.exe

## Project Structure
**main.cpp**: Handles the user interface and program loop.
**BMPProcessor.cpp/h**: Contains the core logic for loading, filtering, and saving images.
