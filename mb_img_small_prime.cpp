#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <pngwriter.h>

#define MAX_READ 256

using namespace std;

int main( int ac, char ** av) {
	if ( ac < 3 ) {
		printf("Usage: %s in_file number\n", av[0]);
		return 1;
	}

	FILE *in_file;
	in_file = fopen(av[1], "r");

	if (in_file == NULL) {
		printf("Could not open file.\n");
		return 1;
	}

	// things to get from the file:
	// img_width
	// img_height
	int *img_width = new int(), *img_height = new int();
	fscanf(in_file, "%ux%u", img_width, img_height);

	// make the image
	char *filename = new char[50];
	char *number = new char[10];
	int i = atoi(av[2]);

	if(i < 10)
		sprintf(number, "00%u", i);
	else if(i < 100)
		sprintf(number, "0%u", i);
	else 
		sprintf(number, "%u", i);

	filename[0] = '\0';
	strcat(filename, av[1]);
	strcat(filename, ".");
	strcat(filename, number);
	strcat(filename, ".png");

	pngwriter frac(*img_width, *img_height, 0, filename);
	// now make the images 
	for(int y=0; y < *img_height; y++) {
		for(int x=0; x < *img_width; x++) {
			int *current = new int();
			fscanf(in_file, "%u", current);
			double red = 0.0, green = 0.0, blue = 0.0;
			if(*current > 0) {
				red=pow(cos(0.04347826087*double(*current+(i<<2))), 2.0);
				green=pow(cos(0.03448275862*double(*current+(i<<2))), 2.0);
				blue=pow(cos(0.0322580645*double(*current+(i<<2))), 2.0);
			} 

			frac.plot(x, y, red, green, blue);
			delete current;
		}
	}
	frac.close();

	delete [] number;
	delete [] filename;

	fclose(in_file);

	delete img_width;
	delete img_height;
	return 0;
}
