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

int Fractal_image::get_depth(int x, int y) {
	if(!have_depth[y][x])
		calc_depth(x, y);
	return frac_data[y][x];
}

void Fractal_image::calc_depth(int x, int y) {
	mpf_t real_g, imag_g;
	init_c_from_specs(real_g, x, img_width, focus_x);
	init_c_from_specs(imag_g, y, img_height, focus_y);

	mpf_t real_p, imag_p;
	mpf_init_set(real_p, real_g);
	mpf_init_set(imag_p, imag_g);

	int i;
	for(i = 1; (i < iter) && ! (this->*bound_check)(real_p, imag_p); i++)
		square_z_and_add_c(real_p, imag_p, real_g, imag_g);

	frac_data[y][x] = i % iter;
	have_depth[y][x] = true;
	above_axis[y][x] = (mpf_cmp_si(imag_p, 0) > 0);

	mpf_clear(real_g);
	mpf_clear(imag_g);
	mpf_clear(real_p);
	mpf_clear(imag_p);
}

bool Fractal_image::in_image(int x, int y) {
	return  (x >= 0 && x < img_width && y >= 0 && y < img_height);
}

void Fractal_image::turn_left(int &x, int &y) {
	int temp=y;
	y=x;
	x=-temp;
}

void Fractal_image::turn_around(int &x, int &y) {
	y=-y;
	x=-x;
}

void Fractal_image::turn_right(int &x, int &y) {
	turn_left(y, x); // How's that for code re-use? :)
}

void Fractal_image::step(int &x, int &y, int x_dir, int y_dir) {
	x+=x_dir;
	y+=y_dir;
}

void Fractal_image::fill_area(int x, int y) {
	if(have_depth[y][x])
		return;

	calc_depth(x, y);
	int test_depth = frac_data[y][x];
	bool test_switch = above_axis[y][x];

	if(test_depth == 0)
		return;

	bool same_this_way[4];
	int same_count=0;
	// Check the adjacent depths and see if anything matches
	int x_dir = 1, y_dir = 0;
	for(int dir=0; dir<4; dir++) {
		int x_alt = x, y_alt = y;
		step(x_alt, y_alt, x_dir, y_dir);
		if(in_image(x_alt, y_alt)) {
			if(test_depth == get_depth(x_alt, y_alt)
					&& test_switch == above_axis[y_alt][x_alt]) {
				same_this_way[dir] = true;
				same_count++;
			} else same_this_way[dir] = false;
		} else same_this_way[dir] = false;
		turn_left(x_dir, y_dir);
	}
	if(same_count == 0) {
		//No matches, no filling to do
		return;
	} else; //We've got a bite. Time to reel it in.

	if(same_count == 4) {
		//All matches ! Step backward until we find something different
		x_dir = -1, y_dir = 0;
		int x_alt = x, y_alt = y;
		while(in_image(x_alt, y_alt) &&
				test_depth == get_depth(x_alt, y_alt) &&
				test_switch == above_axis[y_alt][x_alt])
			step(x_alt, y_alt, x_dir, y_dir);
		turn_around(x_dir, y_dir);
		step(x_alt, y_alt, x_dir, y_dir);
		turn_left(x_dir, y_dir);
		find_and_fill_path(x_alt, y_alt, x_dir, y_dir);

		return;
	}
	x_dir=1, y_dir=0;
	for(int i=0; i<4; i++) {
		if(!same_this_way[i])
			break;
		turn_left(x_dir, y_dir);
	}
	turn_right(x_dir, y_dir);
	find_and_fill_path(x, y, x_dir, y_dir);
}

