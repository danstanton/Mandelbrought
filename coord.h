
struct Coord {
	int x, y;

	Coord() : x(0), y(0){};
	Coord(int a, int b) : x(a), y(b) {};
	Coord(const Coord &other) : x(other.x), y(other.y) {};

	void place_at(int a, int b) {
		x = a;
		y = b;
	};

	void place_at(Coord other) {
		place_at(other.x, other.y);
	};

	bool operator == ( const Coord rhs ) {
		return x == rhs.x
			&& y == rhs.y;
	};

	bool operator != ( const Coord rhs ) {
		return !( *this == rhs);
	};
};

void turn_left(int &x, int &y) {
	int temp = y;
	y = x;
	x = -temp;
};

void turn_left(Coord &dir) {
	turn_left(dir.x, dir.y);
};

void turn_right(Coord &dir) {
	turn_left(dir.y, dir.x); // How's that for code re-use? :)
};

void turn_around(Coord &dir) {
	dir.y=-dir.y;
	dir.x=-dir.x;
};

void step(Coord &result, Coord spot, Coord dir) {
	result.x = spot.x + dir.x;
	result.y = spot.y + dir.y;
};

void step(Coord &spot, Coord dir) {
	step(spot, spot, dir);
};

void turn_walker_right(
		Coord &leftfoot, Coord &rightfoot, Coord &direct) {
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
	leftfoot.place_at( rightfoot);
    turn_around(direct);
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
    step(rightfoot, direct);
    turn_left(direct);
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
};

void turn_walker_left(
		Coord &leftfoot, Coord &rightfoot, Coord &direct) {
    // Same as above except opposite direction
	rightfoot.place_at(leftfoot);
    turn_around(direct);
    step(leftfoot, direct);
    turn_right(direct);
};

