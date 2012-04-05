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

#include "fractal_image.h"

using namespace std;

bool well_read(int scanner) {
	if(scanner == 0 || scanner == EOF) {
		printf("Read error from input file.\n");
		return false;
	} else {
		return true;
	}
};

void Fractal_image::load_data(char *in_data) {
	if(data_file) 
		fclose(data_file);
	data_file = fopen(in_data, "r+");
	int x, y, depth;
	while(fscanf(data_file, "%u %u %u", &x, &y, &depth) == 3) {
		// don't use set_depth. it would try to write to the data_file
		frac_data[y][x] = depth;
		have_depth[y][x] = true;
	}
}

Fractal_image::Fractal_image(char *in_filename) {
	bound_check = &Fractal_image::out_of_circle;
	frac_data = NULL;
	have_depth = NULL;
	have_fx = false;
	have_fy = false;
	have_z = false;
	calced = 0;
	bled = 0;
	char filename[50], input_buffer[80];
	FILE *in_file;


	in_file = fopen(in_filename, "r");
	if (in_file == NULL) {
		printf("Could not open file.\n");
		return;
	}

	filename[0] = '\0';
	strcat(filename, in_filename);
	strcat(filename, ".dat");

	data_file = fopen(filename, "w");
	if (data_file == NULL) {
		printf("Could not open data file.\n");
		return;
	}

	mpfr_set_default_prec(100);

	if(!well_read(fscanf(in_file, "Horizontal: %upx\n", &img_width))) return;
	if(!well_read(fscanf(in_file, "Vertical: %upx\n", &img_height))) return;

	if(!well_read(fscanf(in_file, "X Focus: %s\n", input_buffer))) return;
	mpfr_init_set_str(focus_x, input_buffer, 10, GMP_RNDN);
	have_fx = true;
	if(!well_read(fscanf(in_file, "Y Focus: %s\n", input_buffer))) return;
	mpfr_init_set_str(focus_y, input_buffer, 10, GMP_RNDN);
	have_fy = true;
	if(!well_read(fscanf(in_file, "Zoom: %s\n", input_buffer))) return;
	mpfr_init_set_str(zoom, input_buffer, 10, GMP_RNDN);
	have_z = true;

	if(!well_read(fscanf(in_file, "Depth: %u\n", &iter))) return;

	fclose(in_file);

	// echo the input
	printf("Image Size: %ux%u \n", img_width, img_height);
	printf("Zoom: %f \n", mpfr_get_d(zoom, GMP_RNDN));
	printf("iterations per pixel: %u \n", iter);


	mpfr_mul_2exp(zoom, zoom, 1, GMP_RNDN);

	frac_data = new int *[img_height];
	have_depth = new bool *[img_height];
	for(int i=0; i < img_height; i++) {
		frac_data[i] = new int[img_width];
		have_depth[i] = new bool[img_width];
		for(int j=0; j < img_width; j++)
			have_depth[i][j] = false;
	}
}

Fractal_image::~Fractal_image() {
	fclose(data_file);
	if(have_fx)
		mpfr_clear(focus_x);
	if(have_fy)
		mpfr_clear(focus_y);
	if(have_z)
		mpfr_clear(zoom);

	if(frac_data) {
		for(int y=0; y < img_height; y++)
			delete [] frac_data[y];
		delete [] frac_data;
	}

	if(have_depth) {
		for(int y=0; y < img_height; y++)
			delete [] have_depth[y];
		delete [] have_depth;
	}
}

void Fractal_image::write_data_to_file(char *in_file) {
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
}

