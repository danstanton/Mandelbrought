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
#include "coord.h"

using namespace std;

bool Fractal_image::flat_area(int x, int y, int depth) { //, bool side) {
    return (in_image(x, y) && (get_depth(x, y) == depth)); // && (above_axis[y][x] == side));
}

bool cuts_the_set(double target) {
    return target <=1.0 && target >= -1.0;
};

void Fractal_image::become_image() {
    mpfr_t eedge, dedge;
    double edge[4];
    bool cut = false;

    // start with the middle line if needed
    mpfr_init(eedge);
    mpfr_init(dedge);

    mpfr_ui_div(dedge, img_width, zoom, GMP_RNDN);
    mpfr_div_ui(eedge, dedge, 2, GMP_RNDN);
    mpfr_add(eedge, eedge, focus_x, GMP_RNDN);
    edge[0] = mpfr_get_d(eedge, GMP_RNDN);
    mpfr_sub(eedge, eedge, dedge, GMP_RNDN);
    edge[2] = mpfr_get_d(eedge, GMP_RNDN);
    mpfr_ui_div(dedge, img_height, zoom, GMP_RNDN);
    mpfr_div_ui(eedge, dedge, 2, GMP_RNDN);
    mpfr_add(eedge, eedge, focus_y, GMP_RNDN);
    edge[1] = mpfr_get_d(eedge, GMP_RNDN);
    mpfr_sub(eedge, eedge, dedge, GMP_RNDN);
    edge[3] = mpfr_get_d(eedge, GMP_RNDN);

    cut = false;
    for(int i=0; i<4; i++)
        cut = cut || cuts_the_set(edge[i]);

    mpfr_clear(eedge);
    mpfr_clear(dedge);

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
        // It should step left when it realizes all the surrounding areas
        // are the same. Once it steps back far enough, it'll wrap an area
        // around the mandelbrot set counter-clockwise.
        // I'm using minus two because the image edge is also counted as
        // an edge, and we don't want to use that yet.
        for( int i=img_width-2; i>=0; i--)
            fill_area(i, center);
    }

    fill_all();
}

void Fractal_image::fill_all() {
    for(int j=0; j<img_height; j++)
        for(int i=0; i<img_width; i++)
            fill_area(i, j);
}

void Fractal_image::fill_area(int x, int y) {
    if(have_depth[y][x])
        return;
    calc_depth(x, y);

    int test_depth = frac_data[y][x];
    bool same_this_way[4];
    int same_count = 0;

    // Check the adjacent depths and see if anything matches
    // Start toward the right (not important, but must be consistent)

    Coord dir(1, 0), spot(x, y);
    for(int d=0; d<4; d++) {
        Coord adj(0, 0);
        step(adj, spot, dir);

        if(flat_area(adj.x, adj.y, test_depth)) {
            same_this_way[d] = true;
            same_count++;
        } else same_this_way[d] = false;

        turn_left(dir);
    }

    if(same_count == 0)
        return;

    if(same_count == 4) {
        //All matches ! Step left until we find something different
        dir.place_at(-1,0);
        Coord adj(x, y);

        while( flat_area( adj.x, adj.y, test_depth))
            step( adj, dir);

        //Now that we found an edge, take just one step back into the
        //region to be filled.
        turn_around(dir);
        step(adj,dir);

        //Now turn to the left and follow that edge
        turn_left(dir);
        find_and_fill_path( adj.x, adj.y, dir.x, dir.y);

        return;
    }

    // Turn the direction toward an edge
    dir.place_at(1,0);
    for( int i=0; i<4; i++) {
        if(!same_this_way[i])
            break;
        turn_left( dir);
    }

    // Now turn to the right and follow that edge
    turn_right(dir);
    find_and_fill_path(x, y, dir.x, dir.y);
}

void Fractal_image::find_and_fill_path(int x, int y, int x_d, int y_d) {
    int area_depth = frac_data[y][x];
	Coord spot(x, y), orig_dir(x_d, y_d);
	Coord leftfoot(0,0), rightfoot(x, y), dir(orig_dir);

    turn_left(dir);
	step(leftfoot, spot, dir);
    turn_right(dir);

    do {
        step(rightfoot, dir);
        if(!flat_area(rightfoot.x, rightfoot.y, area_depth)) {
            // Missed a turn to the right
            turn_walker_right(leftfoot, rightfoot, dir);
        } else {
            step(leftfoot, dir);
            if(flat_area(leftfoot.x, leftfoot.y, area_depth))
                turn_walker_left(leftfoot, rightfoot, dir);
        }
    } while( !(rightfoot == spot && dir == orig_dir));

    // Follow the entire path but look toward the inside to see if any
    // of the spaces don't have a depth.
    // (Following the path again is cheap now because the depths at each
    // pixel have already been calculated.)
	Coord paintcan(0,0);
    do {
		turn_right(dir);
		step(paintcan, rightfoot, dir);
		turn_left(dir);
        bleed_color(paintcan.x, paintcan.y, area_depth);

        step(rightfoot, dir);
        if(!flat_area(rightfoot.x, rightfoot.y, area_depth)) { 
            // Missed a turn to the right
            turn_walker_right(leftfoot, rightfoot, dir);
        } else {
            step(leftfoot, dir);
            if(flat_area(leftfoot.x, leftfoot.y, area_depth)) 
                turn_walker_left(leftfoot, rightfoot, dir);
        }
    } while(!(rightfoot == spot && dir == orig_dir));
}

void Fractal_image::bleed_color(int x, int y, int depth) { // , bool side) {
    if(in_image(x, y) && !have_depth[y][x]) {
        set_depth(x, y, depth);
        bled++;

        // bleed from top to bottom
        bleed_color(x, y-1, depth); 
    }
}

