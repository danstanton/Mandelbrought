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

int main(int ac, char ** av) {
	if ( ac < 4 || ac > 5) {
		printf("Usage: %s in_file x_coord y_coord\n", av[0]);
		printf("Or: %s in_file x_coord y_coord zoom_factor\n", av[0]);
		printf("Please supply only integers for all command-line arguments.\n");
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
	int img_width, img_height, iterates;
	temp_test = fscanf(in_file, "%ux%u", &img_width, &img_height);
	if (temp_test == EOF) {
		printf("Could not read resolution.\n");
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
	mpfr_t zoom;
	mpfr_init(zoom);
	mpfr_inp_str(zoom, in_file, 10, GMP_RNDN);

	temp_test = fscanf(in_file, "%u", &iterates);
	if (temp_test == EOF) {
		printf("Could not read iterations.\n");
		return 1;
	}

	fclose(in_file);

	// get the coordinates
	int x_coord = atoi(av[2]);
	int y_coord = atoi(av[3]);
	y_coord = img_height-y_coord;
	int z_factor = 1;
	bool change_zoom = (ac == 5);
	if (change_zoom)
		z_factor = atoi(av[4]);

	// the halves are important to me now
	img_width /= 2;
	img_height /= 2;

	mpfr_t img_wid, img_hei;
	mpfr_init(img_wid);
	mpfr_init(img_hei);

	// Translate image half-dimensions into a vector in the complex plane
	mpfr_ui_div(img_wid, img_width, zoom, GMP_RNDN);
	mpfr_ui_div(img_hei, img_height, zoom, GMP_RNDN);

	// Use the half-distances to get the upper-left corner
	mpfr_sub(focus_x, focus_x, img_wid, GMP_RNDN);
	mpfr_add(focus_y, focus_y, img_hei, GMP_RNDN);

	// Translate user-input into a vector in the complex plane
	mpfr_ui_div(img_wid, x_coord, zoom, GMP_RNDN);
	mpfr_ui_div(img_hei, y_coord, zoom, GMP_RNDN);

	// Use the vector to get the new center
	mpfr_add(focus_x, focus_x, img_wid, GMP_RNDN);
	mpfr_sub(focus_y, focus_y, img_hei, GMP_RNDN);

	// Find the new zoom factor
	if (change_zoom)
		mpfr_mul_ui(zoom, zoom, z_factor, GMP_RNDN);

	// Print out the new input 
	printf("%ux%u\n",2*img_width,2*img_height);
	mpfr_out_str(NULL, 10, (size_t)40, focus_x, GMP_RNDN);
	printf("\n");
	mpfr_out_str(NULL, 10, (size_t)40, focus_y, GMP_RNDN);
	printf("\n");
	mpfr_out_str(NULL, 10, (size_t)40, zoom, GMP_RNDN);
	printf("\n");
	printf("%u\n",iterates);

	mpfr_clear(img_wid);
	mpfr_clear(img_hei);
	mpfr_clear(focus_x);
	mpfr_clear(focus_y);
	mpfr_clear(zoom);
	return 0;
}
