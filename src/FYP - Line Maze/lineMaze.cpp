#include "lineMaze.h"

void initialise(){
	pololu_3pi_init(2000);
	load_custom_characters();
	load_custom_characters();
	loadCalibration();
	play_from_program_space(bloop);
	
	foundFinish = false;
	isLeft = false;
	isRight = false;
	isForward = false;
	
	currentDir.dir = NORTH;
	currentPos.x = 0;
	currentPos.y = 0;
	
	costCounter = 0;
	
	speed = 60;
}

void load_custom_characters(){
	lcd_load_custom_character(levels+0,0); // no offset, e.g. one bar
	lcd_load_custom_character(levels+1,1); // two bars
	lcd_load_custom_character(levels+2,2); // etc...
	lcd_load_custom_character(levels+3,3);
	lcd_load_custom_character(levels+4,4);
	lcd_load_custom_character(levels+5,5);
	lcd_load_custom_character(levels+6,6);
	clear();
}

void display_readings(const unsigned int *calibrated_values){
	print(" ");
	unsigned char i;
	for(i=0;i<5;i++) {
		const char display_characters[10] = {' ',0,0,1,2,3,4,5,6,255};
		char c = display_characters[calibrated_values[i]/101];
		print_character(c);
	}
}

void calibrate(){
	unsigned int counter;
	delay_ms(1000);
	for(counter=0;counter<80;counter++){
		if(counter < 20 || counter >= 60){
			set_motors(40,-40);
			} else {
			set_motors(-40,40);
		}
		calibrate_line_sensors(IR_EMITTERS_ON);
		delay_ms(20);
	}
	set_motors(0,0);
	
	saveCalibration();
}

int main(){
	initialise();
	
	while(1){
		clear();
		print(" ");
		print_long(read_battery_millivolts());
		print("mv");
		delay_ms(20);
		
		if (button_is_pressed(BUTTON_A)){
			calibrate();
		} else if(button_is_pressed(BUTTON_B)){
			mapMaze();
			
			clear();
			print(" Maze");
			lcd_goto_xy(0, 1);
			print(" mapped!");
			wait_for_button(BUTTON_B);
			
			speed = 100;
			
			astarTravel(finish);
			
			speed = 60;
			
		} else if(button_is_pressed(BUTTON_C)){
			unsigned int sensors[5];
			while(1){
				unsigned int position = read_line(sensors, IR_EMITTERS_ON);
				clear();
				print(" ");
				print_long(position);
				print(" ");
				//print_long(sensors[2]);
				lcd_goto_xy(0,1);
				display_readings(sensors);
				delay_ms(100);
			}
		}
	}
	
	return 0;
}

void faceDir(Direction d){
	if (currentDir.getNext() == d){
		turn(RIGHT);
		} else if(currentDir.getPrevious() == d){
		turn(LEFT);
		} else if(currentDir.getOpposite() == d){
		turn(BACK);
	}
}

void faceDir(int i){
	Direction d = static_cast<Direction>(i);
	faceDir(d);
}

struct NodeList {
	private:
	node* nodes[40];
	int8_t top;
	public:
	NodeList(){
		top = 0;
	}
	int8_t size(){
		return top;
	}
	void addNode(node* n){
		if (top < 40) {
			nodes[top] = n;
			top++;
		}
	}
	node* removeNodeByPosition(position p){
		return removeNodeByPosition(p.x, p.y);
	}
	node* removeNodeByPosition(int8_t x, int8_t y){
		if (isEmpty()){
			return NULL;
		}
		position p;
		p.x = x;
		p.y = y;
		int8_t i;
		node* n;
		for (i=0; i<top && nodes[i]->pos != p; ++i);
		n = nodes[i];
		if (i == top){
			return NULL;
		}
		for (;i<top-1; i++){
			nodes[i] = nodes[i+1];
		}
		top--;
		return n;
	}
	node* getNodeByPosition(position p){
		return getNodeByPosition(p.x, p.y);
	}
	node* getNodeByPosition(int8_t x, int8_t y){
		if (isEmpty()){
			return NULL;
		}
		position p;
		p.x = x;
		p.y = y;
		int8_t i;
		node* n;
		for (i=0; i<top && nodes[i]->pos != p; ++i);
		n = nodes[i];
		if (i == top){
			return NULL;
		}
		return n;
	}
	node* getNodeAtIndex(int8_t i){
		if (i < top){
			return nodes[i];
		}
		return NULL;
	}
	bool isNodeInList(node* n){
		for (int8_t i=0; i<top; i++){
			if (n == nodes[i]){
				return true;
			}
		}
		return false;
	}
	bool isEmpty(){
		return (top == 0);
	}
	node* operator[] (int8_t i) { return nodes[i]; }
};

