#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gmp.h>

using namespace std;

int main( int ac, char ** av) {
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

	mpf_set_default_prec(100);

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
	mpf_t focus_x, focus_y;
	mpf_init(focus_x);
	mpf_init(focus_y);
	mpf_inp_str(focus_x, in_file, 10);
	mpf_inp_str(focus_y, in_file, 10);

	// zoom
	mpf_t zoom;
	mpf_init(zoom);
	mpf_inp_str(zoom, in_file, 10);

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

	mpf_t img_wid, img_hei;
	mpf_init(img_wid);
	mpf_init(img_hei);

	// Translate image half-dimensions into a vector in the complex plane
	mpf_ui_div(img_wid, img_width, zoom);
	mpf_ui_div(img_hei, img_height, zoom);

	// Use the half-distances to get the upper-left corner
	mpf_sub(focus_x, focus_x, img_wid);
	mpf_add(focus_y, focus_y, img_hei);

	// Translate user-input into a vector in the complex plane
	mpf_ui_div(img_wid, x_coord, zoom);
	mpf_ui_div(img_hei, y_coord, zoom);

	// Use the vector to get the new center
	mpf_add(focus_x, focus_x, img_wid);
	mpf_sub(focus_y, focus_y, img_hei);

	// Find the new zoom factor
	if (change_zoom)
		mpf_mul_ui(zoom, zoom, z_factor);

	// Print out the new input 
	printf("%ux%u\n",2*img_width,2*img_height);
	mpf_out_str(NULL, 10, (size_t)40, focus_x);
	printf("\n");
	mpf_out_str(NULL, 10, (size_t)40, focus_y);
	printf("\n");
	mpf_out_str(NULL, 10, (size_t)40, zoom);
	printf("\n");
	printf("%u\n",iterates);

	mpf_clear(img_wid);
	mpf_clear(img_hei);
	mpf_clear(focus_x);
	mpf_clear(focus_y);
	mpf_clear(zoom);
	return 0;
}
