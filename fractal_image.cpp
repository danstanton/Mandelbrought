#include "fractal_image.h"

using namespace std;

int Fractal_image::get_depth(int x, int y) {
	if(!have_depth[y][x])
		calc_depth(x, y);
	return frac_data[y][x];
}

void Fractal_image::calc_depth(int x, int y) {
	mpf_t real_g, imag_g;
	init_c_from_specs(real_g, x, img_width, focus_x);
	init_c_from_specs(imag_g, y, img_height, focus_y);

	mpf_t real_p, imag_p;
	mpf_init_set(real_p, real_g);
	mpf_init_set(imag_p, imag_g);

	int i;
	for(i = 1; (i < iter) && ! (this->*bound_check)(real_p, imag_p); i++)
		square_z_and_add_c(real_p, imag_p, real_g, imag_g);

	fill_count++;
	int depth = i % iter;
	frac_data[y][x] = depth;
	have_depth[y][x] = true;
	above_axis[y][x] = !depth || (mpf_cmp_si(imag_p, 0) > 0);

	if((fill_count % 500) == 0) {
		printf("Fill percent: %f \r", (fill_count*100.0)/(1.0*img_width*img_height));
		fflush(stdout);
	}

	mpf_clear(real_g);
	mpf_clear(imag_g);
	mpf_clear(real_p);
	mpf_clear(imag_p);
}

bool Fractal_image::in_image(int x, int y) {
	return  (x >= 0 && x < img_width && y >= 0 && y < img_height);
}

