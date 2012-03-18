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

		void write_data_to_file(char *in_file);

		bool out_of_diamond(mpf_t a_real, mpf_t a_imag);
		bool out_of_circle(mpf_t a_real, mpf_t a_imag);

		Fractal_image(char *in_filename) {
			bound_check = &Fractal_image::out_of_circle;
			frac_data = NULL;
			have_depth = NULL;

			FILE *in_file;
			in_file = fopen(in_filename, "r");

			if (in_file == NULL) {
				printf("Could not open file.\n");
				return;
			}

			mpf_set_default_prec(100);

			int read_test;

			read_test = fscanf(in_file, "%ux%u", &img_width, &img_height);
			if (read_test == EOF) {
				printf("could not read resolution %d \n", read_test);
				return;
			}

			mpf_init(focus_x);
			mpf_init(focus_y);
			mpf_inp_str(focus_x, in_file, 10);
			mpf_inp_str(focus_y, in_file, 10);

			mpf_init(zoom);
			mpf_inp_str(zoom, in_file, 10);

			read_test = fscanf(in_file, "%u", &iter);
			if (read_test == EOF) {
				printf("could not read iteration spec %d \n", read_test);
				return;
			}

			fclose(in_file);

			// echo the input
			printf("Image Size: %ux%u \n", img_width, img_height);
			printf("Zoom: %f \n", mpf_get_d(zoom));
			printf("iterations per pixel: %u \n", iter);

			mpf_mul_2exp(zoom, zoom, 1);

			frac_data = new int *[img_height];
			have_depth = new bool *[img_height];
			for(int i=0; i < img_height; i++) {
				frac_data[i] = new int[img_width];
				have_depth[i] = new bool[img_width];
				for(int j=0; j < img_width; j++)
					have_depth[i][j] = false;
			}
		};

		~Fractal_image() {
			mpf_clear(focus_x);
			mpf_clear(focus_y);
			mpf_clear(zoom);
			for(int y=0; y < img_height; y++){
				delete [] frac_data[y];
				delete [] have_depth[y];
			}
			delete [] frac_data;
			delete [] have_depth;
		};

		void square_z_and_add_c(mpf_t z_real, mpf_t z_imag, mpf_t c_real, mpf_t c_imag);

		void init_c_from_specs(mpf_t c_val, int pos, int img_length, mpf_t img_center) {
			// This first step converts the position into a zero-centered value
			// with maximum = img_length and minimum = -img_length
			mpf_init_set_si(c_val, (2*pos - img_length));

			// Dividing by the zoom brings the value to the proper precision
			mpf_div(c_val, c_val, zoom);

			// Adding the img_center brings the value to the proper place
			mpf_add(c_val, c_val, img_center);
		};

		void calc_depth(int x, int y) {
			mpf_t real_g, imag_g;
			init_c_from_specs(real_g, x, img_width, focus_x);
			init_c_from_specs(imag_g, y, img_height, focus_y);

			mpf_t real_p, imag_p;
			mpf_init_set(real_p, real_g);
			mpf_init_set(imag_p, imag_g);

			if (x%1000 == 0)
				printf("Progress: Y:%u X:%u       \r", y, x);

			int i;
			for(i = 1; (i < iter) && ! (this->*bound_check)(real_p, imag_p); i++)
				square_z_and_add_c(real_p, imag_p, real_g, imag_g);

			frac_data[y][x] = i % iter;
			have_depth[y][x] = true;

			mpf_clear(real_g);
			mpf_clear(imag_g);
			mpf_clear(real_p);
			mpf_clear(imag_p);
		};

		int get_depth(int x, int y) {
			if(!have_depth[y][x])
				calc_depth(x, y);
			return frac_data[y][x];
		};
};

