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
        paraboloid(point3 cen, double valA, double valB, double height_limit, shared_ptr<material> m)
            : center(cen), value_A(valA), value_B(valB), hl(height_limit), mat_ptr(m){}; 

        virtual bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

    public:
        point3 center;
        double value_A;
        double value_B;
        double hl;
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

    
    auto a = (directionX*directionX*value_B*value_B + directionY*directionY*value_A*value_A)/(value_A*value_A*value_B*value_B);
    //std::cerr << "a: " << a << std::endl;
    auto b = (2*originX*directionX*value_B*value_B + 2*originY*directionY*value_A*value_A - 2*center[0]*directionX*value_B*value_B - 2*center[1]*directionY*value_A*value_A - directionZ*value_A*value_A*value_B*value_B)/(value_A*value_A*value_B*value_B);
    //std::cerr << "half_b: " << half_b << std::endl;
    auto c = (originX*originX*value_B*value_B + originY*originY*value_A*value_A + center[0]*center[0]*value_B*value_B + center[1]*center[1]*value_A*value_A - 2*originX*center[0]*value_B*value_B - 2*originY*center[1]*value_A*value_A + center[2]*value_A*value_A*value_B*value_B - originZ*value_A*value_A*value_B*value_B)/(value_A*value_A*value_B*value_B);
    //std::cerr << "c: " << c << std::endl;

    auto discriminant =  b*b - 4 * a * c;
    if (discriminant < 0)
        return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-b - sqrtd) / (2 * a);
    if ((r.at(root) - center)[2] > hl) return false;
    if (root < t_min || t_max < root) {
        root = (-b + sqrtd) / (2 * a);
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / value_A*value_A*value_B*value_B;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    return true;
}

#endif