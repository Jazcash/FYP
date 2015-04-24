#ifndef NODESTACK_H_
#define NODESTACK_H_

#include "node.h"
#include <stdbool.h>

//struct NodeStack {
//public:
	//NodeStack() : top(0) {
		//
	//}
	//~NodeStack() {
		//while (!isEmpty()) {
			//pop();
		//}
		//isEmpty();
	//}
//
	//void push (node* n, Direction d);
	//node* getTop();
	//Direction pop();
	//bool isEmpty();
	//int size();
	//node* getNodeByPosition(position p);
	//node* getNodeWithUnvisitedConnections();
//protected:
	//struct StackNode {
		//node* data;
		//StackNode* next;
		//Direction enteredFrom;
//
		//StackNode(node* newData, StackNode* nextNode, Direction d) : data(newData), next(nextNode), enteredFrom(d) {}
	//};
//
	//NodeStack(NodeStack& lhs) {}
//
	//StackNode* top;
	//
	//StackNode* _getNodeByPosition(StackNode* n, position p);
	//StackNode* _getNodeWithUnvisitedConnections(StackNode* n);
//};
//
//void NodeStack::push(node* n, Direction d) {
	//top = new StackNode(n, top, d);
//}
//
//Direction NodeStack::pop() {
	//if ( !isEmpty() ) {
		//StackNode *topNode = top;
		//top = top->next;
		////node* data = topNode->data;
		//delete topNode;
		//Direction d = topNode->enteredFrom;
		//return d;
		////return data;
	//}
	//return NORTH;
//}
//
//node* NodeStack::top() {
	//if ( !isEmpty() ) {
		//return top->data;
	//}
	//return NULL;
//}
//
//bool NodeStack::isEmpty() {
	//if (top == 0) {
		//return true;
	//}
	//else {
		//return false;
	//}
//}
//
//int NodeStack::size(){
	//int count = 0;
	//StackNode* n = this->top;
	//while(n != NULL){
		//count++;
		//n = n->next;
	//}
	//return count;
//}
//
//node* NodeStack::getNodeWithUnvisitedConnections(){
	//StackNode* foundNode = _getNodeWithUnvisitedConnections(top);
	//if (foundNode == NULL){
		//return NULL;
	//}
	//return foundNode->data;
//}
//
//NodeStack::StackNode* NodeStack::_getNodeWithUnvisitedConnections(StackNode* n){
	//for (int i=0; i<4; i++){
		//if (n->data->connections[i] != NULL){
			//return n;
		//}
	//}
	//if (n->next != NULL){
		//_getNodeWithUnvisitedConnections(n->next);
	//} else {
		//return NULL;
	//}
	//return NULL;
//}
//
//node* NodeStack::getNodeByPosition(position p){
	//StackNode* foundNode = _getNodeByPosition(top, p);
	//if (foundNode == NULL){
		//return NULL;
	//}
	//return foundNode->data;
//}
//
//NodeStack::StackNode* NodeStack::_getNodeByPosition(StackNode* n, position p){
	//if (n->data->pos == p){
		//return n;
	//}
	//if (n->next != NULL){
		//_getNodeByPosition(n->next, p);
	//} else {
		//return NULL;
	//}
	//return NULL;
//}

#define STACK_MAX 50

struct NodeStack {
	node* data[STACK_MAX];
	int8_t size;
	NodeStack(){
		size = 0;
	}
	int8_t getSize();
	bool isEmpty();
	node* getTop();
	void push(node* n);
	void pop();
	node* getNodeByPosition(int8_t x, int8_t y);
	node* getNodeByPosition(position p);
};

int8_t NodeStack::getSize(){
	return this->size;
}

bool NodeStack::isEmpty(){
	return (size == 0);
}

node* NodeStack::getTop(){
	if (size > 0){
		return data[size-1];
		} else {
		return NULL;
	}
}

void NodeStack::push(node* n){
	if (size < STACK_MAX){
		data[size++] = n;
	}
}

void NodeStack::pop(){
	if (size > 0){
		size--;
	}
}

node* NodeStack::getNodeByPosition(int8_t x, int8_t y){
	position p;
	p.x = x;
	p.y = y;
	return getNodeByPosition(p);
}

node* NodeStack::getNodeByPosition(position p){
	for (int i=0; i<size; i++){
		if (data[i]->pos = p){
			return data[i];
		}
	}
	return NULL;
}

#endif /* NODESTACK_H_ */