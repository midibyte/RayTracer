#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h" //Vec3f definition

void render() {
    const int height = 768;
    const int width = 1024;
    std::vector<Vec3f> framebuffer(width*height);

    for (size_t j = 0; j < height; j++)
    {
        for(size_t i = 0; i < width; i++)
        {
            // set each RGB pixel as fraction of width and height value
            // will be a float value in range [0, 1], convert to [0, 255] later
            framebuffer[i + j*width] = Vec3f(j/float(height), i/float(width), 0);
        }
    }
    
    // time to save the image
    std::ofstream ofs;
    // open the file stream for writing
    ofs.open("./out.ppm");
    // write the header of the ppm file to define image dimensions
    ofs << "P6\n" << width << " " << height << "\n255\n";
    // write each pixel to file
    for(size_t i = 0; i < width*height; i++)
    {
        for (size_t j =0; j<3; j++)
        {
            // convert float to char, ensure that value is within [0, 255] range and non negative
            ofs << (char)(255 * std::max( std::min(framebuffer[i][j], 1.f), 0.f));
        }
    }
    // close the file stream
    ofs.close();
}

int main ()
{
    render();
    return 0;
}