int8_t hCost(node* a, node* b){
	int8_t x1 = a->pos.x,
	x2 = b->pos.x,
	y1 = a->pos.y,
	y2 = b->pos.y;

	int8_t xDiff = (x1 > x2) ? x1 - x2 : x2 - x1;
	int8_t yDiff = (y1 > y2) ? y1 - y2 : y2 - y1;
	int8_t totalDiff = xDiff + yDiff;

	return totalDiff;
}

void travelPath(node* n){
	while (n->parent != NULL){
		int8_t i;
		for (i=0; i<4; i++){
			if (n->connections[i] != NULL){
				if (n->connections[i] == n->parent){
					break;
				}
			}
		}

		if (i != 4){
			cDirection cd;
			cd = i;
		
			faceDir(i);
			followLine();

			n = n->parent;
		}
	}
}

void astarTravel(node* target){
	NodeList openlist;
	NodeList closedlist;

	target->gCost = 0;

	openlist.addNode(target);

	while (!openlist.isEmpty()){

		node* lowest = openlist.getNodeAtIndex(0);
		int8_t lowestFCost = lowest->gCost + hCost(lowest, root);
		for (int8_t i=0; i<openlist.size(); i++){
			int8_t fCost = openlist[i]->gCost + hCost(openlist[i], root);
			if (fCost < lowestFCost){
				lowest = openlist[i];
			}
		}

		node* currentNode = openlist.removeNodeByPosition(lowest->pos);

		closedlist.addNode(currentNode);

		if (currentNode == root){
			break;
		}

		for (int8_t i=0; i<4; i++){
			node* child = currentNode->connections[i];
			if (child != NULL){
				if (closedlist.isNodeInList(child)){
					continue;
				}
				if (!openlist.isNodeInList(child)){
					openlist.addNode(child);
					child->parent = currentNode;
					child->gCost = currentNode->gCost + currentNode->costs[i];
				} else {
					int8_t thisGCost = currentNode->gCost + currentNode->costs[i];
					if (thisGCost < openlist.getNodeByPosition(child->pos)->gCost){
						child->parent = currentNode;
						child->gCost = currentNode->gCost + currentNode->costs[i];
					}
				}
			}
		}
	}
	
	travelPath(root);
}

void mapMaze(){
	int startRAM = get_free_ram();
	
	currentDir.dir = NORTH;
	currentPos.x = 0;
	currentPos.y = 0;
	
	//myNodeStack = NodeStack();
	
	node* allNodes[50];
	int head = 0;
	
	delete root;
	root = new node;
	root->setPosition(currentPos);
	root->connections[2] = new node;
	root->visited = true;
	
	allNodes[head] = root;
	
	myNodeStack.push(root, NORTH);
	
	int i;
	while(!myNodeStack.isEmpty()){
		node* currentNode = myNodeStack.getTop();
		
		for (i=3; i>-1; i--){
			if (currentNode->connections[i] != NULL){
				if (currentNode->connections[i]->visited == false){
					break;
				}
			}
		}
		
		if (foundFinish == true){
			finish = currentNode;
		}
		
		foundFinish = false;
		
		if (i == -1){ // this node has no unvisited connections
			cDirection d;
			d = myNodeStack.pop();
			faceDir(d.getOpposite());
			followLine();
		} else {
			faceDir(i);
			int8_t distanceFromLastNode = followLine();
			
			node* nextNode = NULL;
			for (int j=0; j<=head; j++){
				if (allNodes[j]->pos == currentPos){
					nextNode = allNodes[j];
					break;
				}
			}
			
			if (nextNode == NULL){ // new node
				play_from_program_space(bloop);
				
				delete nextNode;
				nextNode = new node;

				nextNode->setPosition(currentPos);
				if (isLeft) nextNode->connections[currentDir.getPrevious()] = new node;
				if (isRight) nextNode->connections[currentDir.getNext()] = new node;
				if (isForward) nextNode->connections[currentDir] = new node;
				
				nextNode->visited = true;
				head++;
				allNodes[head] = nextNode;
			}
			
			delete currentNode->connections[i];
			currentNode->connections[i] = nextNode;
			currentNode->costs[i] = distanceFromLastNode;
			delete nextNode->connections[currentDir.getOpposite()];
			nextNode->connections[currentDir.getOpposite()] = currentNode;
			nextNode->costs[currentDir.getOpposite()] = distanceFromLastNode;
			
			myNodeStack.push(nextNode, currentDir);
		}
	}
	
	faceDir(currentDir.getOpposite());
	
	play_from_program_space(bloop);
}


int8_t followLine(){
	costCounter = 0;
	
	while(1){
		uint8_t tilesTravelled = followSegment();
		
		costCounter += tilesTravelled;
		
		switch(currentDir){
			case NORTH:
			currentPos.y += tilesTravelled;
			break;
			case SOUTH:
			currentPos.y -= tilesTravelled;
			break;
			case EAST:
			currentPos.x -= tilesTravelled;
			break;
			case WEST:
			currentPos.x += tilesTravelled;
			break;
			default:
			break;
		}
		
		if (isLeft && !isRight && !isForward){
			turn(LEFT);
		} else if(isRight && !isLeft && !isForward){
			turn(RIGHT);
		} else {
			break;
		}
	}
	
	return costCounter;
}

