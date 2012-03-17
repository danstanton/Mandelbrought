#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gmp.h>

using namespace std;

const int sqr_size = 2;

class Frimage {
	public:
		bool (Frimage::*bound_check)(mpf_t, mpf_t); 
		int iter, img_width, img_height, **frac_data;
		bool **have_depth; 
		mpf_t focus_x, focus_y, zoom;

		void write_data_to_file(char *in_file) {
			char *filename = new char[50];
			filename[0] = '\0';
			strcat(filename, in_file);
			strcat(filename, ".dat");
			FILE *data_file;
			data_file = fopen(filename, "w");
			fprintf(data_file, "%ux%u\n", img_width, img_height);
			for(int y=0; y < img_height; y++)
				for(int x=0; x < img_width; x++)
					fprintf(data_file, "%u\n", frac_data[y][x]);

			fclose(data_file);
		};

		bool out_of_diamond(mpf_t a_real, mpf_t a_imag) {
			bool too_big;
			mpf_t temp_real, temp_imag;
			mpf_init(temp_real);
			mpf_init(temp_imag);

			// ?? a_r + a_i > sqr_size ??
			mpf_abs(temp_real, a_real);
			mpf_abs(temp_imag, a_imag);
			mpf_add(temp_real, temp_real, temp_imag);
			if (mpf_cmp_ui(temp_real, sqr_size) > 0)
				too_big = true;
			else
				too_big = false;
			mpf_clear(temp_real);
			mpf_clear(temp_imag);
			return too_big;
		};

		bool out_of_circle(mpf_t a_real, mpf_t a_imag) {
			bool too_big;
			mpf_t temp_real, temp_imag;
			mpf_init(temp_real);
			mpf_init(temp_imag);

			// ?? sqrt( a_r^2 + a_i^2 ) > sqr_size ??
			mpf_mul(temp_real, a_real, a_real);
			mpf_mul(temp_imag, a_imag, a_imag);
			mpf_add(temp_real, temp_real, temp_imag);
			mpf_sqrt(temp_real, temp_real);
			if (mpf_cmp_ui(temp_real, sqr_size) > 0)
				too_big = true;
			else
				too_big = false;

			mpf_clear(temp_real);
			mpf_clear(temp_imag);
			return too_big;
		};

		Frimage(char *in_filename) {
			bound_check = &Frimage::out_of_circle;
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

		~Frimage() {
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

		void square_z_and_add_c(mpf_t z_real, mpf_t z_imag, mpf_t c_real, mpf_t c_imag) {
			mpf_t temp_real, temp_imag;
			mpf_init(temp_real);
			mpf_init(temp_imag);

			// square z
			// -------------
			// Mathematical justification
			// Let z = a+ bi
			// z^2 = (a+bi)*(a+bi)
			//     = a^2 + 2*abi - b^2
			//     = a^2 - b^2 + 2*abi
			//     = (a+b)*(a-b) + 2*abi
			//  real part : (a+b)*(a-b)
			//  imaginary part : 2*a*b
			//  ------------
			//  Hopefully, reducing the calculation of the real part to one
			//  multiplication increases the speed of the calculation, but I
			//  haven't benchmarked it.

			// First, temp_real gets the real part
			mpf_add(temp_real, z_real, z_imag);
			mpf_sub(temp_imag, z_real, z_imag);
			mpf_mul(temp_real, temp_real, temp_imag);

			// Now, z_imag gets the imaginary part
			mpf_mul(temp_imag, z_real, z_imag);
			mpf_mul_2exp(z_imag, temp_imag, 1);

			// Now, z_real gets the real part from temp_real
			mpf_set(z_real, temp_real);
			// done with squaring z

			// add c
			mpf_add(z_real, z_real, c_real);
			mpf_add(z_imag, z_imag, c_imag);

			mpf_clear(temp_real);
			mpf_clear(temp_imag);
		};

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

int main( int ac, char ** av) {
	if ( ac < 2 ) {
		printf("Usage: %s in_file\n", av[0]);
		return 1;
	}

	Frimage beauty(av[1]);
	if(!beauty.have_depth)
		return 1;


	// start the fractal
	for (int y_pos = 0; y_pos < beauty.img_height; y_pos++) {
		for (int x_pos = 0; x_pos < beauty.img_width; x_pos++) {
			beauty.get_depth(x_pos, y_pos);
		}
	}

	beauty.write_data_to_file(av[1]);
	return 0;
}
