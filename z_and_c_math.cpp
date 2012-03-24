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

