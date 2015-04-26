#ifndef NODE_H_
#define NODE_H_

#include "enums.h"
#include "position.h"
#include "cardinalDirection.h"

struct node {
	position pos;
	bool visited;
	node* connections[4]; // 0 = South (where mouse came from), 1 = West, 2 = North, 3 = East
	int8_t costs[4];
	int8_t gCost;
	node* parent;

	node(){
		this->visited = false;
		for (int i=0; i<4; i++){
			this->connections[i] = NULL;
			this->costs[i] = -1;
			this->gCost = -1;
			this->parent = NULL;
		}
	}
	~node(){
		for (int i=0; i<4; i++){
			delete connections[i];
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