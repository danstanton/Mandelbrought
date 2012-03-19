#include "fractal_image.h"

using namespace std;

Fractal_image::Fractal_image(char *in_filename) {
	bound_check = &Fractal_image::out_of_circle;
	frac_data = NULL;
	have_depth = NULL;
	above_axis = NULL;

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
	above_axis = new bool *[img_height];
	for(int i=0; i < img_height; i++) {
		frac_data[i] = new int[img_width];
		have_depth[i] = new bool[img_width];
		above_axis[i] = new bool[img_width];
		for(int j=0; j < img_width; j++) {
			have_depth[i][j] = false;
			above_axis[i][j] = false;
		}
	}
}

Fractal_image::~Fractal_image() {
	mpf_clear(focus_x);
	mpf_clear(focus_y);
	mpf_clear(zoom);
	for(int y=0; y < img_height; y++){
		delete [] frac_data[y];
		delete [] have_depth[y];
		delete [] above_axis[y];
	}
	delete [] frac_data;
	delete [] have_depth;
	delete [] above_axis;
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

