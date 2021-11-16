#ifndef PARABOLOID_H
#define PARABOLOID_H

#include "hittable.h"
#include "vec3.h"

// Equação parametrizada:
// paraboloid(r*cos(t), r*sin(t), r^2)
// t variando de [0, 2*pi]
// r variando de 0 até qualquer valor maior

class paraboloid : public hittable {
    public :

        paraboloid(){}
        paraboloid(point3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m){}; 

        virtual bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

bool paraboloid::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {

    // (z - O_z) = (y - O_y)^2/r^2 + (x - O_x)^2/r^2
    // r^2 = ((y - O_y)^2 + (x - O_x)^2)/(z - O_z)
    // r^2 = ((P(t)[1] - O_y)^2 + (P(t)[0] - O_x)^2)/(P(t)[2] - O_z)
    // ((P(t)[1] - O_y)^2 + (P(t)[0] - O_x)^2)/(P(t)[2] - O_z) - r^2 = 0
    // (((directionY - originY)**2 + (directionX - originX)**2)/(directionZ - originZ)) - radius**2 = 0
    
    
    double originX = r.origin()[0];
    double originY = r.origin()[1];
    double originZ = r.origin()[2];

    double directionX = r.direction()[0];
    double directionY = r.direction()[1];
    double directionZ = r.direction()[2];
    
    auto a = (directionY*directionY + directionX*directionX)/(directionZ - originZ);
    //std::cerr << "a: " << a << std::endl;
    auto half_b = (directionY*originY + directionX*originX)/(directionZ - originZ); 
    //std::cerr << "half_b: " << half_b << std::endl;
    auto c = ((originY*originY + originX*originX)/(directionZ - originZ)) - radius*radius;
    //std::cerr << "c: " << c << std::endl;

    auto discriminant = half_b * half_b - 4 * a * c;
    if (discriminant < 0)
        return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / 2*a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif