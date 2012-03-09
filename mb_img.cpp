#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <utility> // for pair
#include <gmp.h>
#include <png.h>

#define MAX_READ 256

using namespace std;

typedef unsigned int uint;

struct color_node {
	char red, green, blue;
	color_node *next;
};

enum fn_type {LINEAR};

struct RGB {
	char elem[3];
};

RGB linear_mix( int current_position, int total_distance, RGB first, RGB second) {
	double mix_strength = double(current_position)/double(total_distance);
	RGB result;
	
	for(uint i=0; i<3; i++)
		result.elem[i] = linear_helper(mix_strength, first.elem[i], second.elem[i]);
	return result;
};

char linear_helper(double strength, char first, char second) {
	return (char)((1-strength)*first + strength*second);
};

pair<string,RGB (*)(int, int, RGB, RGB)> fns = {pair("linear",&linear_mix)};

struct fn_node {
	fn_type type;
	uint distance;
	fn_node *next;
};

/* This is a simple wrapper around an error condition. It will print
 * a very simple error message. */
bool flag_error( bool error, string task) {
	if (error) 
		printf("Could not %s.\n", task.c_str());
	return error;
};

bool flag_eof( int error, string task) {
	return flag_error(error == EOF, task);
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
	for(uint i = 0; i < strlen(av[2])-5; i++)
		filename.push_back(av[2][i]);
	filename.append(".png");

	printf("%s\n",filename.c_str());

	FILE *in_file=fopen(av[1], "r"),
		 *color_file=fopen(av[2], "r"),
		 *out_file=fopen(filename.c_str(), "wb");

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep *img_row = NULL;


	bool err_jump = false;
	err_jump |= flag_error(!in_file, "open data file");
	err_jump |= flag_error(!color_file, "open gradient file");
	err_jump |= flag_error(!out_file, "ready image output file");
	if(err_jump) goto cleanup;

	png_ptr = png_create_write_struct(
			PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(flag_error(!png_ptr, "create PNG memory structure")) 
		goto cleanup;
	info_ptr = png_create_info_struct(png_ptr);

	if(flag_error(!info_ptr, "create PNG info structure")) 
		goto cleanup;

	uint img_width, img_height; // <-- Things to get from the data file
	if(flag_eof(fscanf(in_file, "%ux%u", &img_width, &img_height), "read resolution"))
		goto cleanup;

	// vvvvv  Things to get from the color file
	color_node *head_color = new color_node, *temp_color = NULL;
	fn_node *head_fn = new fn_node, *temp_fn = NULL;
	head_color.next = NULL;
	head_fn.next = NULL;
	char *temp_read=NULL;

	fscanf(color_file, "%s", temp_read);

	if(flerr(!parseHex(temp_read), "parse color spec"))
		goto cleanup;

	
	
	

	// Initialize the PNG structure for writing
	if(flag_error(setjmp(png_jmpbuf(png_ptr)), "initialize PNG I/O")) 
		goto cleanup;
	png_init_io(png_ptr, out_file);

	if(flag_error(setjmp(png_jmpbuf(png_ptr)), "write PNG header"))
		goto cleanup;
	png_set_IHDR(png_ptr, info_ptr, 
			img_width, img_height, 
			8 /* <-- means 24-bit RGB */, PNG_COLOR_TYPE_RGB, 
			PNG_INTERLACE_NONE, 
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	img_row = new png_bytep[img_height];
	for(uint i = 0; i < img_height; i++) {
		img_row[i] = new png_byte[img_width*3];
	}

	int current;
	png_bytep pixel;
	// now make the images 
	for(uint y=0; y < img_height; y++) {
		for(uint x=0; x < img_width; x++) {

			if(flag_eof(fscanf(in_file, "%u", &current), "read point value"))
				goto cleanup;

			pixel = ((img_row[y])+(x*3));
			pixel[0] = (current+100)%256;
			pixel[1] = (current+80)%256;
			pixel[2] = current%256;
		}
	}

	if(flag_error(setjmp(png_jmpbuf(png_ptr)), "write PNG data"))
		goto cleanup;
	png_write_image(png_ptr, img_row);

	if(flag_error(setjmp(png_jmpbuf(png_ptr)), "close PNG data"))
		goto cleanup;
	png_write_end(png_ptr, info_ptr);

cleanup:
	if(in_file) fclose(in_file);
	if(color_file) fclose(color_file);
	if(out_file) fclose(out_file);
	if(png_ptr) png_destroy_write_struct(&png_ptr, &info_ptr);
	if(img_row) {
		for(uint i = 0; i < img_height; i++) {
			if(img_row[i]){
				delete [] img_row[i];
				img_row[i] = NULL;
			}
		}
		delete [] img_row;
		img_row = NULL;
	}

	return 0;
}
