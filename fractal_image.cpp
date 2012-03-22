#include "fractal_image.h"

using namespace std;

int Fractal_image::get_depth(int x, int y) {
	if(!have_depth[y][x])
		calc_depth(x, y);
	return frac_data[y][x];
}

void Fractal_image::calc_depth(int x, int y) {
	mpf_t real_c, imaginary_c;
	init_c_from_specs(real_c, x, img_width, focus_x);
	init_c_from_specs(imaginary_c, y, img_height, focus_y);

	// "planet" means "wanderer"
	mpf_t real_planet, imaginary_planet;
	mpf_init_set(real_planet, real_c);
	mpf_init_set(imaginary_planet, imaginary_c);

	int i=1;
	for(; i < iter; i++) {
		if((this->*bound_check)(real_planet, imaginary_planet))
			break;
		square_z_and_add_c(real_planet, imaginary_planet, real_c, imaginary_c);
	}

	int depth = i % iter;
	frac_data[y][x] = depth;
	have_depth[y][x] = true;
	above_axis[y][x] = (depth != 0) && (mpf_cmp_si(imaginary_planet, 0) > 0);
	calced++;

	double pixels = img_width*img_height;

	if((calced % 500) == 0) {
		printf("Fill ratio: %f \r", double(calced+bled)/pixels);
		fflush(stdout);
	}

	mpf_clear(real_c);
	mpf_clear(imaginary_c);
	mpf_clear(real_planet);
	mpf_clear(imaginary_planet);
}

bool Fractal_image::in_image(int x, int y) {
	return  (x >= 0 && x < img_width && y >= 0 && y < img_height);
}