void Fractal_image::find_and_fill_path(int x, int y, int x_dir, int y_dir) {
	int x_rhs=x, y_rhs=y;
	turn_left(x_dir, y_dir);
	int x_lhs=x+x_dir, y_lhs=y+y_dir;
	turn_right(x_dir, y_dir);

	int temp_x = x_dir, temp_y = y_dir;

	int area_color = frac_data[y][x];
	bool area_shade = above_axis[y][x];

	do {
		step(x_rhs, y_rhs, x_dir, y_dir);
		if(!in_image(x_rhs, y_rhs) || // Gotta love short-circuit evaluation :)
			area_color != get_depth(x_rhs, y_rhs) ||
				area_shade != above_axis[y_rhs][x_rhs]) {
					// Missed a turn to the right
					turn_walker_right(x_rhs, y_rhs, x_lhs, y_lhs, x_dir, y_dir);
				} else {
					step(x_lhs, y_lhs, x_dir, y_dir);
					if(in_image(x_lhs, y_lhs) &&
							area_color == get_depth(x_lhs, y_lhs) &&
							area_shade == above_axis[y_lhs][x_lhs])
						turn_walker_left(x_rhs, y_rhs, x_lhs, y_lhs, x_dir, y_dir);
				}
	} while(!(x_rhs == x && y_rhs == y && x_dir == temp_x && y_dir == temp_y));

	int x_third, y_third;
	do {
		turn_right(x_dir, y_dir);
		x_third = x_rhs, y_third = y_rhs;
		step(x_third, y_third, x_dir, y_dir);
		turn_left(x_dir, y_dir);
		bleed_color(x_third, y_third, area_color, area_shade);

		step(x_rhs, y_rhs, x_dir, y_dir);
		if(!in_image(x_rhs, y_rhs) || // Gotta love short-circuit evaluation :)
			area_color != get_depth(x_rhs, y_rhs) ||
				area_shade != above_axis[y_rhs][x_rhs]) {
					// Missed a turn to the right
					turn_walker_right(x_rhs, y_rhs, x_lhs, y_lhs, x_dir, y_dir);
				} else {
					step(x_lhs, y_lhs, x_dir, y_dir);
					if(in_image(x_lhs, y_lhs) &&
							area_color == get_depth(x_lhs, y_lhs) &&
							area_shade == above_axis[y_lhs][x_lhs])
						turn_walker_left(x_rhs, y_rhs, x_lhs, y_lhs, x_dir, y_dir);
				}
	} while(!(x_rhs == x && y_rhs == y && x_dir == temp_x && y_dir == temp_y));

	// Follow the entire path but look toward the inside to see if any
	// of the spaces don't have a depth.
	// (Following the path again is cheap now because the depths at each
	// pixel have already been calculated.)
}

void Fractal_image::bleed_color(int x, int y, int depth, bool axis) {
	if(in_image(x, y) && !have_depth[y][x]) {
		have_depth[y][x] = true;
		frac_data[y][x] = depth;
		above_axis[y][x] = axis;
		bleed_color(x+1, y, depth, axis);
		bleed_color(x, y+1, depth, axis);
		bleed_color(x-1, y, depth, axis);
		bleed_color(x, y-1, depth, axis);
	}
}

void Fractal_image::turn_walker_right(int &x_rightfoot, int &y_rightfoot, 
		int &x_leftfoot, int &y_leftfoot, int &x_dir, int &y_dir) {
	x_leftfoot = x_rightfoot;
	y_leftfoot = y_rightfoot;
	turn_around(x_dir, y_dir);
	step(x_rightfoot, y_rightfoot, x_dir, y_dir);
	turn_left(x_dir, y_dir);
}

void Fractal_image::turn_walker_left(int &x_rightfoot, int &y_rightfoot, 
		int &x_leftfoot, int &y_leftfoot, int &x_dir, int &y_dir) {
	x_rightfoot = x_leftfoot;
	y_rightfoot = y_leftfoot;
	turn_around(x_dir, y_dir);
	step(x_leftfoot, y_leftfoot, x_dir, y_dir);
	turn_right(x_dir, y_dir);
	// Again with the code re-use :) 
	// Just have to put your feet on backwards
	// turn_walker_right(x_leftfoot, y_leftfoot, x_rightfoot, y_rightfoot, y_dir, x_dir);
}

void Fractal_image::square_z_and_add_c(mpf_t z_real, mpf_t z_imag, mpf_t c_real, mpf_t c_imag) {
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
}

void Fractal_image::init_c_from_specs(mpf_t c_val, int pos, int img_length, mpf_t img_center) {
	// This first step converts the position into a zero-centered value
	// with maximum = img_length and minimum = -img_length
	mpf_init_set_si(c_val, (2*pos - img_length));

	// Dividing by the zoom brings the value to the proper precision
	mpf_div(c_val, c_val, zoom);

	// Adding the img_center brings the value to the proper place
	mpf_add(c_val, c_val, img_center);
}

