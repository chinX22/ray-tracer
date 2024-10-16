#include "uber.h"

#include "material.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
using namespace std;

int main(){

	// World
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.8));
	auto material_mid = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<metal>(color(0.7, 0.3, 0.7), 0.4);
	auto material_right = make_shared<metal>(color(0.3, 0.7, 0.7), 0);

	world.add(make_shared<sphere>(point3(0,0,-2.2), .5, material_mid));
	world.add(make_shared<sphere>(point3(-1.3,0,-2), .5, material_left));
	world.add(make_shared<sphere>(point3(1.3,0,-2), .5, material_right));
	world.add(make_shared<sphere>(point3(0,-100.5,-1), 100, material_ground));
	
	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.width = 400;
	cam.samples_per_pixel = 50;
	cam.max_depth = 50;
	cam.focal_length = 1.5;

	cam.render(world);
}
