#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <gmp.h>
#include <pngwriter.h>

#define MAX_READ 256

using namespace std;

typedef unsigned int uint;

int main( int ac, char ** av) {
	if ( ac < 3 ) {
		printf("Usage: %s dat_file color_file\n", av[0]);
		return 1;
	}

	FILE *in_file;
	in_file = fopen(av[1], "r");

	if (in_file == NULL) {
		printf("Could not open file.\n");
		return 1;
	}

	FILE *color_file;
	color_file = fopen(av[2], "r");
	
	if (color_file == NULL) {
		printf("Could not open color file.\n");
		return 1;
	}

	int temp_test;

	// things to get from the file:
	// img_width
	// img_height
	int *img_width = new int(), *img_height = new int();
	temp_test = fscanf(in_file, "%ux%u", img_width, img_height);
	if (temp_test == EOF) {
		printf("Could not read resolution.\n");
		return 1;
	}

	double rfq, gfq, bfq;
	temp_test = fscanf(color_file, "%lf", &rfq);
	if (temp_test == EOF) {
		printf("Could not read color.\n");
		return 1;
	}
	temp_test = fscanf(color_file, "%lf", &gfq);
	if (temp_test == EOF) {
		printf("Could not read color.\n");
		return 1;
	}
	temp_test = fscanf(color_file, "%lf", &bfq);
	if (temp_test == EOF) {
		printf("Could not read color.\n");
		return 1;
	}

	int offset;
	temp_test = fscanf(color_file, "%d", &offset);
	if (temp_test == EOF) {
		printf("Could not read offset.\n");
		return 1;
	}

	fclose(color_file);

	// make the image
	char *filename = new char[50];
	/*
	char *number = new char[10];
	int i = atoi(av[2]);

	if(i < 10)
		sprintf(number, "00%u", i);
	else if(i < 100)
		sprintf(number, "0%u", i);
	else 
		sprintf(number, "%u", i);
	*/

	/*
	filename[0] = '\0';
	strcat(filename, av[1]);
	*/
	for(uint i = 0; i < strlen(av[1])-4; i++) {
		filename[i] = av[1][i];
	}
	filename[strlen(av[1])-4] = '\0';
	strcat(filename, "-");
	strcat(filename, av[2]);
	strcat(filename, ".png");

	cout<<filename<<endl;

	pngwriter frac(*img_width, *img_height, 0, filename);

	int current;
	double red, green, blue;
	// now make the images 
	for(int y=0; y < *img_height; y++) {
		for(int x=0; x < *img_width; x++) {
			temp_test = fscanf(in_file, "%u", &current);
			if (temp_test == EOF) {
				printf("Could not read point value.\n");
				return 1;
			}
			red = 0.0, green = 0.0, blue = 0.0;
			if(current > 0) {
				red=cos(rfq*(double(current))+offset);
				red = red * red;
				green=cos(gfq*(double(current))+offset);
				green = green * green;
				blue=cos(bfq*(double(current))+offset);
				blue = blue * blue;
			} 

			frac.plot(x, y, red, green, blue);
		}
	}

	frac.close();

	// delete [] number;
	delete [] filename;

	fclose(in_file);


	delete img_width;
	delete img_height;
	return 0;
}
