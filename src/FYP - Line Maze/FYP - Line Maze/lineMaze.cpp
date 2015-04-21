#include <pololu/Pololu3pi.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

void* operator new(size_t objsize) {
	return malloc(objsize);
}

void operator delete(void* obj) {
	free(obj);
}

enum Turn { LEFT, RIGHT, BACK };
enum Direction { SOUTH, WEST, NORTH, EAST };

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
};

struct position {
	int8_t x, y;
	bool operator == (const position &RHS) {
		return (this->x == RHS.x && this->y == RHS.y);
	}
	bool operator != (const position &RHS) {
		return (this->x != RHS.x || this->y != RHS.y);
	}
};

struct node {
	position pos;
	bool visited;
	node* connections[4]; // 0 = South (where mouse came from), 1 = West, 2 = North, 3 = East
	node* parent;
	float f, g, h;

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
	
	void addConnection(Direction d, node* n){
		switch(d){
			case SOUTH:
			this->connections[0] = n;
			break;
			case WEST:
			this->connections[1] = n;
			break;
			case NORTH:
			this->connections[2] = n;
			break;
			case EAST:
			this->connections[3] = n;
			break;
			default:
			break;
		}
	}
	
	node* getConnection(Direction d){
		switch(d){
			case SOUTH:
			return connections[0];
			break;
			case WEST:
			return connections[1];
			break;
			case NORTH:
			return connections[2];
			break;
			case EAST:
			return connections[3];
			break;
			default:
			break;
		}
	}
};

unsigned int calibrated_maximum_on[5] EEMEM;
unsigned int calibrated_minimum_on[5] EEMEM;

bool isForward, isLeft, isRight;

cDirection currentDir;
position currentPos;

void saveCalibration();
void loadCalibration();
void turn(Turn t);
void stop();
void mapMaze();
void followLine();
void load_custom_characters();
void display_readings(const unsigned int *calibrated_values);
int8_t followSegment();

const char levels[] PROGMEM = {
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};

void load_custom_characters()
{
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
	unsigned char i;
	for(i=0;i<5;i++) {
		const char display_characters[10] = {' ',0,0,1,2,3,4,5,6,255};
		char c = display_characters[calibrated_values[i]/101];
		print_character(c);
	}
}

int randomBetween(int a, int b){
	return rand() % ( b - a + 1) + a;
}

int main(){
	unsigned int sensors[5];
	pololu_3pi_init(2000);
	load_custom_characters();
	loadCalibration();
	play_frequency(1000, 200, 10);
	
	clear();
	print("Press B");
	
	while(1){
		if (button_is_pressed(BUTTON_A)){
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
			} else if(button_is_pressed(BUTTON_B)){
			mapMaze();
			} else if(button_is_pressed(BUTTON_C)){
			while(1){
				unsigned int position = read_line(sensors, IR_EMITTERS_ON);
				clear();
				print_long(position);
				lcd_goto_xy(0,1);
				display_readings(sensors);
				delay_ms(100);
			}
		}
	}
	
	return 0;
}

int8_t head = 0;
node* nodes[40];

node* getNodeByPosition(position p){
	for (int i=0; i<head; i++){
		if (nodes[i]->pos == p){
			return nodes[i];
		}
	}
	return NULL;
}

void faceDir(Direction d){
	if (currentDir.getNext() == d){
		turn(RIGHT);
		} else if(currentDir.getPrevious() == d){
		turn(LEFT);
		} else if(currentDir.getOpposite() == d){
		turn(BACK);
		} else {
		clear();
		print("ERR");
	}
}

void mapMaze2(){
	
}

void mapMaze(){
	currentDir.dir = NORTH;
	currentPos.x = 0;
	currentPos.y = 0;
	
	nodes[head] = new node;
	nodes[head]->setPosition(currentPos);
	nodes[head]->addConnection(NORTH, new node);
	nodes[head]->visited = true;
	
	node* lastNode = nodes[head];
	
	int i;
	
	while(1){
		followLine();
		
		node* thisNode = getNodeByPosition(currentPos);
		if (thisNode != NULL){
			delete lastNode->connections[i];
			lastNode->connections[i] = thisNode;
		} else {
			thisNode = new node;
			thisNode->setPosition(currentPos);
			thisNode->addConnection(currentDir.getOpposite(), new node);
			if (isLeft){
				thisNode->addConnection(currentDir.getPrevious(), new node);
			}
			if (isRight){
				thisNode->addConnection(currentDir.getNext(), new node);
			}
			if (isForward){
				thisNode->addConnection(currentDir, new node);
			}
		}
		
		thisNode->visited = true;
		
		nodes[head] = thisNode;
		
		head++;
		
		nodes[head]->visited = true;

		node* nextNode = NULL;
		
		for (i=0; i<4; i++){
			if (nodes[head]->connections[i] != NULL){
				if (nodes[head]->connections[i]->visited == false){
					nextNode = nodes[head]->connections[i];
					break;
				}
			}
		}
		
		if (nextNode != NULL){ // if there is an unvisited node attached to this node
			
		} else { // if not, then find the next node with unvisited connections and travel to it
			
		}
		
		if (nextNode != NULL){
			switch(i){
				case 0:
				faceDir(SOUTH);
				break;
				case 1:
				faceDir(EAST);
				break;
				case 2:
				faceDir(WEST);
				break;
				case 3:
				faceDir(NORTH);
				break;
				default:
				break;
			}
		}
		
		followLine();
	}
}