void saveCalibration(){
	int i;
	for(i=0;i<5;i++){
		eeprom_write_word(
		&calibrated_minimum_on[i],
		get_line_sensors_calibrated_minimum_on()[i]
		);

		eeprom_write_word(
		&calibrated_maximum_on[i],
		get_line_sensors_calibrated_maximum_on()[i]
		);
	}
}

void loadCalibration(){
	calibrate_line_sensors(IR_EMITTERS_ON); // need to do this to allocate the arrays
	int i;
	for(i=0;i<5;i++){
		get_line_sensors_calibrated_minimum_on()[i] =
		eeprom_read_word(&calibrated_minimum_on[i]);

		get_line_sensors_calibrated_maximum_on()[i] =
		eeprom_read_word(&calibrated_maximum_on[i]);
	}
}

void turn(Turn t){
	unsigned int sensors[5];
	unsigned int turnCalibration = 500;
	
	switch(t){
		case LEFT:
		set_motors(-40, 40);
		delay_ms(100);
		read_line(sensors,IR_EMITTERS_ON);
		while (sensors[2] < turnCalibration){
			read_line(sensors,IR_EMITTERS_ON);
		}
		currentDir.previous();
		break;
		case RIGHT:
		set_motors(40, -40);
		delay_ms(100);
		read_line(sensors,IR_EMITTERS_ON);
		while (sensors[2] < turnCalibration){
			read_line(sensors,IR_EMITTERS_ON);
		}
		currentDir.next();
		break;
		case BACK:
		set_motors(40, -40);
		delay_ms(700);
		read_line(sensors,IR_EMITTERS_ON);
		while (sensors[2] < turnCalibration){
			read_line(sensors,IR_EMITTERS_ON);
		}
		stop();
		currentDir.next();
		currentDir.next();
		break;
		default:
		break;
	}
	stop();
}

void stop(){
	set_motors(0, 0);
}

int8_t followSegment(){
	unsigned long startTime = millis();
	
	int last_proportional = 0;
	long integral=0;
	
	unsigned int threshhold = 350;
	
	unsigned int sensors[5];
	
	while(1){
		unsigned int position = read_line(sensors,IR_EMITTERS_ON);
		
		int proportional = ((int)position) - 1900;
		
		int derivative = proportional - last_proportional;
		integral += proportional;
		
		last_proportional = proportional;
		
		int power_difference = proportional/20 + integral/10000 + derivative*3/2;

		const int max = speed
		if(power_difference > max) {
			power_difference = max;
		}
		if(power_difference < -max) {
			power_difference = -max;
		}
		
		if(power_difference < 0) {
			set_motors(max+power_difference,max);
			} else {
			set_motors(max,max-power_difference);
		}
		
		if(sensors[1] < threshhold && sensors[2] < threshhold && sensors[3] < threshhold){
			// There is no line visible ahead, and we didn't see any
			// intersection.  Must be a dead end.
			
			isLeft = false;
			isRight = false;
			isForward = false;
			
			set_motors(40, 40);
			delay_ms(40);
			
			stop();
			
			break;
		} else if(sensors[0] > threshhold || sensors[4] > threshhold) {
			// Found an intersection
			
			// slow down and position sensors fully over the line ahead
			set_motors(40, 40);
			delay_ms(50);
			
			// take a second reading at the new location and determine the presence of connections
			read_line(sensors,IR_EMITTERS_ON);
			isLeft = (sensors[0] > 500);
			isRight = (sensors[4] > 500);

			// to detect a forward connection, drive the Micromouse forward slightly to avoid incorrect assumption
			//  that a corner is a forward connection
			set_motors(40, 40);
			delay_ms(200);
			stop();
			
			read_line(sensors,IR_EMITTERS_ON);
			
			if (sensors[0] > threshhold && sensors[1] > threshhold && sensors[2] > threshhold && sensors[3] > threshhold && sensors[4] > threshhold){
				play_frequency(2500, 100, 10);
				
				foundFinish = true;
				isLeft = false;
				isRight = false;
				isForward = false;
				break;
			} else {
				foundFinish = false;
			}
			
			isForward = (sensors[1] > threshhold || sensors[2] > threshhold || sensors[3] > threshhold);
			
			break;
		}
	}
	
	unsigned long totalTime = millis() - startTime;
	
	if (totalTime >= 50 && totalTime <= 850){
		return 1;
	} else if(totalTime > 850 && totalTime <= 1300){
		return 2;
	} else if(totalTime > 1300 && totalTime <= 1850){
		return 3;
	} else if(totalTime > 1850 && totalTime <= 2700){
		return 4;
	} else {
		return 0;
	}
}