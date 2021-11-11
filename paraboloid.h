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

    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
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
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif