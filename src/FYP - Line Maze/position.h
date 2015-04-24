#ifndef POSITION_H_
#define POSITION_H_

struct position {
	int8_t x, y;
	bool operator == (const position &RHS) {
		return (this->x == RHS.x && this->y == RHS.y);
	}
	bool operator != (const position &RHS) {
		return (this->x != RHS.x || this->y != RHS.y);
	}
};

#endif /* POSITION_H_ */