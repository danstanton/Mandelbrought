#include "fractal_image.h"

using namespace std;

bool Fractal_image::out_of_diamond(mpf_t a_real, mpf_t a_imag) {
	bool too_big;
	mpf_t temp_real, temp_imag;
	mpf_init(temp_real);
	mpf_init(temp_imag);

	// ?? abs(real) + abs(imag) > bound_size ??
	mpf_abs(temp_real, a_real);
	mpf_abs(temp_imag, a_imag);
	mpf_add(temp_real, temp_real, temp_imag);
	if (mpf_cmp_ui(temp_real, bound_size) > 0)
		too_big = true;
	else
		too_big = false;
	mpf_clear(temp_real);
	mpf_clear(temp_imag);
	return too_big;
}

bool Fractal_image::out_of_circle(mpf_t a_real, mpf_t a_imag) {
	bool too_big;
	mpf_t temp_real, temp_imag;
	mpf_init(temp_real);
	mpf_init(temp_imag);

	// ?? sqrt( real^2 + imag^2 ) > bound_size ??
	mpf_mul(temp_real, a_real, a_real);
	mpf_mul(temp_imag, a_imag, a_imag);
	mpf_add(temp_real, temp_real, temp_imag);
	mpf_sqrt(temp_real, temp_real);
	if (mpf_cmp_ui(temp_real, bound_size) > 0)
		too_big = true;
	else
		too_big = false;

	mpf_clear(temp_real);
	mpf_clear(temp_imag);
	return too_big;
}
