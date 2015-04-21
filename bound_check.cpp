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

bool Fractal_image::out_of_diamond(mpfr_t a_real, mpfr_t a_imag) {
	bool too_big;
	mpfr_t temp_real, temp_imag;
	mpfr_init(temp_real);
	mpfr_init(temp_imag);

	// ?? abs(real) + abs(imag) > bound_size ??
	mpfr_abs(temp_real, a_real, GMP_RNDN);
	mpfr_abs(temp_imag, a_imag, GMP_RNDN);
	mpfr_add(temp_real, temp_real, temp_imag, GMP_RNDN);
	if (mpfr_cmp_ui(temp_real, bound_size) > 0)
		too_big = true;
	else
		too_big = false;
	mpfr_clear(temp_real);
	mpfr_clear(temp_imag);
	return too_big;
}

bool Fractal_image::out_of_circle(mpfr_t a_real, mpfr_t a_imag) {
	bool too_big;
	mpfr_t temp_real, temp_imag;
	mpfr_init(temp_real);
	mpfr_init(temp_imag);

	// ?? sqrt( real^2 + imag^2 ) > bound_size ??
	mpfr_mul(temp_real, a_real, a_real, GMP_RNDN);
	mpfr_mul(temp_imag, a_imag, a_imag, GMP_RNDN);
	mpfr_add(temp_real, temp_real, temp_imag, GMP_RNDN);
	mpfr_sqrt(temp_real, temp_real, GMP_RNDN);
	if (mpfr_cmp_ui(temp_real, bound_size) > 0)
		too_big = true;
	else
		too_big = false;

	mpfr_clear(temp_real);
	mpfr_clear(temp_imag);
	return too_big;
}
