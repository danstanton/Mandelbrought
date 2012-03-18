#include <cstdio>
#include "fractal_image.h"

using namespace std;

int main( int ac, char ** av) {
	if ( ac < 2 ) {
		printf("Usage: %s in_file\n", av[0]);
		return 1;
	}

	Fractal_image beauty(av[1]);
	if(!beauty.have_depth)
		return 1;


	// start the fractal
	for (int y_pos = 0; y_pos < beauty.img_height; y_pos++) {
		for (int x_pos = 0; x_pos < beauty.img_width; x_pos++) {
			beauty.get_depth(x_pos, y_pos);
		}
	}

	beauty.write_data_to_file(av[1]);
	return 0;
}
