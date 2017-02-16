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

//check using stat for most recent rooms dir
void getRecentDir(){
    DIR *mydir;
    char directory[50];
    strcpy(directory,"./struthj.rooms."); //..PID
    int fileTime = 0;
    struct stat recentFile;
    stat(directory, &recentFile);
    if(recentFile.st_mtime > fileTime){
        fileTime = recentFile.st_mtime;
    }
}

int main(){



    return 0;
}
