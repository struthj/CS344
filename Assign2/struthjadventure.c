#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

#define NUM_NAMES 10
#define CON_MIN 3
#define CON_MAX 6
#define NUM_ROOMS 7
#define NAME_LENGTH 18

//7  Rooms for our BattleStar Galactica Themed Adventure Game
char* Room_Names[NUM_NAMES] = {
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
	MID_ROOM,
	END_ROOM
};

struct Rooms {
	int name;
	struct Rooms* totConnectedRooms[6];
	int numConnects;
	int totNumConnects;
	enum Room_Type type;
};

pthread_mutex_t tlock;
char roomDir[256]; //ff in hex
struct Rooms maze[NUM_ROOMS];  //our built maze to read in to from files

//helper function to get name given a room index
char *getNamefromRoom(int n){
    return Room_Names[n];
}

//display all possible connections to a room use for debug
void printConnectionsAd(int roomNum) {
	int i;
    printf("POSSIBLE CONNECTIONS: ");
	for (i = 0; i < maze[roomNum].numConnects; i++) {
		printf("%s, ", Room_Names[maze[roomNum].totConnectedRooms[i]->name]);
	}
}

//display all possible connections to a room
void printConnectionsPossible(int roomNum) {
	int i;
	for (i = 0; i < maze[roomNum].numConnects; i++) {
		printf("%s,", Room_Names[maze[roomNum].totConnectedRooms[i]->name]);
	}
}

//given bad input display this to the user
void badInputMessage(){
    printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
}
//For Debug purposes
void printRoomsAd() {
	int i;
	for (i = 0; i < NUM_ROOMS; i++) {
		printf("RoomName is %s, Number of Connections is %d, Total Connections is %d , Type is: %d \n", Room_Names[maze[i].name], maze[i].numConnects, maze[i].totNumConnects, maze[i].type);
		printConnectionsAd(i);
	}
}

//helper function to print one room
void printSingleRoomAd(int n){
    printf("____ROOM CONTENTS_____\n");
    printf("RoomName is %s, Number of Connections is %d, Total Connections is %d , Type is: %d \n", getNamefromRoom(maze[n].name), maze[n].numConnects, maze[n].totNumConnects, maze[n].type);

}

//check using stat for most recent rooms dir
void getRecentDir(){
    DIR *dir;
    char directory[100];
    long int fileTime = 0;
    struct stat *recentFile;
    struct dirent *newDir;
    long int compareTime = 0;
    recentFile = malloc(sizeof(struct stat));
    newDir = malloc(sizeof(struct dirent));
    memset(roomDir,'\0',sizeof(roomDir));
    memset(directory,'\0', sizeof(directory));
    //set us in .
    getcwd(directory, sizeof(directory));
    dir = opendir(directory);
    if(dir != NULL){
        while(newDir = readdir(dir)){
            //find str in dir
            if(strstr(newDir->d_name, "struthj.rooms.") != NULL){ //PID is right after dot
                stat(newDir->d_name, recentFile);
                fileTime = recentFile->st_mtime;
                if(fileTime > compareTime){
                    compareTime = fileTime;
                    strcpy(roomDir,newDir->d_name); //we need most recently modified dir for rooms
                }
            }
        }
    }
}

//get Name of most recent directory for roomfiles
void getName(){
    DIR *mydir;
    struct dirent *curDir; //each room text file in current struthj.rooms.PID dir
    int i = 0;
    int j = 0;
    if((mydir = opendir(roomDir)) != NULL){
        while((curDir = readdir(mydir)) != NULL){
            if(strlen(curDir->d_name) > 2){
                j=  getMatching(curDir->d_name);
                if(j != NULL){
                    maze[i].name = j;
                }
                i++; //increment to next room in maze
            }
        }
    }
}

//return index based on name
int getMatching(char *name){
    int nameMatch = 0;
    int i = 0;
    while(nameMatch != 1){
        if(strcmp(name, Room_Names[i]) == 0){
            nameMatch = 1;
        }
        else{
            nameMatch = 0;
            i++;
        }
    }
    return i;
}

//logic is simpler here since graph was already built in buildrooms program
void conRooms(int roomA, int roomB){
    int numCon = maze[roomA].numConnects;
    int numConB = maze[roomB].numConnects;
    maze[roomA].totConnectedRooms[numCon] = &maze[roomB];
    maze[roomA].numConnects++;
}

//Helper func to return the index of a Room given a room name
int getRoomNum(char *roomName){
    int i = 0;
    int roomNum = 0;
    for(i = 0; i < NUM_ROOMS; i++){
        if(strcmp(Room_Names[maze[i].name], roomName) == 0){
            roomNum = i;
        }
    }
    return roomNum;
}

//helper func to remove whitespace
void RemoveSpaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}

//Helper function to separate RoomName and Label from text file line, using strtok
void eolParse(char *val, char* buf){
    strtok(val, ":"); //strtok from lecture
    strcpy(buf, strtok(NULL,""));
    val[strlen(val) - 1] = '\0'; //c string null terminator
    buf[strlen(buf) - 1] = '\0';
    RemoveSpaces(buf); //trim whitespace
}


