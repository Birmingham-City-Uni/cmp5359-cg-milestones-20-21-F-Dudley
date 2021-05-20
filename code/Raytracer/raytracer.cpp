#pragma once

#include <fstream>
#include <chrono>
#include <SDL.h>

#include "ThreadPool.h"
#include "geometry.h"
#include "Ray.h"
#include "tgaimage.h"
#include "Utils.h"

#include "BVH.h"
#include "aabb.h"

#include "Camera.h"
#include "model.h"
#include "Collideable.h"
#include "Collideable_list.h"
#include "Material.h"
#include "Sphere.h"
#include "Triangle.h"

#define M_PI 3.14159265359

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* screenSurface;

void init() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Software Ray Tracer",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        0
    );

    screenSurface = SDL_GetWindowSurface(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);


}

void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16*)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32*)p = pixel;
        break;
    }
}

// method to ensure colours don’t go out of 8 bit range in RGB​
void clamp255(Vec3f& col) {
    col.x = (col.x > 255) ? 255 : (col.x < 0) ? 0 : col.x;
    col.y = (col.y > 255) ? 255 : (col.y < 0) ? 0 : col.y;
    col.z = (col.z > 255) ? 255 : (col.z < 0) ? 0 : col.z;
}

// Functions - Possible Encapsulation.
//

