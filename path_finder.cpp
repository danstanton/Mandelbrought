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

#include "fractal_image.h"

using namespace std;

void Fractal_image::turn_left(int &x, int &y) {
	int temp=y;
	y=x;
	x=-temp;
}

void Fractal_image::turn_around(int &x, int &y) {
	y=-y;
	x=-x;
}

void Fractal_image::turn_right(int &x, int &y) {
	turn_left(y, x); // How's that for code re-use? :)
}

void Fractal_image::step(int &x, int &y, int x_direction, int y_direction) {
	x+=x_direction;
	y+=y_direction;
}

bool Fractal_image::flat_area(int x, int y, int depth) { //, bool side) {
	return (in_image(x, y) && (get_depth(x, y) == depth)); // && (above_axis[y][x] == side));
}

bool in_two(double target) {
	return target <=1.0 && target >= -1.0;
};

void Fractal_image::become_image() {
	mpf_t eedge, dedge;
	double edge[4];
	bool cut = false;

	// start with the middle line if needed
	mpf_init(eedge);
	mpf_init(dedge);

	mpf_ui_div(dedge, img_width, zoom);
	mpf_div_ui(eedge, dedge, 2);
	mpf_add(eedge, eedge, focus_x);
	edge[0] = mpf_get_d(eedge);
	mpf_sub(eedge, eedge, dedge);
	edge[2] = mpf_get_d(eedge);
	mpf_ui_div(dedge, img_height, zoom);
	mpf_div_ui(eedge, dedge, 2);
	mpf_add(eedge, eedge, focus_y);
	edge[1] = mpf_get_d(eedge);
	mpf_sub(eedge, eedge, dedge);
	edge[3] = mpf_get_d(eedge);

	cut = false;
	for(int i=0; i<4; i++)
		cut = cut || in_two(edge[i]);

	mpf_clear(eedge);
	mpf_clear(dedge);

	if(!cut) {
		//printf("View of Mandelbrot set is whole.\n");
		// Find the center line
		int center;
		// set edge[0] as the size of the image
		edge[0] = (edge[1]-edge[3]);

		// edge[1] is already the distance of y=0 from the top
		// edge[1]/edge[0] will be the fractional distance of the line
		// y=0 from the top of the image.

		center = (edge[1]/edge[0])*img_height;
		// draw the line down the center
		// It should hopefully step left when it realizes all the
		// surrounding areas are the same. Once it steps back far
		// enough, it'll wrap an area around the mandelbrot set.
		// I'm using minus two because the image edge is also counted as
		// an edge, and we don't want to use that yet.
		for( int i=img_width-2; i>=0; i--)
			fill_area(i, center);
	}

	for(int j=0; j<img_height; j++)
		for(int i=0; i<img_width; i++)
			fill_area(i, j);
}

void Fractal_image::fill_area(int x, int y) {
	if(have_depth[y][x])
		return;
	calc_depth(x, y);

	int test_depth = frac_data[y][x];
	// bool test_side = above_axis[y][x];

	bool same_this_way[4];
	int same_count = 0;

	// Check the adjacent depths and see if anything matches
	// Start toward the right (not important, but must be consistent)
	int x_direction = 1, y_direction = 0;
	for(int dir=0; dir<4; dir++) {
		int x_adjacent = x+x_direction, y_adjacent = y+y_direction;

		if(flat_area(x_adjacent, y_adjacent, test_depth)) { //, test_side)) {
			same_this_way[dir] = true;
			same_count++;
		} else same_this_way[dir] = false;

		turn_left(x_direction, y_direction);
	}

	if(same_count == 0) {
		//No matches, no filling to do
		return;
	} 

	if(same_count == 4) {
		//All matches ! Step left until we find something different
		x_direction = -1, y_direction = 0;
		int x_adjacent = x, y_adjacent = y;

		while(flat_area(x_adjacent, y_adjacent, test_depth)) { //, test_side)) {
			step(x_adjacent, y_adjacent, x_direction, y_direction);
		}

		//Now that we found an edge, take just one step back into the
		//region to be filled.
		turn_around(x_direction, y_direction);
		step(x_adjacent, y_adjacent, x_direction, y_direction);

		//Now turn to the left and follow that edge
		turn_left(x_direction, y_direction);
		find_and_fill_path(x_adjacent, y_adjacent, x_direction, y_direction);

		return;
	}

	// Turn the direction toward an edge
	x_direction=1, y_direction=0;
	for(int i=0; i<4; i++) {
		if(!same_this_way[i])
			break;
		turn_left(x_direction, y_direction);
	}

	// Now turn to the right and follow that edge
	turn_right(x_direction, y_direction);
	find_and_fill_path(x, y, x_direction, y_direction);
}

