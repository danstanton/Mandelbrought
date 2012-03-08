#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <gmp.h>
#include <png.h>

#define MAX_READ 256

using namespace std;

typedef unsigned int uint;

/* flerr stands for "flag error".
 * This means that if an error condition is met, a flag will
 * be set so that code above can handle the error. At the same time, it
 * prints a little error message*/
bool flerr( bool error, string task) {
	if (error) 
		printf("Could not %s.\n", task.c_str());
	return error;
};

bool fleof( int error, string task) {
	return flerr(error == EOF, task);
};

int main( int ac, char ** av) {
	if ( ac < 3 ) {
		printf("Usage: %s dat_file color_file\n", av[0]);
		return 0;
	}

	string filename;

	for(uint i = 0; i < strlen(av[1])-4; i++)
		filename.push_back(av[1][i]);

	filename.push_back('-');
	filename.append(av[2]);
	filename.append(".png");

	cout<<filename<<endl;

	FILE *in_file=fopen(av[1], "r"),
		 *color_file=fopen(av[2], "r"),
		 *out_file=fopen(filename.c_str(), "wb");

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_colorp palette = NULL;

	bool err_jump = false;
	err_jump |= flerr(!in_file, "open data file");
	err_jump |= flerr(!color_file, "open color spec");
	err_jump |= flerr(!out_file, "ready image output file");
	if(err_jump) goto cleanup;

	png_ptr = png_create_write_struct(
			PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(flerr(!png_ptr, "create PNG memory structure")) 
		goto cleanup;
	info_ptr = png_create_info_struct(png_ptr);

	if(flerr(!info_ptr, "create PNG info structure")) 
		goto cleanup;


	// things to get from the data file:
	// img_width
	// img_height
	int err_cond;
	uint img_width, img_height;
	if(fleof(fscanf(in_file, "%ux%u", &img_width, &img_height), "read resolution"))
		goto cleanup;

	double rfq, gfq, bfq;
	int offset;
	if(fleof(fscanf(color_file, "%lf", &rfq), "read red color spec"))
		goto cleanup;
	if(fleof(fscanf(color_file, "%lf", &gfq), "read green color spec"))
		goto cleanup;
	if(fleof(fscanf(color_file, "%lf", &bfq), "read blue color spec"))
		goto cleanup;
	if(fleof(fscanf(color_file, "%d", &offset), "read offset"))
		goto cleanup;

	fclose(color_file);
	color_file = NULL;

	// Initialize the PNG structure for writing
	if(flerr(setjmp(png_jmpbuf(png_ptr)), "initialize PNG I/O")) 
		goto cleanup;
	png_init_io(png_ptr, out_file);

	if(flerr(setjmp(png_jmpbuf(png_ptr)), "write PNG header"))
		goto cleanup;
	png_set_IHDR(png_ptr, info_ptr, 
			img_width, img_height, 
			8 /* 8 means 24-bit RGB */, PNG_COLOR_TYPE_RGB, 
			PNG_INTERLACE_NONE, 
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	png_uint_32 k, height=img_height, width=img_width;
	png_byte image[height][width*3];
	png_bytep row_pointers[height];

	// use libpng example to fix this
	for (png_uint_32 k=0; k<height; k++)
		row_pointers[k] = image + k*width*3;

	int current;
	double red, green, blue;
	// now make the images 
	for(int y=0; y < img_height; y++) {
		for(int x=0; x < img_width; x++) {

			if(fleof(fscanf(in_file, "%u", &current), "read point value"))
				goto cleanup;

			255*cos(rfq*(double(current))+offset);
			red = 0.0, green = 0.0, blue = 0.0;

			if(current > 0) {
				image[y][(x*3)+0] = current%256;
				image[y][(x*3)+1] = current%256;
				image[y][(x*3)+2] = current%256;
			} 
		}
	}

	if(flerr(setjmp(png_jmpbuf(png_ptr)), "write PNG data"))
		goto cleanup;
	png_write_image(png_ptr, row_pointers);

	if(flerr(setjmp(png_jmpbuf(png_ptr)), "close PNG data"))
		goto cleanup;
	png_write_end(png_ptr, info_ptr);

cleanup:
	if(in_file) fclose(in_file);
	if(out_file) fclose(out_file);
	if(color_file) fclose(color_file);
	if(png_ptr) png_destroy_write_struct(&png_ptr, &info_ptr);

	return 0;
}
