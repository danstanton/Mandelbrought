#include "fractal_image.h"

using namespace std;

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