double hit_sphere(const Point3f& _centre, double _radius, const Ray& _ray) {
    Vec3f oc = _ray.GetOrigin() - _centre;

    auto a = _ray.GetDirection().dotProduct(_ray.GetDirection());
    auto b = 2.0f * oc.dotProduct(_ray.GetDirection());
    auto c = oc.dotProduct(oc) - _radius * _radius;

    auto discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1.0;
    }
    else {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

Colour ray_colour(const Ray& _ray, const Colour& _background, const Collideable& _world, int _depth) {
    Hit_Record record;

    if (_depth <= 0) return Colour(0, 0, 0);

    /*
    if (_world.Hit(_ray, 0.001, Utils::infinity, record)) {
        Ray scattered;
        Colour attenuation;
        if (record.material_ptr->Scatter(_ray, record, attenuation, scattered)) {
            return attenuation * ray_colour(scattered, _background, _world, _depth - 1);
        }

        return Colour(0, 0, 0);
    }

    Vec3f unit_direction = _ray.GetDirection().normalize();
    auto t = 0.5 * (unit_direction.y + 1.0);

    return (1.0 - t) * Colour(1.0, 1.0, 1.0) + t * Colour(0.5, 0.7, 1.0) * 255;
    */

    if (!_world.Hit(_ray, 0.001, Utils::infinity, record)) return _background;

    Ray scattered;
    Colour attenuation;
    Colour emitted = record.material_ptr->emitted();

    if (!record.material_ptr->Scatter(_ray, record, attenuation, scattered)) return emitted;


    return emitted + attenuation * ray_colour(scattered, _background, _world, _depth - 1);
}

void ThreadedRender(SDL_Surface* _screen, Collideable_List _world, int _y, int _spp, int _maxDepth, Camera* _camera) {
    const float aspectRatio = 16.0 / 9;
    const int imageWidth = _screen->w;
    const int imageHeight = static_cast<int> (imageWidth / aspectRatio);
    
    const Colour black(0);
    Colour pix_col(black);
    Colour background(0);

    for (int x = 0; x < _screen->w; ++x) {
        pix_col = black;

        for (int s = 0; s < _spp; s++) {
            auto u = double(x + Utils::random_double()) / (imageWidth - 1);
            auto v = double(_y + Utils::random_double()) / (imageHeight - 1);

            Ray ray = _camera->Get_Ray(u, v);
            Vec3f unit_direction = ray.GetDirection().normalize();

            auto t = 0.5 * (unit_direction.y + 1.0);
            background = (1.0 - t) * Colour(1) + t * Colour(0.5, 0.7, 1.0) * 255;

            // Accumulate Colours over Samples.
            pix_col = pix_col + ray_colour(ray, background, _world, _maxDepth);
        }

        pix_col /= 255.f * _spp;
        pix_col.x = sqrt(pix_col.x);
        pix_col.y = sqrt(pix_col.y);
        pix_col.z = sqrt(pix_col.z);
        pix_col *= 255;

        // Scale Colour Values According to Provided Sample Per Pixel (spp).
        Uint32 colour = SDL_MapRGB(screenSurface->format, pix_col.x, pix_col.y, pix_col.z);

        putpixel(screenSurface, x, _y, colour);
    }
}

void addModelToScene(Collideable_List& _world, Model* _model, Vec3f _modelTransform, std::shared_ptr<Material> _modelMat) {

    for (uint32_t i = 0; i < _model->nfaces(); ++i)
    {
        const Vec3f& vertex0 = _model->vert(_model->face(i)[0]);
        const Vec3f& vertex1 = _model->vert(_model->face(i)[1]);
        const Vec3f& vertex2 = _model->vert(_model->face(i)[2]);

        const Vec3f& normals = Vec3f(_model->vnorms(i)[0], _model->vnorms(i)[1], _model->vnorms(i)[2]);

        _world.Add(std::make_shared<Triangle>(vertex0 + _modelTransform, vertex1 + _modelTransform, vertex2 + _modelTransform, normals, _modelMat));
        std::cout << "Added Triangle: " << i << ". to Scene" << std::endl;
    }

    size_t test = _model->vnorms(1).size();

    std::cout << _model->nfaces() << " ----- " << _model->nnorms() << std::endl;
}

Collideable_List random_scene() {
    Collideable_List world_;

    auto ground_material = std::make_shared<Lambertian>(Colour(0.5));
    world_.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++) 
        {
            auto choose_mat = Utils::random_double();
            Point3f centre(a + 0.9 * Utils::random_double(), 0.2, b + 0.9 * Utils::random_double());

            if ((centre - Point3f(4, 0.2, 0)).length() > 0.9) 
            {
                std::shared_ptr<Material> sphere_material;
                Colour albedo;

                if (choose_mat < 0.8) // Diffuse Material
                {
                    albedo = Colour::random() * Colour::random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                }
                else if (choose_mat < 0.95) // Metal Material
                {
                    albedo = Colour::random(0.5, 1);
                    double fuzz = Utils::random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                }
                else  // Glass Material
                {
                    sphere_material = std::make_shared<Diffuse_Light>(Colour(255, 255, 255));
                }

                world_.Add(std::make_shared<Sphere>(centre, 0.2, sphere_material));
            }
        }
    }

    // Big Glass Sphere
    auto mat1 = std::make_shared<Dielectric>(1.5);
    world_.Add(std::make_shared<Sphere>(Point3f(0, 1, 0), 1.0, mat1));

    // Big Lambert Sphere
    auto mat2 = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
    world_.Add(std::make_shared<Sphere>(Point3f(-4, 1, 0), 1.0, mat2));

    // Big Metal Sphere
    auto mat3 = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
    world_.Add(std::make_shared<Sphere>(Point3f(4, 1, 0), 1, mat3));

    Model* testModel = new Model("../Models/bathroom_ShowerScreen.obj");
    addModelToScene(world_, testModel, Vec3f(0), mat3);

    //return world_;
    return Collideable_List(std::make_shared<BVH_Node>(world_));
}

