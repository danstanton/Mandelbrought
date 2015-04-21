#include <cctype> // for tolower
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <utility> // for pair
#include <gmp.h>
#include <png.h>

#define MAX_READ 256

using namespace std;

typedef unsigned int uint;
typedef unsigned char uchar;

struct RGB {
	uchar elem[3];
};

uchar hexDigit(char one) {
	if(isdigit(one))
		return (uchar)(one-'0');
	if(isalpha(one) && tolower(one)<'g')
		return (uchar)tolower(one)-'a'+10;
	return 255;
}

uchar hexPair(char *pair) {
	uchar valuePair[] = {hexDigit(pair[0]), hexDigit(pair[1])};
	for(int i=0; i < 2; i++)
		if(valuePair[i] == 255) {
			printf("%c is not a valid hexadecimal digit--using 0", pair[i]);
			valuePair[i] = 0;
		}
	return valuePair[0]*16 + valuePair[1];
}

RGB parseHex(char *color_str) {
	RGB group;
	if(strlen(color_str) != 6) {
		printf("Color spec \"%s\" is bad--using black", color_str);
		for(uint i=0; i<3; i++) 
			group.elem[i] = 0;
		return group;
	} else {
		for(uint i=0; i<3; i++)
			group.elem[i] = hexPair(color_str+(i*2));
		return group;
	}
};

struct color_node {
	RGB pixel;
	color_node *next;
};

// for all mixes, 0 would be full first, 1 would be full second
RGB apply_mix( double (*mixer)(int, int), int place, int range, RGB first, RGB second) {
	double mix_strength = mixer(place, range);
	RGB result;
	for(uint i=0; i<3; i++)
		result.elem[i] = (uchar)((1-mix_strength)*first.elem[i]+mix_strength*second.elem[i]);
	return result;
};

double linear_mix( int current_position, int total_distance) {
	return double(current_position)/double(total_distance);
};

RGB linear( int place, int range, RGB first, RGB second) {
	return apply_mix(&linear_mix, place, range, first, second);
};

RGB color1( int current_position, int total_distance, RGB first, RGB second) {
	return first;
};

RGB color2( int current_position, int total_distance, RGB first, RGB second) {
	return second;
};

typedef RGB (*gradient_fn)(int, int, RGB, RGB);
uint fn_count = 3; // REMEMBER TO CHANGE THIS NEXT TIME
gradient_fn fns[] = {&linear, &color1, &color2};
string fn_names[] = {"linear", "first", "second"};

struct fn_node {
	RGB (*blender)(int, int, RGB, RGB);
	uint distance;
	uint color_index;
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

	// vvvvv  Things to get from the color file
	uint color_count = 0, gradient_length = 0;
	color_node *head_color = new color_node, *temp_color = NULL, *prev_color = NULL;
	fn_node *head_fn = new fn_node, *temp_fn = NULL, *prev_fn = NULL;
	head_color->next = NULL;
	head_fn->next = NULL;
	temp_color = head_color;
	temp_fn = head_fn;
	char temp_read[30];


	FILE *in_file=fopen(av[1], "r"),
		 *color_file=fopen(av[2], "r"),
		 *out_file=fopen(filename.c_str(), "wb");

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep *img_row = NULL;

	// vvvvvvv For eventual gradient application
	uint color_bookmark=0, true_depth, gradient_depth, depth_bookmark;
	RGB dot_color;
	png_bytep pixel;

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

	// get color first
	if(flag_eof(fscanf(color_file, "%s", temp_read), "read first color"))
		goto cleanup;
	if(flag_error(temp_read[0] != '#', "parse first color"))
		goto cleanup;

	temp_color->pixel = parseHex((temp_read+1));
	temp_color->next = new color_node;
	prev_color = temp_color;
	temp_color = temp_color->next;
	color_count++;

	uint distance;
	bool found_match;
	while(fscanf(color_file, "%s", temp_read) != EOF) {
		if(temp_read[0] == '#') {
			//use color
			temp_color->pixel = parseHex((temp_read+1));
			temp_color->next = new color_node;
			prev_color = temp_color;
			temp_color = temp_color->next;
			temp_color->next = NULL; // in case an error breaks the read loop
			color_count++;
		} else {
			//find function
			for(uint i=0; i<strlen(temp_read); i++) 
				temp_read[i] = tolower(temp_read[i]);
			found_match = false;
			// simple linear search (adequate for under 50 functions)
			for(uint i=0; i<fn_count; i++) {
				if(strcmp(temp_read, fn_names[i].c_str())==0) {
					if(flag_eof(fscanf(color_file, "%u", &distance), "find distance after function"))
						goto cleanup;
					found_match = true;
					temp_fn->blender = fns[i];
					temp_fn->distance = distance;
					gradient_length += distance;
					temp_fn->color_index = color_count-1;
					temp_fn->next = new fn_node;
					prev_fn = temp_fn;
					temp_fn = temp_fn->next;
					temp_fn->next = NULL; // in case we need to error out and delete the list
					break;
				}
			}
			if(flag_error(!found_match, "match function"))
				goto cleanup;

		}
	}

	// close color loop
	delete temp_color;
	temp_color = NULL;
	prev_color->next = head_color;

	delete temp_fn;
	temp_fn = NULL;
	prev_fn->next = head_fn;

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

	temp_fn = head_fn;
	temp_color = head_color->next;
	prev_color = head_color;
	// now make the images 
	for(uint y=0; y < img_height; y++) {
		for(uint x=0; x < img_width; x++) {

			if(flag_eof(fscanf(in_file, "%u", &true_depth), "read point value"))
				goto cleanup;

			gradient_depth = true_depth%gradient_length;
			// find correct function for gradient depth
			temp_fn = head_fn;
			depth_bookmark = 0;
			while(depth_bookmark+temp_fn->distance <= gradient_depth) {
				depth_bookmark += temp_fn->distance;
				temp_fn = temp_fn->next;
			}
			// find correct colors for this function
			while(color_bookmark != temp_fn->color_index) {
				prev_color = temp_color;
				temp_color = temp_color->next;
				color_bookmark = (color_bookmark+1)%color_count;
			}

			// now use the inner function to set the color
			dot_color = (temp_fn->blender)(
					gradient_depth-depth_bookmark, temp_fn->distance,
					prev_color->pixel, temp_color->pixel);

			pixel = ((img_row[y])+(x*3));
			for(uint i=0; i<3; i++)
				pixel[i] = dot_color.elem[i];
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

	// Deleting a circular or regular linked list
	if(head_color) {
		temp_color = head_color->next;
		head_color->next = NULL;
		prev_color = head_color;
		head_color = temp_color;
		while(head_color) {
			temp_color = head_color->next;
			if(head_color == prev_color) {
				delete head_color;
				prev_color = NULL;
			}
			head_color = temp_color;
		}
		if(prev_color) {
			// Ah, it was just a regular list
			delete prev_color;
			prev_color = NULL;
		}
	}

	// Deleting another circular or regular list
	if(head_fn) {
		temp_fn = head_fn->next;
		head_fn->next = NULL;
		prev_fn = head_fn;
		head_fn = temp_fn;
		while(head_fn) {
			temp_fn = head_fn->next;
			if(head_fn == prev_fn) {
				delete head_fn;
				prev_fn = NULL;
			}
			head_fn = temp_fn;
		}
		if(prev_fn) {
			// it was a simple linked list
			delete prev_fn;
			prev_fn = NULL;
		}
	}

	return 0;
}
