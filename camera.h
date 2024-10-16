#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

using namespace std;

class camera{
	public:
		double aspect_ratio = 1.0;
		int width = 100;
		int samples_per_pixel = 10; // Count of random samples for each pixel
		int max_depth = 10;
		double focal_length = 1;

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
		vec3 delta_u;
		vec3 delta_v;

		void initialize(){
			height = int(width / aspect_ratio);
			height = (height < 1) ? 1 : height;

			pixel_samples_scale = 1.0 / samples_per_pixel;
			camera_center = point3(0, 0, 0);

			// Viewport settings
			auto viewport_height = 2.0;
			auto viewport_width = viewport_height * (double(width) / height);

			// Horizontal and verical viewport vectors
			auto viewport_u = vec3(viewport_width, 0, 0);
			auto viewport_v = vec3(0, -viewport_height, 0);

			// Horizontal and vertical vectors for the distance from pixel2pixel
			delta_u = viewport_u / width;
			delta_v = viewport_v / height;

			// Find upper left pixel
			auto viewport_upper_left = camera_center - vec3(0, 0, focal_length)
				- viewport_u/2 - viewport_v/2;
			pixel100_loc = viewport_upper_left + 0.5 * (delta_u + delta_v);
		}

		// Make a camera ray that points at a random point around the pixel i, j
		ray get_ray(int i, int j) const{
			auto offset = sample_square();
			auto pixel_sample = pixel100_loc + ((i + offset.y()) * delta_v)
				+ ((j + offset.x()) * delta_u);
			auto ray_origin = camera_center;
			auto ray_direction = pixel_sample - ray_origin;

			return ray(ray_origin, ray_direction);
		}

		// Get a random point within the -.5 and .5 unit square
		vec3 sample_square() const{
			return vec3(random_double() - 0.5, random_double() - 0.5, 0);
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
