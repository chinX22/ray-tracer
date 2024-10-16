#ifndef HITTABLE_H
#define HITTABLE_H

// An abstract class for hittable objects

class material;

// This record is our way of having a whole bunch of info
class hit_record{
	public:
		// For the closest point (if any) that gets hit by a ray: its point,
		// surface normal, and sorta distance from ray
		point3 p;
		vec3 normal;

		// When a ray hits a surface, this pointer points to the material pointer
		// the surface was given when it was made, ray_color() also gets to use 
		// its functions to find out if any rays get scattered
		shared_ptr<material> mat;
		double t;
		bool front_face;

		void set_face_normal(const ray& r, const vec3& outward_normal){
			// Sets the hit record normal vector
			// outward_normal vector assumed to have unit length
			
			front_face = dot(r.direction(), outward_normal) < 0;
			normal = front_face ? outward_normal : -outward_normal;
		}
};

class hittable{
	public:
		virtual ~hittable() = default;

		// Every "hittable" object (sphere, cube, ect), is reponsible for its
		// own hit detection
		virtual bool hit(const ray& r, interval ray_t,
			       	hit_record& rec) const = 0;
};

#endif
