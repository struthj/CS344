#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>


//7  Rooms for our BattleStar Galactica Themed Adventure Game
char* Room_Name[7] = {
        "TheBridge",
        "TheLandingBay",
        "TheLifeStation",
        "TheBrig",
        "OfficersClub",
        "CouncilChamber",
        "Airlocks"
        };

enum Room_Type {
        Start,
        Middle,
        End
};

struct Room {
        const char* name;
        struct Room* totConnect[6];
        int numConnects;
        enum Room_Type type;
};



struct Room totRooms[7];

int main(){
 
 
 return 0;   
}