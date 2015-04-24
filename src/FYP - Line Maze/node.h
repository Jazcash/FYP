#ifndef NODE_H_
#define NODE_H_

#include "enums.h"
#include "position.h"
#include "cardinalDirection.h"

struct node {
	position pos;
	bool visited;
	node* connections[4]; // 0 = South (where mouse came from), 1 = West, 2 = North, 3 = East
	//node* parent;
	//float f, g, h;

	node(){
		this->visited = false;
		for (int i=0; i<4; i++){
			this->connections[i] = NULL;
		}
	}
	
	void setPosition(position p){
		this->pos = p;
	}
	
	void setPosition(int x, int y){
		this->pos.x = x;
		this->pos.y = y;
	}
};

#endif /* NODE_H_ */