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

void Fractal_image::step(int &x, int &y, int x_dir, int y_dir) {
	x+=x_dir;
	y+=y_dir;
}

void Fractal_image::fill_area(int x, int y) {
	if(have_depth[y][x])
		return;

	calc_depth(x, y);
	int test_depth = frac_data[y][x];
	bool test_switch = above_axis[y][x];

	bool same_this_way[4];
	int same_count=0;
	// Check the adjacent depths and see if anything matches
	int x_dir = 1, y_dir = 0;
	for(int dir=0; dir<4; dir++) {
		int x_alt = x, y_alt = y;
		step(x_alt, y_alt, x_dir, y_dir);
		if(in_image(x_alt, y_alt)) {
			if(test_depth == get_depth(x_alt, y_alt)
					&& test_switch == above_axis[y_alt][x_alt]) {
				same_this_way[dir] = true;
				same_count++;
			} else same_this_way[dir] = false;
		} else same_this_way[dir] = false;
		turn_left(x_dir, y_dir);
	}
	if(same_count == 0) {
		//No matches, no filling to do
		return;
	} else; //We've got a bite. Time to reel it in.

	if(same_count == 4) {
		//All matches ! Step backward until we find something different
		x_dir = -1, y_dir = 0;
		int x_alt = x, y_alt = y;
		while(in_image(x_alt, y_alt) &&
				test_depth == get_depth(x_alt, y_alt) &&
				test_switch == above_axis[y_alt][x_alt])
			step(x_alt, y_alt, x_dir, y_dir);
		turn_around(x_dir, y_dir);
		step(x_alt, y_alt, x_dir, y_dir);
		turn_left(x_dir, y_dir);
		find_and_fill_path(x_alt, y_alt, x_dir, y_dir);

		return;
	}
	x_dir=1, y_dir=0;
	for(int i=0; i<4; i++) {
		if(!same_this_way[i])
			break;
		turn_left(x_dir, y_dir);
	}
	turn_right(x_dir, y_dir);
	find_and_fill_path(x, y, x_dir, y_dir);
}

void Fractal_image::find_and_fill_path(int x, int y, int x_dir, int y_dir) {
	int x_rhs=x, y_rhs=y;
	turn_left(x_dir, y_dir);
	int x_lhs=x+x_dir, y_lhs=y+y_dir;
	turn_right(x_dir, y_dir);

	int temp_x = x_dir, temp_y = y_dir;

	int area_color = frac_data[y][x];
	bool area_shade = above_axis[y][x];

	do {
		step(x_rhs, y_rhs, x_dir, y_dir);
		if(!in_image(x_rhs, y_rhs) || // Gotta love short-circuit evaluation :)
			area_color != get_depth(x_rhs, y_rhs) ||
				area_shade != above_axis[y_rhs][x_rhs]) {
					// Missed a turn to the right
					turn_walker_right(x_rhs, y_rhs, x_lhs, y_lhs, x_dir, y_dir);
				} else {
					step(x_lhs, y_lhs, x_dir, y_dir);
					if(in_image(x_lhs, y_lhs) &&
							area_color == get_depth(x_lhs, y_lhs) &&
							area_shade == above_axis[y_lhs][x_lhs])
						turn_walker_left(x_rhs, y_rhs, x_lhs, y_lhs, x_dir, y_dir);
				}
	} while(!(x_rhs == x && y_rhs == y && x_dir == temp_x && y_dir == temp_y));

	int x_third, y_third;
	do {
		turn_right(x_dir, y_dir);
		x_third = x_rhs, y_third = y_rhs;
		step(x_third, y_third, x_dir, y_dir);
		turn_left(x_dir, y_dir);
		bleed_color(x_third, y_third, area_color, area_shade);

		step(x_rhs, y_rhs, x_dir, y_dir);
		if(!in_image(x_rhs, y_rhs) || // Gotta love short-circuit evaluation :)
			area_color != get_depth(x_rhs, y_rhs) ||
				area_shade != above_axis[y_rhs][x_rhs]) {
					// Missed a turn to the right
					turn_walker_right(x_rhs, y_rhs, x_lhs, y_lhs, x_dir, y_dir);
				} else {
					step(x_lhs, y_lhs, x_dir, y_dir);
					if(in_image(x_lhs, y_lhs) &&
							area_color == get_depth(x_lhs, y_lhs) &&
							area_shade == above_axis[y_lhs][x_lhs])
						turn_walker_left(x_rhs, y_rhs, x_lhs, y_lhs, x_dir, y_dir);
				}
	} while(!(x_rhs == x && y_rhs == y && x_dir == temp_x && y_dir == temp_y));

	// Follow the entire path but look toward the inside to see if any
	// of the spaces don't have a depth.
	// (Following the path again is cheap now because the depths at each
	// pixel have already been calculated.)
}

void Fractal_image::bleed_color(int x, int y, int depth, bool axis) {
	if(in_image(x, y) && !have_depth[y][x]) {
		have_depth[y][x] = true;
		frac_data[y][x] = depth;
		above_axis[y][x] = axis;
		fill_count++;
		bleed_color(x+1, y, depth, axis);
		bleed_color(x, y+1, depth, axis);
		bleed_color(x-1, y, depth, axis);
		bleed_color(x, y-1, depth, axis);
	}
}

void Fractal_image::turn_walker_right(int &x_rightfoot, int &y_rightfoot, 
		int &x_leftfoot, int &y_leftfoot, int &x_dir, int &y_dir) {
	x_leftfoot = x_rightfoot;
	y_leftfoot = y_rightfoot;
	turn_around(x_dir, y_dir);
	step(x_rightfoot, y_rightfoot, x_dir, y_dir);
	turn_left(x_dir, y_dir);
}

void Fractal_image::turn_walker_left(int &x_rightfoot, int &y_rightfoot, 
		int &x_leftfoot, int &y_leftfoot, int &x_dir, int &y_dir) {
	x_rightfoot = x_leftfoot;
	y_rightfoot = y_leftfoot;
	turn_around(x_dir, y_dir);
	step(x_leftfoot, y_leftfoot, x_dir, y_dir);
	turn_right(x_dir, y_dir);
	// Again with the code re-use :) 
	// Just have to put your feet on backwards
	// turn_walker_right(x_leftfoot, y_leftfoot, x_rightfoot, y_rightfoot, y_dir, x_dir);
}

