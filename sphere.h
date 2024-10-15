#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

using namespace std;

class sphere : public hittable{
	public:
		sphere(const point3& center, double radius):center(center), radius(fmax(0, radius))_{}

		bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override{
			vec3 oc = center - r.origin();
			auto a = r.direction().length_squared();
			auto h = dot(r.direction(), oc);
			auto c = oc.length_squared() - radius*radius;

			auto discriminant = h*h - a*c;
			if (discriminant < 0) {
				return false;
			}

			auto sqrtd = sqrt(discriminant);

			// Find nearest root thats in the range;
			auto root = (h - sqrtd) / a;
			if (root <= ray_tmin || ray_tmax <= root){
				root = (h + sqrtd) / a;
				if (root <= ray_tmin || ray_tmax <= root){
					return false;
				}
			}

			rec.t = root;
			rec.p = r.at(rec.t);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			return true;
		}
	private:
		point3 center;
		double radius;
}
