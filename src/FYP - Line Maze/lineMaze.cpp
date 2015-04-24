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

void memTest(){
	int startRam = get_free_ram();
	clear();
	print(" ");
	print_long(startRam - get_free_ram());
	lcd_goto_xy(0, 1);
	
	NodeStack myNodeStack;
	node* a = new node;
	myNodeStack.push(a, NORTH);
	
	print(" ");
	print_long(startRam - get_free_ram());
}

int main(){
	initialise();
	
	while(1){
		//clear();
		//print(" ");
		//print_long(read_battery_millivolts());
		//print("mv");
		//delay_ms(20);
		
		if (button_is_pressed(BUTTON_A)){
			//calibrate();
			memTest();
		} else if(button_is_pressed(BUTTON_B)){
			mapMaze();
		} else if(button_is_pressed(BUTTON_C)){
			unsigned int sensors[5];
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

void travelTo(int8_t x, int8_t y){
	
}

void mapMaze(){
	int startRAM = get_free_ram();
	
	currentDir.dir = NORTH;
	currentPos.x = 0;
	currentPos.y = 0;
	
	//myNodeStack = NodeStack();
	
	node* allNodes[50];
	int head = 0;
	
	node* root = new node;
	root->setPosition(currentPos);
	root->connections[2] = new node;
	root->visited = true;
	
	allNodes[head] = root;
	
	myNodeStack.push(root, NORTH);
	
	int i;
	while(!myNodeStack.isEmpty()){
		node* currentNode = myNodeStack.getTop();
		
		clear();
		print(" ");
		print_long(get_free_ram());
		lcd_goto_xy(0, 1);
		print(" ");
		print_long(startRAM - get_free_ram());
		//delay_ms(1000);
		wait_for_button(BUTTON_B);
		
		for (i=3; i>-1; i--){
			if (currentNode->connections[i] != NULL){
				if (currentNode->connections[i]->visited == false){
					break;
				}
			}
		}
		
		if (i == -1){ // this node has no unvisited connections
			cDirection d;
			d = myNodeStack.pop();
			faceDir(d.getOpposite());
			followLine();
		} else {
			faceDir(i);
			followLine();
			
			node* nextNode = NULL;
			for (int j=0; j<=head; j++){
				if (allNodes[j]->pos == currentPos){
					nextNode = allNodes[j];
					break;
				}
			}
			
			if (nextNode == NULL){ // new node
				play_from_program_space(bloop);
				
				nextNode = new node;
				
				clear();
				print(" ");
				print_long(get_free_ram());
				
				lcd_goto_xy(0, 1);
				
				print(" ");
				print_long(startRAM - get_free_ram());
				
				//delay_ms(1000);
				
				wait_for_button(BUTTON_B);
				
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
			delete nextNode->connections[currentDir.getOpposite()];
			nextNode->connections[currentDir.getOpposite()] = currentNode;
			
			myNodeStack.push(nextNode, currentDir);
		}
	}
	
	faceDir(currentDir.getOpposite());
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
	unsigned int turnCalibration = 600;
	
	switch(t){
		case LEFT:
		set_motors(-40, 40);
		delay_ms(200);
		read_line(sensors,IR_EMITTERS_ON);
		while (sensors[2] < turnCalibration){
			read_line(sensors,IR_EMITTERS_ON);
		}
		currentDir.previous();
		break;
		case RIGHT:
		set_motors(40, -40);
		delay_ms(200);
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
	
	unsigned int sensors[5];
	
	while(1){
		// Normally, we will be following a line.  The code below is
		// similar to the 3pi-linefollower-pid example, but the maximum
		// speed is turned down to 60 for reliability.
		
		// Get the position of the line.
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
		

		if(sensors[1] < 350 && sensors[2] < 350 && sensors[3] < 350){
			// There is no line visible ahead, and we didn't see any
			// intersection.  Must be a dead end.
			
			isLeft = false;
			isRight = false;
			isForward = false;
			
			stop();
			
			break;
		} else if(sensors[0] > 350 || sensors[4] > 350) {		
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
			
			if (sensors[0] > 350 && sensors[1] > 350 && sensors[2] > 350 && sensors[3] > 350 && sensors[4] > 350){
				play_frequency(2000, 100, 10);
				foundFinish = true;
				isLeft = false;
				isRight = false;
				isForward = false;
				break;
			} else {
				foundFinish = false;
			}
			
			isForward = (sensors[1] > 350 || sensors[2] > 350 || sensors[3] > 350);
			
			break;
		}
	}
	
	unsigned long totalTime = millis() - startTime;
	
	//clear();
	//print(" ");
	//print_long(totalTime);
	
	//wait_for_button(BUTTON_B);
	
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