Collideable_List bathroom_scene() {
    Collideable_List world_;
    int modelNum = 0;

    // The Model Directory
    std::string directory = "../Maya Project/Model Exports/";

    // Materials
    auto glassMat = std::make_shared<Dielectric>(2.2);
    auto plasticMat_white = std::make_shared<Lambertian>(Colour(0.7, 0.7, 0.7));
    auto carpetMat = std::make_shared<Lambertian>(Colour(0.0, 0.5, 0.0));
    auto metalMat = std::make_shared<Metal>(Colour(0.5, 0.5, 0.5), 0.2);
    auto mirrorMat = std::make_shared<Metal>(Colour(0.1, 0.1, 0.1), 0.1);
    auto lightMat = std::make_shared<Diffuse_Light>(Colour(255, 255, 255));

    // Models and Vectors (if Parent Object)
    Vec3f baseModelPos = Vec3f(0);

    
    // Counter Area
    Model* counter = new Model("../Models/bathroom_Counter.obj");
    addModelToScene(world_, counter, baseModelPos, carpetMat); modelNum++;
        
        
        Model* counter_light = new Model("../Models/bathroom_Counter_Light.obj");
        addModelToScene(world_, counter_light, baseModelPos, lightMat); modelNum++;

       
        //Model* counter_lightMount = new Model("../Models/bathroom_Counter_Light_Mount.obj");
        //addModelToScene(world_, counter_lightMount, baseModelPos, plasticMat_white); modelNum++;

        
        Model* counter_mirrorCover = new Model("../Models/bathroom_Counter_Mirror_Cover.obj");
        addModelToScene(world_, counter_mirrorCover, baseModelPos, plasticMat_white); modelNum++;

        Model* counter_mirrorSurface = new Model("../Models/bathroom_Counter_Mirror_Surface.obj");
        addModelToScene(world_, counter_mirrorSurface, baseModelPos, mirrorMat); modelNum++;

        //Model* counter_panel = new Model("../Models/bathroom_Counter_Panel.obj");
        //addModelToScene(world_, counter_panel, baseModelPos, testGreen); modelNum++;

        Model* counter_sink = new Model("../Models/bathroom_Counter_Sink.obj");
        addModelToScene(world_, counter_sink, baseModelPos, metalMat); modelNum++;

        Model* counter_tap = new Model("../Models/bathroom_Counter_Tap.obj");
        addModelToScene(world_, counter_tap, baseModelPos, metalMat); modelNum++;

        Model* counter_shelve = new Model("../Models/bathroom_Counter_Shelve.obj");
        addModelToScene(world_, counter_shelve, baseModelPos, carpetMat);
        

    Model* counterPole_right_outter = new Model("../Models/bathroom_Counter_Pole_Outer.obj");
    Model* counterPole_right_inner = new Model("../Models/bathroom_Counter_Pole_Inner.obj");
    addModelToScene(world_, counterPole_right_outter, baseModelPos, plasticMat_white); modelNum++;
    addModelToScene(world_, counterPole_right_inner, baseModelPos, metalMat); modelNum++;

    // ----- //

    // Shower Area

    Model* showerScreen = new Model("../Models/bathroom_ShowerScreen.obj");
    addModelToScene(world_, showerScreen, baseModelPos, mirrorMat); modelNum++;

    Model* shower = new Model("../Models/bathroom_Shower.obj");
    addModelToScene(world_, shower, baseModelPos, metalMat); modelNum++;

    // ------ //
    
    Model* carpet = new Model("../Models/bathroom_Carpet.obj");
    addModelToScene(world_, carpet, baseModelPos, carpetMat); modelNum++;
    
    //Model* walls = new Model("../Models/bathroom_Walls.obj");
    //addModelToScene(world_, walls, baseModelPos, plasticMat_white); modelNum++;

    std::cout << "Models All Loaded.\nTOTAL MODELS: " << modelNum << std::endl;

    world_.Add(std::make_shared<Sphere>(Vec3f(0, 1, 0), 0.5, lightMat));

    //return world_;
    return Collideable_List(std::make_shared<BVH_Node>(world_));
}

Collideable_List carloTest_scene() {
    Collideable_List world_;

    auto plasticMat_white = std::make_shared<Lambertian>(Colour(0.7, 0.7, 0.7));
    auto carpetMat = std::make_shared<Lambertian>(Colour(0.0, 0.5, 0.0));
    auto metalMat = std::make_shared<Metal>(Colour(0.5, 0.5, 0.5), 0.2);

    Vec3f baseModelPos = Vec3f(0);

    Model* showerScreen = new Model("../Models/bathroom_ShowerScreen.obj");
    addModelToScene(world_, showerScreen, baseModelPos, carpetMat);

    Model* shower = new Model("../Models/bathroom_Shower.obj");
    addModelToScene(world_, shower, baseModelPos, metalMat);

    return world_;
}

