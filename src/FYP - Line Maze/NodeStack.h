#ifndef NODESTACK_H_
#define NODESTACK_H_

#include "node.h"
#include <stdbool.h>

struct NodeStack {
public:
	NodeStack() : top(0) {
		
	}
	~NodeStack() {
		while (!isEmpty()) {
			pop();
		}
		isEmpty();
	}

	void push (node* n, Direction d);
	node* getTop();
	Direction pop();
	bool isEmpty();
	int size();
	node* getNodeByPosition(position p);
	node* getNodeWithUnvisitedConnections();
protected:
	struct StackNode {
		node* data;
		StackNode* next;
		Direction enteredFrom;

		StackNode(node* newData, StackNode* nextNode, Direction d) : data(newData), next(nextNode), enteredFrom(d) {}
	};

	NodeStack(NodeStack& lhs) {}

	StackNode* top;
	
	StackNode* _getNodeByPosition(StackNode* n, position p);
	StackNode* _getNodeWithUnvisitedConnections(StackNode* n);
};

void NodeStack::push(node* n, Direction d) {
	top = new StackNode(n, top, d);
}

Direction NodeStack::pop() {
	if ( !isEmpty() ) {
		StackNode *topNode = top;
		top = top->next;
		//node* data = topNode->data;
		delete topNode;
		Direction d = topNode->enteredFrom;
		return d;
		//return data;
	}
	return NORTH;
}

node* NodeStack::getTop() {
	if ( !isEmpty() ) {
		return top->data;
	}
	return NULL;
}

bool NodeStack::isEmpty() {
	if (top == 0) {
		return true;
	}
	else {
		return false;
	}
}

int NodeStack::size(){
	int count = 0;
	StackNode* n = this->top;
	while(n != NULL){
		count++;
		n = n->next;
	}
	return count;
}

/* Alternative implementation using defined stack memory */

//#define STACK_MAX 40
//
//struct NodeStack {
	//struct StackNode {
		//node* n;
		//Direction enteredFrom;
	//};
	//
	//StackNode data[STACK_MAX];
	//int8_t size;
	//NodeStack(){
		//size = 0;
	//}
	//int8_t getSize();
	//bool isEmpty();
	//node* getTop();
	//void push(node* n, Direction d);
	//Direction pop();
	//node* getNodeByPosition(int8_t x, int8_t y);
	//node* getNodeByPosition(position p);
//};
//
//int8_t NodeStack::getSize(){
	//return this->size;
//}
//
//bool NodeStack::isEmpty(){
	//return (size == 0);
//}
//
//node* NodeStack::getTop(){
	//if (!isEmpty()){
		//return data[size-1].n;
	//}
	//return NULL;
//}
//
//void NodeStack::push(node* n, Direction d){
	//if (size < STACK_MAX){
		//data[size].n = n;
		//data[size].enteredFrom = d;
		//size++;
	//}
//}
//
//Direction NodeStack::pop(){
	//if (!isEmpty()){
		//Direction d = data[size].enteredFrom;
		//size--;
		//return d;
	//}
	//return NORTH;
//}

#endif /* NODESTACK_H_ */