void followLine(){
	while(1){
		uint8_t tilesTravelled = followSegment();
		
		switch(currentDir){
			case NORTH:
			currentPos.y += tilesTravelled;
			break;
			case SOUTH:
			currentPos.y -= tilesTravelled;
			break;
			case EAST:
			currentPos.x += tilesTravelled;
			break;
			case WEST:
			currentPos.x -= tilesTravelled;
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
	read_line(sensors,IR_EMITTERS_ON);
	
	switch(t){
		case LEFT:
		play_frequency(500, 200, 10);
		set_motors(-40, 40);
		while (sensors[2] < 950){
			read_line(sensors,IR_EMITTERS_ON);
		}
		currentDir.previous();
		break;
		case RIGHT:
		play_frequency(1000, 200, 10);
		set_motors(40, -40);
		while (sensors[2] < 950){
			read_line(sensors,IR_EMITTERS_ON);
		}
		currentDir.next();
		break;
		case BACK:
		play_frequency(1500, 200, 10);
		set_motors(40, -40);
		delay_ms(700);
		while (sensors[2] < 950){
			read_line(sensors,IR_EMITTERS_ON);
		}
		stop();
		currentDir.next();
		currentDir.next();
		break;
		default:
		clear();
		print("No turn");
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
	
	while(1){
		// Normally, we will be following a line.  The code below is
		// similar to the 3pi-linefollower-pid example, but the maximum
		// speed is turned down to 60 for reliability.
		
		// Get the position of the line.
		unsigned int sensors[5];
		unsigned int position = read_line(sensors,IR_EMITTERS_ON);
		
		// The "proportional" term should be 0 when we are on the line.
		int proportional = ((int)position) - 2000;
		
		// Compute the derivative (change) and integral (sum) of the
		// position.
		int derivative = proportional - last_proportional;
		integral += proportional;
		
		// Remember the last position.
		last_proportional = proportional;
		
		// Compute the difference between the two motor power settings,
		// m1 - m2.  If this is a positive number the robot will turn
		// to the left.  If it is a negative number, the robot will
		// turn to the right, and the magnitude of the number determines
		// the sharpness of the turn.
		int power_difference = proportional/20 + integral/10000 + derivative*3/2;
		
		// Compute the actual motor settings.  We never set either motor
		// to a negative value.
		const int max = 60; // the maximum speed
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
		
		// We use the inner three sensors (1, 2, and 3) for
		// determining whether there is a line straight ahead, and the
		// sensors 0 and 4 for detecting lines going to the left and
		// right.
		
		if(sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100){
			// There is no line visible ahead, and we didn't see any
			// intersection.  Must be a dead end.
			
			isLeft = false;
			isRight = false;
			isForward = false;
			
			stop();
			
			break;
			} else if(sensors[0] > 150 || sensors[4] > 150) {
			// Found an intersection.
			set_motors(40, 40);
			delay_ms(50);
			
			read_line(sensors,IR_EMITTERS_ON);
			isLeft = (sensors[0] > 500);
			isRight = (sensors[4] > 500);

			set_motors(40, 40);
			delay_ms(200);
			stop();
			
			read_line(sensors,IR_EMITTERS_ON);
			isForward = (sensors[1] > 150 || sensors[2] > 150 || sensors[3] > 150);
			
			break;
		}
	}
	
	unsigned long totalTime = millis() - startTime;
	
	if (totalTime >= 50 && totalTime <= 850){
		return 1;
		} else if(totalTime > 850 && totalTime <= 1400){
		return 2;
		} else if(totalTime > 1400 && totalTime <= 2000){
		return 3;
		} else if(totalTime > 2000 && totalTime <= 2600){
		return 4;
		} else {
		return 0;
	}
}