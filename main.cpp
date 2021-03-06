#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "paraboloid.h"
#include "ray.h"
#include "vec3.h"
#include "camera.h"
#include "material.h"
#include "cylinder.h"
#include "texture.h"

#include <iostream>
#include <fstream>  // para ler e gravar em arquivos.


// Função auxiliar para criar um fundo de imagem colorido
// pega a direcao do raio e calcula uma interpolacao entra banco e azul
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}



double hit_sphere(const point3& center, double radius, const ray& r) {
   vec3 oc = r.origin() - center;
   auto a = r.direction().length_squared();
   auto half_b = dot(oc, r.direction());
   auto c = oc.length_squared() - radius*radius;
   auto discriminant = half_b*half_b - a*c;

   if (discriminant < 0) {
       return -1.0;
   } else {
       return (-half_b - sqrt(discriminant) ) / a;
   }
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }

        shared_ptr<material> sphere_material;
        auto albedo = color::random() * color::random();
        sphere_material = make_shared<lambertian>(albedo);

        world.add(make_shared<paraboloid>(point3(1, 0, 0), 2.0, 2.0, 3.0, sphere_material));
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);
    return hittable_list(globe);
}

hittable_list earth_cylider() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<cylinder>(point3(0,0,0), 1.5, 4, earth_surface);
    return hittable_list(globe);
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>();

    auto metal_material = make_shared<metal>(color(0.8, 0.1, 0.3), 0.0);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, metal_material));

    return objects;
}

hittable_list marble_spheres() {
    // World Objects
    hittable_list objects;

    // Textures
    auto pertext = make_shared<noise_texture>();
    auto pertext2 = make_shared<noise_texture2>(0.5, color(0.9, 0.8, 0.9));
    auto pertext3 = make_shared<noise_texture2>(0.5, color(0.8, 0.9, 0.8));

    // Defined Materials
    // auto marble_material = make_shared<marble>(0.0);
    auto lambertian_material = make_shared<lambertian>(color(0.9, 0.9, 0.9));
    auto glass_material = make_shared<dielectric>(1.5);
    auto metal_material = make_shared<metal>(color(0.7, 0.8, 0.7), 0.0);

    // Ground
    objects.add(make_shared<sphere>(point3(0,-1000,0), 999, lambertian_material));

    // Objects

    // Marbles
    objects.add(make_shared<sphere>(point3(0, 0, 0), 1, make_shared<marble>(pertext2)));
    objects.add(make_shared<sphere>(point3(-4, 0, 2), 1, make_shared<marble>(pertext)));
    objects.add(make_shared<sphere>(point3(-2, 0, -2), 1, make_shared<marble>(pertext3)));

    // Glass
    objects.add(make_shared<sphere>(point3(-8, 0, -5), 2, glass_material));

    // Metal
    objects.add(make_shared<sphere>(point3(-70, 0, -8), 5, metal_material));

    return objects;
}

hittable_list paraboloid_plot() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 998, ground_material));

    auto metal_material = make_shared<metal>(color(0.8, 0.1, 0.3), 0.0);

    shared_ptr<material> diffuse_material;
    auto albedo = color::random() * color::random();
    diffuse_material = make_shared<lambertian>(albedo);
    
    world.add(make_shared<paraboloid>(point3(0, 0, 0), 0.5, 0.5, 4.0, diffuse_material));
    // world.add(make_shared<paraboloid>(point3(1, 1, 1), 3.0, 1.0, diffuse_material));

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(-7, 1, -1), 1.0, material1));

    world.add(make_shared<sphere>(point3(-1, 1, -5), 2.0, metal_material));

    world.add(make_shared<sphere>(point3(0, 0, 0), 1.0, material1));
}

int main() {

    // Image

    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World

    hittable_list world;

    world = marble_spheres();

    // Camera

    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render
    std::ofstream file;  // Cria um stream para arquivos
    file.open("image.ppm");  // Abre um arquivo para saída do stream

    file << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);

            }
            write_color(file, pixel_color, samples_per_pixel);
        }
    }

    file.close();  // Fecha o stream para arquivo
    std::cerr << "\nDone.\n";
}