int main(int argc, char **argv)
{
    // initialise SDL2
    init();

    // -- //

    // Image Variables
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = screenSurface->w;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int spp = 10;
    const int max_depth = 50;

    const float scale = 1.f / spp;

    // Camera Variables
    Point3f cam_position(4.725, 6.976, 10.187);
    Point3f cam_aimPosition(1.703, 6.154, 4.154);
    Vec3f cam_vup(0, 1, 0);
    double cam_distanceToFocus = 10.0;
    double aperture = 0.1;
    double cam_fov = 80;

    Point3f tmpCamPos(-5, 2, 0);
    Point3f tmpCamAim(0);

    Camera camera(cam_position, cam_aimPosition, cam_vup, cam_fov, aspect_ratio, aperture, cam_distanceToFocus);

    // World Variables
    Collideable_List world = carloTest_scene();

    // -- //

    const Colour white(255, 255, 255);
    const Colour black(0, 0, 0);
    const Colour red(255, 0, 0);

    double t;
    Colour pix_col(black);

    SDL_Event e;
    bool running = true;
    bool firstRun = true;
    while (running) {

        auto t_start = std::chrono::high_resolution_clock::now();

        TGAImage image(screenSurface->w, screenSurface->h, TGAImage::RGB);

        // clear back buffer, pixel data on surface and depth buffer (as movement)
        SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0, 0, 0));
        SDL_RenderClear(renderer);

        
        for (int y = screenSurface->h-1; y >= 0; --y) {
            std::cerr << "\rScanlines Remaining: " << y << std::flush;
            Colour background(0);

            for (int x = 0; x < screenSurface->w; ++x) {
                pix_col = black;


                for (int s = 0; s < spp; s++) {
                    auto u = double(x + Utils::random_double()) / (image_width - 1);
                    auto v = double(y + Utils::random_double()) / (image_height - 1);

                    Ray ray = camera.Get_Ray(u, v);
                    Vec3f unit_direction = ray.GetDirection().normalize();

                    auto t = 0.5 * (unit_direction.y + 1.0);
                    background = (1.0 - t) * Colour(1) + t * Colour(0.5, 0.7, 1.0) * 255;

                    // Accumulate Colours over Samples.
                    pix_col = pix_col + ray_colour(ray, background, world, max_depth);
                }

                pix_col /= 255.f * spp;
                pix_col.x = sqrt(pix_col.x);
                pix_col.y = sqrt(pix_col.y);
                pix_col.z = sqrt(pix_col.z);
                pix_col *= 255;

                // Drawing to TGA Image.
                image.set(x, y, TGAColor((unsigned char) pix_col.x, (unsigned char) pix_col.y, (unsigned char) pix_col.z, 255));

                // Scale Colour Values According to Provided Sample Per Pixel (spp).
                Uint32 colour = SDL_MapRGB(screenSurface->format, pix_col.x, pix_col.y, pix_col.z);
                putpixel(screenSurface, x, y, colour);
            }
        }
        
        
        /*
        {
            t_start = std::chrono::high_resolution_clock::now();

            unsigned int threadNum = std::thread::hardware_concurrency();
            std::cout << "Number of Supported Threads: " << threadNum << std::endl;

            ThreadPool pool(1000);

            int start = screenSurface->h - 1;
            int step = screenSurface->h / std::thread::hardware_concurrency();
            for (int y = 0; y < screenSurface->h - 1; y++)
            {
                pool.Enqueue(std::bind(ThreadedRender, screenSurface, world, y, spp, max_depth, &camera));
            }

        }
        */

        auto t_end = std::chrono::high_resolution_clock::now();
        auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
        std::cerr << "\nFrame render time:  " << passedTime << " ms" << std::endl;

        if (firstRun) {
            // Output Drawn TGAImage.
            image.flip_horizontally();
            image.write_tga_file("raytracerOutput.tga");
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, screenSurface);
        if (texture == NULL) {
            fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
            exit(1);
        }
        SDL_FreeSurface(screenSurface);

        SDL_RenderCopyEx(renderer, texture, nullptr, nullptr, 0, 0, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(texture);

        if (SDL_PollEvent(&e))
        {
            switch (e.type) {
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
                break;
            }
        }
    }

    return 0;
}
