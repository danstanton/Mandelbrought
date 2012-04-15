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

#include "read_in_file.h"

bool well_read(int scanner) {
	if(scanner == 0 || scanner == EOF) {
		printf("Read error from input file.\n");
		return false;
	} else {
		return true;
	}
}

bool read_in_file(
		FILE *in_file, 
		int &img_width, int &img_height, 
		mpfr_ptr focus_x, mpfr_ptr focus_y, 
		mpfr_ptr zoom, 
		int &iter) {
	char input_buffer[80];
	if(!well_read(fscanf(in_file, "Horizontal: %upx\n", &img_width))) return false;
	if(!well_read(fscanf(in_file, "Vertical: %upx\n", &img_height))) return false;

	if(well_read(fscanf(in_file, "X Focus: %s\n", input_buffer)))
		mpfr_init_set_str(focus_x, input_buffer, 10, GMP_RNDN);
	else return false;

	if(well_read(fscanf(in_file, "Y Focus: %s\n", input_buffer)))
		mpfr_init_set_str(focus_y, input_buffer, 10, GMP_RNDN);
	else return false;

	if(well_read(fscanf(in_file, "Zoom: %s\n", input_buffer)))
		mpfr_init_set_str(zoom, input_buffer, 10, GMP_RNDN);
	else return false;

	if(!well_read(fscanf(in_file, "Depth: %u\n", &iter))) return false;
	return true;
}
