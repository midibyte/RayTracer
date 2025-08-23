#include "geometry.h"
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

const int height = 768;
const int width = 1024;

struct Material
{
    Material(const Vec3f &color) : diffuse_color(color) {}
    Material() : diffuse_color() {}
    Vec3f diffuse_color;
};


struct Sphere {
    // ray-sphere intersection reference
    // http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
    // orig = ray origin vector
    // dir = ray direction vector
    // L - distance from start of Ray to center of sphere
    // O (center) - center of sphere
    // d - distance from center of sphere to center of intersection line of ray and sphere
    // d = L tangent * L - tca^2 (vector multiply)
    // if d > radius -> no intersection
    // ray either intersects: 2 times (d<radius), once (d=radius) or not at all (d>radius)
    // tca = distance from ray origin to center intersection line (2 pts) or single intersection point
    // thc = distance from center of intersection point (2pts) or single point, to the perimeter of the sphere
    // t0 = tca - thc = distance from ray origin to first intersection point (t0 = tca if only one point)

    Vec3f center;
    float radius;
    Material material;

    Sphere(const Vec3f &c, const float &r, const Material &m) : center(c), radius(r), material(m) {}

    // does ray intersect with sphere
    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const {
        // transpose vector L 
        Vec3f L = center - orig;
        float tca = L * dir;
        float d2 = L*L - tca*tca;
        // if d > radius, no intersection with sphere
        if (d2 > radius*radius) return false;
        float thc = sqrtf(radius * radius -d2);
        t0 = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) t0 = t1;
        if (t0 < 0 )return false;
        return true;
    }
};


bool scene_intersect(const Vec3f &orig, const Vec3f &dir, const std::vector<Sphere> &spheres, Vec3f &hit, Vec3f &N, Material &material)
{
    // init to max value
    float spheres_dist = std::numeric_limits<float>::max();
    // iterate thru spheres
    for (size_t i =0; i < spheres.size(); i++) {
        float dist_i;
        if (spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist)
        {
            spheres_dist = dist_i;
            hit = orig + dir*dist_i;
            N =  (hit - spheres[i].center).normalize();
            material = spheres[i].material;
        }
    }
    return spheres_dist < 1000;
}

Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const std::vector<Sphere> &spheres){
    Vec3f point, N;
    Material material;

    if(!scene_intersect(orig, dir, spheres, point, N, material)){
        return Vec3f(0.2, 0.7, 0.8); // background color
    }

    return material.diffuse_color;
}


void render(const std::vector<Sphere> &spheres) {

    // camera is at the origin
    Vec3f camera(0,0,0);
    const int fov = M_PI/2.f;

    std::vector<Vec3f> framebuffer(width*height);

    for (size_t j = 0; j < height; j++)
    {
        for(size_t i = 0; i < width; i++)
        {   
            // each pixel will be a float value in range [0, 1], convert to [0, 255] later
            // check intersection with sphere
            float x = (2*(i + 0.5)/(float)width - 1)*tan(fov/2.f)*width/(float)height;
            float y = -(2*(j + 0.5)/(float)height - 1)*tan(fov/2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            framebuffer[i+j*width] = cast_ray(camera, dir, spheres);
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
            // convert float to char, ensure that value is within [0, 255] range and non-negative
            ofs << (char)(255 * std::max( std::min(framebuffer[i][j], 1.f), 0.f));
        }
    }
    // close the file stream
    ofs.close();
}

int main ()
{
    // colors for spheres
    Material offwhite   (Vec3f(0.4, 0.4, 0.3));
    Material red        (Vec3f(0.3, 0.1, 0.1));

    // make some spheres to be rendered
    std::vector<Sphere> spheres;
    spheres.push_back(Sphere(Vec3f(-3, 0, -16), 2, offwhite));
    spheres.push_back(Sphere(Vec3f(-1.0, -1.5, -12), 2, red));
    spheres.push_back(Sphere(Vec3f(1.5, -0.5, -18), 3, red));
    spheres.push_back(Sphere(Vec3f(7, 5, -18), 4, offwhite));

    render(spheres);

    return 0;
}