//cd up a directory
void backToDir(){
    chdir("..");
}

//read in rooms from textfiles for use in the game
void readRoom(){
    FILE *myfile;
    int i;
    int roomConNum;
    char line[256];
    char buf[256];
    getName();
    chdir(roomDir);
    printf("CWD IS:  %s",getcwd(roomDir, sizeof(roomDir)));
    for(i = 0; i < NUM_ROOMS; i++){
        myfile = fopen(Room_Names[maze[i].name], "r"); //read only, we will modify a buffer
        memset(buf,'\0',sizeof(buf));
        memset(line,'\0',sizeof(line));
        if(myfile == NULL){
            exit(0);
        }
        while(fgets(line, sizeof(line),myfile) != NULL){
            //separate room name and label
            eolParse(line, buf);
            //printf("LINE is: %s \n", line);for use in debug w C strings
            //printf("buf is: %s \n", buf);
            if(strcmp(line, "ROOM TYP") == 0){
                if(strcmp(buf, "START_ROO") == 0){
                   maze[i].type = START_ROOM; //enum val of 0
                }
                else if(strcmp(buf, "END_ROO") == 0){
                   maze[i].type = END_ROOM; //enum val of 2
                }
                else{
                    maze[i].type = MID_ROOM;
                }
            }
            else if(strcmp(line, "CONNECTION ") == 0){ //everything else is labeled for connections (min 3 upto 6)
                roomConNum = getRoomNum(buf);
                conRooms(i, roomConNum);
            }
        }
        fclose(myfile);
    }
    backToDir();
}

//looks for the room swith type START_ROOM and returns index
int getStartRoom(){
    int i;
    for(i = 0; i < NUM_ROOMS; i++){
        if(maze[i].type == 0){
                return i;
        }
    }
}

//create currentTime file for when user enters "Time"
void* writeTime(){
    FILE *myfile = NULL;
    char timeFile[] = "currentTime.txt";
    myfile = fopen(timeFile,"w"); //+w write permission
    time_t curTime;
    struct tm *tTime;
    time(&curTime);
    tTime = localtime(&curTime);
    fprintf(myfile, "%s\n" ,asctime(tTime)); //write time to file
    fclose(myfile);
    return NULL;
}


//read time from file and display to user if tie command entered
void* getTime(){
    pthread_mutex_lock(&tlock); //lock file before reading
    char timeFile[] = "currentTime.txt";
    char time[100];
    memset(time,'\0', sizeof(time));
    FILE *myfile;
    myfile = fopen(timeFile, "r");
    while(fgets(time, sizeof(time), myfile) != NULL){
        printf("\n%s\n", time);
    }
    fclose(myfile); //close and unlock file when we are done

}

//Main loop for the gam handleing input and program flow for user choices
void gameLoop(){
    struct Rooms *myroom = &maze[getStartRoom()];
    struct Rooms **path = malloc(sizeof(struct Rooms*) * NUM_ROOMS);
    int curRoom = 0;
    int totRoom = 0;
    int goodInput = 0;
    char buf[NAME_LENGTH];
    int steps = 0;
    int gameWin = 0;
    int i;
    while(gameWin == 0){
        printf("\nCURRENT LOCATION: %s \n", Room_Names[myroom->name]);
        printConnectionsAd(getRoomNum(Room_Names[myroom->name]));
        printf("\nWHERE TO? >");
        goodInput = 0;
        char *input = fgets(buf, NAME_LENGTH, stdin);
        buf[strlen(input) - 1] = '\0';
        //handle if user want to check time
        if(strcmp(buf, "time") == 0){
            pthread_t timeTh;
            pthread_mutex_lock(&tlock); //lock it
            if(pthread_create(&timeTh, NULL, writeTime, NULL) != 0) //thread to write and get time
            {
                printf("ERROR");
            }
            pthread_mutex_unlock(&tlock);//unlock before next use
            pthread_join(timeTh, NULL);
            getTime();
            //back to loop, entering Time counts as goodInput
            goodInput = 1;
        }
        for(i = 0; i < myroom->numConnects; i++){
            if(strncmp(buf, Room_Names[myroom->totConnectedRooms[i]->name], NAME_LENGTH) == 0){
                myroom = myroom->totConnectedRooms[i];
                //if we've visited too many rooms realloc mem
                if(curRoom >= totRoom){
                    totRoom += NUM_ROOMS;
                    path = realloc(path, totRoom * sizeof(struct Rooms*));
                }
                path[curRoom] = myroom;
                curRoom++;
                steps++;
                goodInput = 1;
                //inc counters
            }
        }
        if(myroom->type == END_ROOM){
            //This is the win condition, notify user
            printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
            for(i = 0; i < curRoom; i++){
                printf("%s\n", Room_Names[path[i]->name]);
            }
            gameWin = 1;
        }
        if(goodInput == 0){
            //if the input is bad display "Huh i dont recognize that etc"
            badInputMessage();
        }
    }
}

int main(){
    //most recent PID
    getRecentDir();
    //read in rooms from textfiles
    readRoom();
    //run main gameLoop
    gameLoop();
    exit(0);
}
