#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

using namespace std;

class camera{
	public:
		double aspect_ratio = 1.0;
		int width = 100;
		int samples_per_pixel = 10; // Count of random samples for each pixel
		int max_depth = 10;			// Number of times a ray can bounce
		double vfov = 90;			//vertical view angle, quake time
		point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
		point3 lookat = point3(0, 0, -1);  // Point camera is lookingat
		vec3 vup = vec3(0, 1, 0);		   // Camera's "up" direction
		double defocus_angle = 0;	// Variation angle of rays through each pixel
		double focus_dist = 10;		// Distance from lookfrom to perfect focus plane
		vec3 defocus_disk_u, defocus_disk_v; //Defocus disk horizontal and vertical radii

		void render(const hittable& world){
			initialize();

			cout << "P3" << endl << width << " " <<
				height << endl << "255" << endl;

			if (samples_per_pixel == 0){
				for (int i = 0; i < height; i++){
					clog << "lines left: " << (height - i) << " " << endl<< flush;
					for (int j = 0; j < width; j++){
						auto pixel_center = pixel100_loc + (i * delta_v) + (j * delta_u);
						auto ray_direction = pixel_center - camera_center;
						ray r(camera_center, ray_direction);
						color pixel_color = ray_color(r, max_depth, world);
						write_color(cout, pixel_color);
					}
				}
				clog << "done" << endl;

			}else{

				for (int i = 0; i < height; i++){
					clog << "lines left: " << (height - i) << " " << endl<< flush;
					for (int j = 0; j < width; j++){
						color pixel_color(0, 0, 0);
						for (int sample = 0; sample < samples_per_pixel; sample++){
							ray r = get_ray(i, j);
							pixel_color += ray_color(r, max_depth, world);
						}
						write_color(cout, pixel_samples_scale * pixel_color);
					}
				}
				clog << "done" << endl;
			} 
		}
	private:
		int height;
		double pixel_samples_scale;
		point3 camera_center;
		point3 pixel100_loc;
		vec3 delta_u, delta_v;
		vec3 u, v, w;
		vec3 defocus_disc_u, defocus_disc_v;


		void initialize(){
			height = int(width / aspect_ratio);
			height = (height < 1) ? 1 : height;

			pixel_samples_scale = 1.0 / samples_per_pixel;
			camera_center = lookfrom;

			// Viewport settings
			auto theta = degrees_to_radians(vfov);
			auto h = tan(theta / 2);
			auto viewport_height = 2 * h * focus_dist;
			auto viewport_width = viewport_height * (double(width) / height);

			// Solve u, v, and q for camera orthogonial projection
			w = unit_vector(lookfrom - lookat); // points away from looking direction
			u = unit_vector(cross(vup, w)); // points right
			v = cross(w, u); // points up

			// Horizontal and verical viewport vectors
			auto viewport_u = viewport_width * u;
			auto viewport_v = viewport_height * -v;

			// Horizontal and vertical vectors for the distance from pixel2pixel
			delta_u = viewport_u / width;
			delta_v = viewport_v / height;

			// Find upper left pixel
			auto viewport_upper_left = camera_center - (focus_dist * w)
										- viewport_u/2 - viewport_v/2;
			pixel100_loc = viewport_upper_left + 0.5 * (delta_u + delta_v);

			// Find camera defocus disk vectors
			auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
			defocus_disk_u = u * defocus_radius;
			defocus_disk_v = v * defocus_radius;
		}

		ray get_ray(int i, int j) const{
			// Make a ray from the defocus disk to a random point on pixel i, j
			auto offset = sample_square();
			auto pixel_sample = pixel100_loc + ((i + offset.y()) * delta_v)
				+ ((j + offset.x()) * delta_u);
			auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
			auto ray_direction = pixel_sample - ray_origin;

			return ray(ray_origin, ray_direction);
		}

		// Get a random point within the -.5 and .5 unit square
		vec3 sample_square() const{
			return vec3(random_double() - 0.5, random_double() - 0.5, 0);
		}

		// Get randome point in camera defocus disk
		point3 defocus_disk_sample() const{
			auto p = random_in_unit_disk();
			return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
		}

		// Return the color from the ray
		color ray_color(const ray& r, int depth, const hittable& world){
			if (depth <= 0){
				return color(0, 0, 0);
			}

			hit_record rec;

			if(world.hit(r, interval(0.001, infinity), rec)){
				ray scattered;
				color attenuation;
				if (rec.mat->scatter(r, rec, attenuation, scattered)){
					return attenuation *  ray_color(scattered, depth -1, world);
				}
				return color(0, 0, 0);
			}

			vec3 unit_direction = unit_vector(r.direction());
			auto a = 0.5 * (unit_direction.y() + 1.0);

			// Blends between white and blue depending on a
			// (which is how much up or down the ray is looking)
			return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
		}
};

#endif
