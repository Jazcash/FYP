#ifndef CARDINALDIRECTION_H_
#define CARDINALDIRECTION_H_

#include "enums.h"

struct cDirection {
	Direction dir;

	void next() {
		int faceInt = dir;
		if (faceInt == 3) {
			faceInt = -1;
		}
		this->dir = static_cast<Direction>(faceInt + 1);
	}
	void previous() {
		int faceInt = dir;
		if (faceInt <= 0) {
			faceInt = 4;
		}
		this->dir = static_cast<Direction>(faceInt - 1);
	}
	Direction getNext() {
		int faceInt = dir;
		if (faceInt >= 3) {
			faceInt = -1;
		}
		return static_cast<Direction>(faceInt + 1);
	}
	Direction getPrevious() {
		int faceInt = dir;
		if (faceInt <= 0) {
			faceInt = 4;
		}
		return static_cast<Direction>(faceInt - 1);
	}
	Direction getOpposite(){
		int faceInt = dir;
		if (faceInt >= 3) {
			faceInt = -1;
		}
		Direction temp = static_cast<Direction>(faceInt + 1);
		faceInt = temp;
		if (faceInt >= 3) {
			faceInt = -1;
		}
		return static_cast<Direction>(faceInt + 1);
	}
	operator Direction() { return dir; }
	void operator=(const Direction &lhs){ dir = lhs; }
	void operator=(const int8_t &lhs){ dir = static_cast<Direction>(lhs); }
};

#endif /* CARDINALDIRECTION_H_ */