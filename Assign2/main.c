#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define NUM_NAMES 10
#define CON_MIN 3
#define CON_MAX 6
#define NUM_ROOMS 7

//7  Rooms for our BattleStar Galactica Themed Adventure Game
char* Room_Name[NUM_NAMES] = {
	"TheBridge",
	"TheLandingBay",
	"TheLifeStation",
	"TheBrig",
	"OfficersClub",
	"CouncilChamber",
	"Airlocks",
	"ViperCockpit",
	"BaltarsLab",
	"SaulTighsQuarters"
};

enum Room_Type {
	START_ROOM,
	MID,
	END_ROOM
};

struct Room {
	const char* name;
	struct Room* totConnectedRooms[6];
	int numConnects;
	int totNumConnects;
	enum Room_Type type;
};



//Helper function return a random number between 7
int randRoom() {
	return rand() % NUM_ROOMS;
}

char directory[25];
struct Room totRooms[NUM_ROOMS];

//Name and initalize Rooms
void nameRooms() {
	int numRoomsUsed[NUM_ROOMS];
	memset(&numRoomsUsed, 0, NUM_ROOMS * sizeof(int));
	int i;
	for (i = 0; i < NUM_ROOMS; i++) {
		totRooms[i].numConnects = 0;
		int randConnect = CON_MIN + (rand() % (CON_MAX - CON_MIN +1));
		totRooms[i].totNumConnects = randConnect;
		while(1){
			int roomNum = randRoom();
			if (numRoomsUsed[roomNum] == 0) {
				numRoomsUsed[roomNum] = 1;
				totRooms[i].name = Room_Name[roomNum];
				break;
			}
		};
		totRooms[i].type = MID;
	}
}


//list connected rooms
void printConnections(int roomNum) {
	int i;
	for (i = 0; i < totRooms[roomNum].numConnects; i++) {
		printf("Connected to: %s \n", totRooms[roomNum].totConnectedRooms[i]->name);
	}
}
//For Debug purposes
void printRooms() {
	int i;
	int j;
	for (i = 0; i < NUM_ROOMS; i++) {
		printf("RoomName is %s, Number of Connections is %d, Total Connections is %d , Type is: %d \n", totRooms[i].name, totRooms[i].numConnects, totRooms[i].totNumConnects, totRooms[i].type);
		printConnections(i);
	}
}

void printSingleRoom(int n){
    printf("____ROOM CONTENTS_____\n");
    printf("RoomName is %s, Number of Connections is %d, Total Connections is %d , Type is: %d \n", totRooms[n].name, totRooms[n].numConnects, totRooms[n].totNumConnects, totRooms[n].type);
    printConnections(n);

}



int checkConnect(int roomNum, int roomCon) {
	int i;
	int connectResult = 0;
	if (roomNum == roomCon) {
		connectResult = 1;
	}
	for (i = 0; i < totRooms[roomNum].totNumConnects; i++) {
		if (totRooms[roomNum].totConnectedRooms[i] == &totRooms[roomCon] && totRooms[roomNum].totConnectedRooms[i] != NULL) { //make sure the connection is filled
			connectResult = 1;
		}
	}
	return connectResult;
}

//Check if two rooms are connected 1 if true and successful connect, 0 if false and no connect
int tryConnect(int roomNum, int roomCon) {
	struct Room *roomA = &totRooms[roomNum];
	struct Room *roomB = &totRooms[roomCon];
	int connectResult = 1;
	if (roomA->numConnects == CON_MAX || roomA->numConnects == roomA->totNumConnects) {
		connectResult = 0;
		return connectResult;
	}
	else if (checkConnect(roomNum, roomCon) == 1) {
		connectResult = 0;
	}
	else if (roomA->numConnects == CON_MAX || roomB->numConnects == CON_MAX) {
		connectResult = 0;
	}
	else if (roomA != NULL && roomB != NULL) {
		//connect the rooms
		roomA->totConnectedRooms[roomA->numConnects] = roomB;
		roomB->totConnectedRooms[roomB->numConnects] = roomA;
		roomA->numConnects++;
		roomB->numConnects++;
		connectResult = 1;
	}
	return connectResult;
}

//Set connections to Room structs to create the maze
void connectRooms() {
	int i;
	int j;
	//nested loop outer: for 7 rooms inner: for each rooms connections (3 to 6)
	for (i = 0; i < NUM_ROOMS; i++) {
		for (j = 0; j < totRooms[i].totNumConnects ; j++) {
			int randConnect = randRoom();
			//loop for each and try to connect
			while (tryConnect(i, randConnect) == 1)
			{
				randConnect = randRoom();
			}
		}
	}
}

void setMazeGoals() {
	totRooms[0].type = START_ROOM;
	totRooms[NUM_ROOMS - 1].type = END_ROOM;
}



void buildRooms() {
	//create rooms with random names
	nameRooms();
	//connect the maze randomly
	connectRooms();
	//set start and end of maze
	setMazeGoals();
}

void createRoomDir() {
	int Id = getpid();
	char processId[10];
	sprintf(processId, "%d", Id);
	strcpy(directory,"./struthj.rooms.");
	strncat(directory, processId, 50);
	//file permissions, create dir and cd in
	mkdir(directory, 0777);
	chdir(directory);
}

//Save the created rooms to text files
void roomsToFile() {
    int i;
    int j;
    for (i = 0; i < NUM_ROOMS; i++) {
        FILE *myfile = fopen(totRooms[i].name, "w");
        fprintf(myfile, "ROOM NAME: %s\n", totRooms[i].name);
        for (j = 0; j < totRooms[i].numConnects; j++) {
            fprintf(myfile, "CONNECTION %d: %s\n", j + 1, totRooms[j].name);
        }
        if(totRooms[i].type == 0){
            fprintf(myfile, "ROOM TYPE: START_ROOM");
        }
        if(totRooms[i].type == 1){
            fprintf(myfile, "ROOM TYPE: MID_ROOM");
        }
        if(totRooms[i].type == 2){
            fprintf(myfile, "ROOM TYPE: END_ROOM");
        }
        fclose(myfile);
    }
        //cd ..
        chdir("..");
}

int main() {
	buildRooms();
	printRooms();
	createRoomDir();
	roomsToFile();
	printf("FINALROOMS\n");
    printRooms();
	return 0;
}
