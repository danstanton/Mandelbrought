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

bool in_two(double target) {
	return target <=2.0 && target >= -2.0;
};

Fractal_image::Fractal_image(char *in_filename) {
	bound_check = &Fractal_image::out_of_circle;
	frac_data = NULL;
	have_depth = NULL;
	calced = 0;
	bled = 0;

	FILE *in_file;
	in_file = fopen(in_filename, "r");

	char input_buffer[80];

	if (in_file == NULL) {
		printf("Could not open file.\n");
		return;
	}

	mpf_t eedge, dedge;
	double edge[4];
	bool cut = false;

	mpf_set_default_prec(100);

	if(!well_read(fscanf(in_file, "Horizontal: %upx\n", &img_width))) return;
	if(!well_read(fscanf(in_file, "Vertical: %upx\n", &img_height))) return;

	if(!well_read(fscanf(in_file, "X Focus: %s\n", input_buffer))) return;
	mpf_init_set_str(focus_x, input_buffer, 10);
	if(!well_read(fscanf(in_file, "Y Focus: %s\n", input_buffer))) return;
	mpf_init_set_str(focus_y, input_buffer, 10);
	if(!well_read(fscanf(in_file, "Zoom: %s\n", input_buffer))) return;
	mpf_init_set_str(zoom, input_buffer, 10);

	if(!well_read(fscanf(in_file, "Depth: %u\n", &iter))) return;

	fclose(in_file);

	// echo the input
	printf("Image Size: %ux%u \n", img_width, img_height);
	printf("Zoom: %f \n", mpf_get_d(zoom));
	printf("iterations per pixel: %u \n", iter);

	// Detect any errors
	mpf_init(eedge);
	mpf_init(dedge);
	mpf_ui_div(dedge, img_width, zoom);
	mpf_div_ui(eedge, dedge, 2);
	mpf_add(eedge, eedge, focus_x);
	edge[0] = mpf_get_d(eedge);
	mpf_sub(eedge, eedge, dedge);
	edge[2] = mpf_get_d(eedge);
	mpf_ui_div(dedge, img_height, zoom);
	mpf_div_ui(eedge, dedge, 2);
	mpf_add(eedge, eedge, focus_y);
	edge[1] = mpf_get_d(eedge);
	mpf_sub(eedge, eedge, dedge);
	edge[3] = mpf_get_d(eedge);

	cut = false;
	for(int i=0; i<4; i++)
		cut = cut || in_two(edge[i]);
	if(!cut) {
		printf("View of Mandelbrot set too wide. Please zoom in and try again.\n");
		return;
	}

	mpf_mul_2exp(zoom, zoom, 1);

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
	mpf_clear(focus_x);
	mpf_clear(focus_y);
	mpf_clear(zoom);
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

