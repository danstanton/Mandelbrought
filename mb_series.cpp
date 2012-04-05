/* Copyright 2012 Daniel Jacob Stanton
This file is part of Mandelbrought

Mandelbrought is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

Mandelbrought is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with Mandelbrought (in COPYING). If not, see 
<http://www.gnu.org/licenses/>.    */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gmp.h>
#include <mpfr.h>

using namespace std;

const int sqr_size = 5;

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

	mpfr_set_default_prec(100);

	int temp_test;

	// things to get from the file:
	// img_width
	// img_height
	int img_width, img_height;
	temp_test = fscanf(in_file, "%ux%u", &img_width, &img_height) == EOF;
	if (temp_test == EOF) {
		printf("could not read resolution %d \n", temp_test);
		return 1;
	}

	// focus_x
	// focus_y
	mpfr_t focus_x, focus_y;
	mpfr_init(focus_x);
	mpfr_init(focus_y);
	mpfr_inp_str(focus_x, in_file, 10, GMP_RNDN);
	mpfr_inp_str(focus_y, in_file, 10, GMP_RNDN);

	// zoom
	mpfr_t zoom, cur_zoom;
	mpfr_init(zoom);
	mpfr_inp_str(zoom, in_file, 10, GMP_RNDN);
	mpfr_init_set_ui(cur_zoom, 5000, GMP_RNDN);

	// iter
	int iter;
	temp_test = fscanf(in_file, "%u", &iter);
	if (temp_test == EOF) {
		printf("could not read iteration spec %d \n", temp_test);
		return 1;
	}

	fclose(in_file);

	int tic = 1000;
	char tic_str[6];
	char filename[60];

	FILE *out_file;
	while( mpfr_cmp(cur_zoom, zoom ) < 0) {
		strcpy(filename, av[1]);
		strcat(filename, "-");
		sprintf(tic_str, "%u", tic++);
		strcat(filename, tic_str);

		out_file = fopen(filename, "w");
		fprintf(out_file, "%ux%u\n", img_width, img_height);

		mpfr_out_str(out_file, 10, 0, focus_x, GMP_RNDN);
		fprintf(out_file, "\n");
		mpfr_out_str(out_file, 10, 0, focus_y, GMP_RNDN);
		fprintf(out_file, "\n");
		mpfr_out_str(out_file, 10, 0, cur_zoom, GMP_RNDN);
		fprintf(out_file, "\n");

		fprintf(out_file, "%u\n", iter);

		fclose(out_file);

		mpfr_mul_ui(cur_zoom, cur_zoom, 3, GMP_RNDN);
	}

	mpfr_clear(focus_x);
	mpfr_clear(focus_y);
	mpfr_clear(zoom);
	mpfr_clear(cur_zoom);

	return 0;
}
