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

#include <cstdio>
#include "fractal_image.h"

using namespace std;


int main( int ac, char ** av) {
	if ( ac < 2 || ac > 3 ) {
		printf("Usage: %s in_file\n", av[0]);
		printf("   or: %s in_file in_file.dat\n", av[0]);
		return 1;
	}

	Fractal_image beauty(av[1]);
	if(!beauty.have_depth)
		return 1;

	if(ac == 3) {
		beauty.load_data(av[2]);
	}

	for (int y = 0; y < beauty.img_height; y++)
		for (int x = 0; x < beauty.img_width; x++)
			beauty.fill_area(x, y);

	// beauty.write_data_to_file(av[1]);
	return 0;
}
