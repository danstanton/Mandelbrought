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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <gmp.h>

const int bound_size = 2;

class Fractal_image {
	public:
		bool (Fractal_image::*bound_check)(mpf_t, mpf_t); 
		int iter, img_width, img_height, **frac_data, calced, bled;
		bool **have_depth, **above_axis; 
		mpf_t focus_x, focus_y, zoom;

		void write_data_to_file(char *);

		bool out_of_diamond(mpf_t, mpf_t);
		bool out_of_circle(mpf_t, mpf_t);

		Fractal_image(char *);
		~Fractal_image();

		void square_z_and_add_c(mpf_t, mpf_t, mpf_t, mpf_t);
		void init_c_from_specs(mpf_t, int, int, mpf_t);

		void calc_depth(int, int);
		int get_depth(int, int);
		void fill_area(int, int);

		bool in_image(int, int);
		void turn_left(int &, int &);
		void turn_around(int &, int &);
		void turn_right(int &, int &);
		void step(int &, int &, int, int);

		bool flat_area(int, int, int, bool);
		void find_and_fill_path(int, int, int, int); 
		void turn_walker_left(int &, int &, int &, int &, int &, int &);
		void turn_walker_right(int &, int &, int &, int &, int &, int &);

		void bleed_color(int, int, int, bool);
};

