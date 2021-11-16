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
        paraboloid(point3 cen, double valA, double valB, shared_ptr<material> m)
            : center(cen), value_A(valA), value_B(valB), mat_ptr(m){}; 

        virtual bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

    public:
        point3 center;
        double value_A;
        double value_B;
        shared_ptr<material> mat_ptr;
};

bool paraboloid::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {

    // (z - O_z) = (y - O_y)^2/r^2 + (x - O_x)^2/r^2
    // r^2 = ((y - O_y)^2 + (x - O_x)^2)/(z - O_z)
    // r^2 = ((P(t)[1] - O_y)^2 + (P(t)[0] - O_x)^2)/(P(t)[2] - O_z)
    // ((P(t)[1] - O_y)^2 + (P(t)[0] - O_x)^2)/(P(t)[2] - O_z) - r^2 = 0
    // (((directionY*t - originY)**2 + (directionX*t - originX)**2)/(directionZ*t - originZ)) - radius**2 = 0
    
    
    double originX = r.origin()[0];
    double originY = r.origin()[1];
    double originZ = r.origin()[2];

    double directionX = r.direction()[0];
    double directionY = r.direction()[1];
    double directionZ = r.direction()[2];

    // if (directionY - originY < 10) return false;
    
    auto a = (value_A*value_A*directionY*directionY) + (value_B*value_B*directionX*directionX);
    //std::cerr << "a: " << a << std::endl;
    auto half_b = value_A*value_A*directionY*originY + value_B*value_B*directionX*originX - directionZ*value_A*value_A*value_B*value_B/2;
    //std::cerr << "half_b: " << half_b << std::endl;
    auto c = originY*originY + originX*originX + originZ*value_A*value_A*value_B*value_B;
    //std::cerr << "c: " << c << std::endl;

    auto discriminant =  half_b*half_b - a * c;
    if (discriminant < 0)
        return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / value_A*value_B;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    // std::cerr << "true\n"  <<std::endl;
    return true;
}

#endif