void Fractal_image::find_and_fill_path(int x, int y, int x_direction, int y_direction) {
	int x_rightfoot = x, y_rightfoot = y;
	turn_left(x_direction, y_direction);
	int x_leftfoot = x+x_direction, y_leftfoot = y+y_direction;
	turn_right(x_direction, y_direction);

	int x_dir = x_direction, y_dir = y_direction;
	int area_depth = frac_data[y][x];
	// bool area_side = above_axis[y][x];

	// int path_length = 0;
	do {
		step(x_rightfoot, y_rightfoot, x_direction, y_direction);
		if(!flat_area(x_rightfoot, y_rightfoot, area_depth)) { //, area_side)) {
			// Missed a turn to the right
			turn_walker_right(x_rightfoot, y_rightfoot, x_leftfoot, y_leftfoot, x_direction, y_direction);
		} else {
			step(x_leftfoot, y_leftfoot, x_direction, y_direction);
			if(flat_area(x_leftfoot, y_leftfoot, area_depth)) // , area_side))
				turn_walker_left(x_rightfoot, y_rightfoot, x_leftfoot, y_leftfoot, x_direction, y_direction);
		}
		// path_length++;
	} while(x_rightfoot != x || y_rightfoot != y || x_direction != x_dir || y_direction != y_dir);

	// printf("Path length: %u \n", path_length);
	//
	// Follow the entire path but look toward the inside to see if any
	// of the spaces don't have a depth.
	// (Following the path again is cheap now because the depths at each
	// pixel have already been calculated.)
	int x_paintcan, y_paintcan;
	do {
		turn_right(x_direction, y_direction);
		x_paintcan = x_rightfoot+x_direction, y_paintcan = y_rightfoot+y_direction;
		turn_left(x_direction, y_direction);
		bleed_color(x_paintcan, y_paintcan, area_depth); //, area_side);

		step(x_rightfoot, y_rightfoot, x_direction, y_direction);
		if(!flat_area(x_rightfoot, y_rightfoot, area_depth)) { // , area_side)) {
			// Missed a turn to the right
			turn_walker_right(x_rightfoot, y_rightfoot, x_leftfoot, y_leftfoot, x_direction, y_direction);
		} else {
			step(x_leftfoot, y_leftfoot, x_direction, y_direction);
			if(flat_area(x_leftfoot, y_leftfoot, area_depth)) // , area_side))
				turn_walker_left(x_rightfoot, y_rightfoot, x_leftfoot, y_leftfoot, x_direction, y_direction);
		}
	} while(x_rightfoot != x || y_rightfoot != y || x_direction != x_dir || y_direction != y_dir);
}

void Fractal_image::bleed_color(int x, int y, int depth) { // , bool side) {
	if(in_image(x, y) && !have_depth[y][x]) {
		set_depth(x, y, depth);
		bled++;

		// bleed from top to bottom
		bleed_color(x, y-1, depth); // , side);
	}
}

void Fractal_image::turn_walker_right(int &x_rightfoot, int &y_rightfoot, 
		int &x_leftfoot, int &y_leftfoot, int &x_direction, int &y_direction) {
	// Right now, the walker's feet should be oriented like this
	// (relatively):
	// ---+---+---+---+---+
	//    |   |   |   |   |
	// ---+---+---+---+---+
	//    |   |rht|   |   |
	// ---+---+---+---+---+
	//    |lft|   |   |   |
	// ---+---+---+---+---+
	//    |   |   |   |   |
	// ---+---+---+---+---+
	// 
	// And the direction should be up in this case.
	// 
	x_leftfoot = x_rightfoot;
	y_leftfoot = y_rightfoot;
	turn_around(x_direction, y_direction);
	// Right now, the walker's feet should be oriented like this
	// (relatively):
	// ---+---+---+---+---+
	//    |   |   |   |   |
	// ---+---+---+---+---+
	//    |   |rht|   |   |
	//    |   |lft|   |   |
	// ---+---+---+---+---+
	//    |   |   |   |   |
	// ---+---+---+---+---+
	//    |   |   |   |   |
	// ---+---+---+---+---+
	// 
	// And the direction should be down.
	// 
	step(x_rightfoot, y_rightfoot, x_direction, y_direction);
	turn_left(x_direction, y_direction);
	// Right now, the walker's feet should be oriented like this
	// (relatively):
	// ---+---+---+---+---+
	//    |   |   |   |   |
	// ---+---+---+---+---+
	//    |   |lft|   |   |
	// ---+---+---+---+---+
	//    |   |rht|   |   |
	// ---+---+---+---+---+
	//    |   |   |   |   |
	// ---+---+---+---+---+
	// 
	// And the direction should be right.
	// 
}

void Fractal_image::turn_walker_left(int &x_rightfoot, int &y_rightfoot, 
		int &x_leftfoot, int &y_leftfoot, int &x_direction, int &y_direction) {
	// Same as above except opposite direction
	x_rightfoot = x_leftfoot;
	y_rightfoot = y_leftfoot;
	turn_around(x_direction, y_direction);
	step(x_leftfoot, y_leftfoot, x_direction, y_direction);
	turn_right(x_direction, y_direction);
}

