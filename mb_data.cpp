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

	for (int y = 0; y < beauty.img_height; y++)
		for (int x = 0; x < beauty.img_width; x++)
			beauty.fill_area(x, y);

	beauty.write_data_to_file(av[1]);
	return 0;
}
