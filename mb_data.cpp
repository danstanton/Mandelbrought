#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gmp.h>

using namespace std;

const int sqr_size = 2;

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

bool (*bound_check)(mpf_t, mpf_t) = &out_of_circle;

void mpf_complex_sqrt(mpf_t rout, mpf_t iout, mpf_t rin, mpf_t iin) {
	// sqrt of a complex number
	// p = sqrt((sqrt(a^2+b^2)+a)/2)
	// temp_p = sqrt(a^2 + b^2)
	mpf_mul(rout, rin, rin);
	mpf_mul(iout, iin, iin);
	mpf_add(rout, rout, iout);
	mpf_sqrt(rout, rout);
	// temp_q = sqrt(a^2+b^2)
	mpf_set(iout, rout);

	// sqrt((sqrt(a^2+b^2)+a)/2)
	mpf_add(rout, rout, rin);
	mpf_div_ui(rout, rout, 2);
	if(mpf_cmp_ui(rout, 0) < 0) //shouldn't happen, but it does
		mpf_set_ui(rout, 0);
	mpf_sqrt(rout, rout);

	// q = sqrt(sign(b)*(sqrt(a^2+b^2)-a)/2)
	mpf_sub(iout, iout, rin);
	mpf_div_ui(iout, iout, 2);
	if(mpf_cmp_ui(iout, 0) < 0) //shouldn't happen, but it does
		mpf_set_ui(iout, 0);
	mpf_sqrt(iout, iout);
	if(mpf_cmp_ui(iin, 0) < 0)
		mpf_neg(iout, iout);
};

int main( int ac, char ** av) {
	if ( ac < 2 ) {
		printf("Usage: %s in_file\n", av[0]);
		return 1;
	}

	FILE *in_file;
	in_file = fopen(av[1], "r");

	if (in_file == NULL) {
		printf("Could not open file.\n");
		return 1;
	}

	mpf_set_default_prec(100);

	int temp_test;

	// things to get from the file:
	// img_width
	// img_height
	int *img_width = new int(), *img_height = new int();
	temp_test = fscanf(in_file, "%ux%u", img_width, img_height) == EOF;
	if (temp_test == EOF) {
		printf("could not read resolution %d \n", temp_test);
		return 1;
	}

	int **frac_map = new int *[*img_height];
	for(int i=0; i < *img_height; i++) 
		frac_map[i] = new int[*img_width];

	// focus_x
	// focus_y
	mpf_t focus_x, focus_y;
	mpf_init(focus_x);
	mpf_init(focus_y);
	mpf_inp_str(focus_x, in_file, 10);
	mpf_inp_str(focus_y, in_file, 10);

	// zoom
	mpf_t zoom;
	mpf_init(zoom);
	mpf_inp_str(zoom, in_file, 10);

	// iter
	int *iter = new int();
	temp_test = fscanf(in_file, "%u", iter);
	if (temp_test == EOF) {
		printf("could not read iteration spec %d \n", temp_test);
		return 1;
	}

	fclose(in_file);

	// echo the input
	printf("Image Size: %ux%u \n", *img_width, *img_height);
	// printf("zoom: %d \n", mpf_get_d(zoom));
	printf("iterations per pixel: %u \n", *iter);

	mpf_mul_2exp(zoom, zoom, 1);

	// start the fractal
	for (int y_pos = 0; y_pos < *img_height; y_pos++) {
		for (int x_pos = 0; x_pos < *img_width; x_pos++) {
			mpf_t real_g;
			mpf_init_set_si(real_g, (2*x_pos - *img_width));
			mpf_div(real_g, real_g, zoom);
			mpf_add(real_g, real_g, focus_x);

			mpf_t imag_g;
			mpf_init_set_si(imag_g, (2*y_pos - *img_height));
			mpf_div(imag_g, imag_g, zoom);
			mpf_add(imag_g, imag_g, focus_y);

			mpf_t real_p, imag_p, real_t, imag_t;
			mpf_init(real_t);
			mpf_init(imag_t);
			mpf_init(real_p);
			mpf_init(imag_p);

			if (x_pos%1000 == 0)
				printf("Progress: Y:%u X:%u       \r", y_pos, x_pos);
			int i = 0;
			bool too_big = false; 

			/* This is only useful for large views of the set,
			 * and those are better handled using doubles and so forth
			// find miu and see if it's in the open unit disk
			// miu = 1 - 2 * sqrt(1/4 - c)
			mpf_set_d(real_t, 0.25);
			mpf_sub(real_t, real_t, real_g);
			mpf_neg(imag_t, imag_g);

			mpf_complex_sqrt(real_t1, imag_t1, real_t, imag_t);
			mpf_mul_2exp(real_t1, real_t1, 1);
			mpf_mul_2exp(imag_t1, imag_t1, 1);
			mpf_sub_ui(real_t1, real_t1, 1);
			mpf_neg(real_t1, real_t1);
			mpf_neg(imag_t1, imag_t1);
			// Now, miu is in real_t1, imag_t1

			mpf_mul(real_t1, real_t1, real_t1);
			mpf_mul(imag_t1, imag_t1, imag_t1);
			mpf_add(real_t1, real_t1, imag_t1);
			mpf_sqrt(real_t1, real_t1);

			if (mpf_cmp_d(real_t1, 1.0) < 0) {// it's in the cardioid
				printf("time saved!\n");
				goto calc_done;
			}
			*/


			mpf_set_ui(real_p, 0);
			mpf_set_ui(imag_p, 0);

test_place:

			too_big = bound_check(real_p, imag_p);

			i++;

			if(too_big || i > *iter)
				goto calc_done;

			// square p
			// real_t = real_p*real_p - imag_p * imag_p
			// real_t = (real_p+imag_p)*(real_p-imag_p)
			mpf_add(real_t, real_p, imag_p);
			mpf_sub(imag_t, real_p, imag_p);
			mpf_mul(real_t, real_t, imag_t);
			// imag_p = 2*real_p*imag_p
			mpf_mul(imag_t, real_p, imag_p);
			mpf_mul_2exp(imag_p, imag_t, 1);
			// real_p = real_t
			mpf_set(real_p, real_t);
			// done with squaring p

			// add c
			mpf_add(real_p, real_p, real_g);
			mpf_add(imag_p, imag_p, imag_g);
			goto test_place;
calc_done:

			frac_map[y_pos][x_pos] = 0;
			if(too_big)
				frac_map[y_pos][x_pos] = i;
			/*
			else if (i > *iter) {
				printf("\nReal: ");
				mpf_out_str(NULL, 10, (size_t)40, real_p);
				printf("\nImag: ");
				mpf_out_str(NULL, 10, (size_t)40, imag_p);
			}
			*/

			mpf_clear(real_g);
			mpf_clear(imag_g);
			mpf_clear(real_t);
			mpf_clear(imag_t);
			mpf_clear(real_p);
			mpf_clear(imag_p);
		}
	}

	char *filename = new char[50];
	filename[0] = '\0';
	strcat(filename, av[1]);
	strcat(filename, ".dat");
	FILE *data_file;
	data_file = fopen(filename, "w");
	fprintf(data_file, "%ux%u\n", *img_width, *img_height);

	for(int y=0; y < *img_height; y++)
		for(int x=0; x < *img_width; x++)
			fprintf(data_file, "%u\n", frac_map[y][x]);

	delete [] data_file;

	mpf_clear(focus_x);
	mpf_clear(focus_y);
	mpf_clear(zoom);

	for(int y=0; y < *img_height; y++)
		delete [] frac_map[y];

	delete [] frac_map;
	delete iter;
	delete img_width;
	delete img_height;
	return 0;
}
