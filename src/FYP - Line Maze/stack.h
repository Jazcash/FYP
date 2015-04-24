#ifndef NODE_H_
#define NODE_H_

struct Stack {
	Stack():top(0) {
		
	}
	~Stack() {
		while ( !isEmpty() ) {
			pop();
		}
		isEmpty();
	}

	void push (const T& object);
	T getTop();
	T pop();
	T& topElement();
	bool isEmpty();
	protected:
	struct StackNode {              // linked list node
		T data;                     // data at this node
		StackNode* next;            // next node in list

		// StackNode constructor initialises both fields
		StackNode(const T& newData, StackNode *nextNode)
		: data(newData), next(nextNode) {}
	};

	// My Stack should not allow copy of entire stack
	Stack(const Stack& lhs) {}

	// My Stack should not allow assignment of one stack to another
	Stack& operator=(const Stack& rhs) {}
	StackNode* top; // top of stack
};

template <class T>
void Stack<T>::push(const T& obj) {
	top = new StackNode(obj, top);
}

template <class T>
T Stack<T>::pop() {
	if ( !isEmpty() ) {
		StackNode *topNode = top;
		top = top->next;
		T data = topNode->data;
		delete topNode;
		return data;
	}
	return NULL;
	//return 0;
}

template <class T>
T& Stack<T>::topElement() {
	if ( !isEmpty() ) {
		return top->data;
	}
}

template <class T>
bool Stack<T>::isEmpty() {
	if (top == 0) {
		return true;
	}
	else {
		return false;
	}
}

#endif /* STACK_H_ */