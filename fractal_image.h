#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <gmp.h>

const int bound_size = 2;

class Fractal_image {
	public:
		bool (Fractal_image::*bound_check)(mpf_t, mpf_t); 
		int iter, img_width, img_height, **frac_data;
		bool **have_depth; 
		mpf_t focus_x, focus_y, zoom;

		void write_data_to_file(char *);

		bool out_of_diamond(mpf_t, mpf_t);
		bool out_of_circle(mpf_t, mpf_t);

		Fractal_image(char *);
		~Fractal_image();

		void square_z_and_add_c(mpf_t, mpf_t, mpf_t, mpf_t);
		void init_c_from_specs(mpf_t, int, int, mpf_t);

		void calc_depth(int, int);
		int get_depth(int, int);
};

