/*
	Program 1: Wumpus, version 1 (fixed size array)
	CS 211, UIC, Fall 2022
	System: Replit
	Author: Dale Reed and Ellen Kidane

	Hunt the Wumpus is a classic text-based adventure game by Gregory Yob from 1972.
	The Wumpus lives in a completely dark cave of 20 rooms.  Each room has 3 tunnels leading to other rooms.
	This program implements this game, without superbats or arrows, using a fixed size array.
*/

#include <stdio.h>
#include <stdbool.h>	// for the bool type in C
#include <stdlib.h>		// for srand
#include <ctype.h>		// for toupper()

// global constants
#define MAX_LINE_LENGTH 81
#define NUMBER_OF_ROOMS 20

// Room connection values are set as global constants, and don't need to be passed.
// The rooms array is indexed from 0..22, but we don't use the 0th entry and 
// only use 1..20.  This approach helps prevent off-by-one indexing errors.
const int Rooms[ 21][ 3] = {
      // Room 0 is unused, to help avoid off-by-one indexing errors.
                            {0,0,0},
      //                    Room 1     Room 2      Room 3      Room 4      Room 5
      /* Rooms 1-5   */     {2,5,8},  {1,3,10},   {2,4,12},   {3,5,14},    {1,4,6},
      //                    Room 6     Room 7      Room 8      Room 9     Room 10
      /* Rooms 6-10  */    {5,7,15},  {6,8,17},    {1,7,9},  {8,10,18},   {2,9,11},
      //                   Room 11    Room 12     Room 13     Room 14     Room 15
      /* Rooms 11-15 */  {10,12,19}, {3,11,13}, {12,14,20},  {4,13,15},  {6,14,16},
      //                   Room 16    Room 17     Room 18     Room 19     Room 20
      /* Rooms 16-20 */  {15,17,20}, {7,16,18},  {9,17,19}, {11,18,20}, {13,16,19},
};

// Used to more conveniently pass all game information between functions.
struct GameInfo {
	int moveNumber;  // Counts up from 1, incrementing for each move
	int personRoom;  // Room 1..20 the person currently is in
	int wumpusRoom;  // Room 1..20 the Wumpus is in
	int pitRoom1;    // Room 1..20 the first pit is in
	int pitRoom2;    // Room 1..20 the second pit is in
};

// Function prototype needed to allow calls in any order between
//   functions checkForHazards() and inRoomWithBats()
void checkForHazards(struct GameInfo *theGameInfo, bool *personIsAlive);

//--------------------------------------------------------------------------------
void displayCave()
{
    printf( "\n"
			"       ______18______             \n"
    		"      /      |       \\           \n"
    		"     /      _9__      \\          \n"
    		"    /      /    \\      \\        \n"
    		"   /      /      \\      \\       \n"
    		"  17     8        10     19       \n"
    		"  | \\   / \\      /  \\   / |    \n"
    		"  |  \\ /   \\    /    \\ /  |    \n"
    		"  |   7     1---2     11  |       \n"
    		"  |   |    /     \\    |   |      \n"
    		"  |   6----5     3---12   |       \n"
    		"  |   |     \\   /     |   |      \n"
    		"  |   \\       4      /    |      \n"
    		"  |    \\      |     /     |      \n"
    		"  \\     15---14---13     /       \n"
    		"   \\   /            \\   /       \n"
    		"    \\ /              \\ /        \n"
    		"    16---------------20           \n"
    		"\n");
}


//--------------------------------------------------------------------------------
void displayInstructions()
{
    printf( "Hunt the Wumpus:                                             \n"
    		"The Wumpus lives in a completely dark cave of 20 rooms. Each \n"
    		"room has 3 tunnels leading to other rooms.                   \n"
    		"                                                             \n"
    		"Hazards:                                                     \n"
    		"1. Two rooms have bottomless pits in them.  If you go there you fall and die.   \n"
    		"2. The Wumpus is not bothered by the pits, as he has sucker feet. Usually he is \n"
			"   asleep. He will wake up if you enter his room. When you move into the Wumpus'\n"
			"   room, then he wakes and moves if he is in an odd-numbered room, but stays    \n"
			"   still otherwise.  After that, if he is in your room, he snaps your neck and  \n"
			"   you die!                                                                     \n"
    		"                                                                                \n"
    		"Moves:                                                                          \n"
    		"On each move you can do the following, where input can be upper or lower-case:  \n"
    		"1. Move into an adjacent room.  To move enter 'M' followed by a space and       \n"
    		"   then a room number.                                                          \n"
    		"2. Enter 'R' to reset the person and hazard locations, useful for testing.      \n"
    		"3. Enter 'C' to cheat and display current board positions.                      \n"
    		"4. Enter 'D' to display this set of instructions.                               \n"
    		"5. Enter 'P' to print the maze room layout.                                     \n"
            "6. Enter 'G' to guess which room Wumpus is in, to win or lose the game!         \n"
    		"7. Enter 'X' to exit the game.                                                  \n"
    		"                                                                                \n"
    		"Good luck!                                                                      \n"
    		" \n\n");
}//end displayInstructions()


//--------------------------------------------------------------------------------
// Return true if randomValue is already in array
int alreadyFound(int randomValue,      // New number we're checking
                 int randomNumbers[],  // Set of numbers previously found
                 int limit)            // How many numbers previously found
{
    int returnValue = false;
    
    // compare random value against all previously found values
    for( int i = 0; i<limit; i++) {
        if( randomValue == randomNumbers[i]) {
            returnValue = true;   // It is already there
            break;
        }
    }
    
    return returnValue;
} 


//--------------------------------------------------------------------------------
// Fill this array with unique random integers 1..20
void setUniqueValues(int randomNumbers[],   // Array of random numbers
                     int size)              // Size of random numbers array
{
    int randomValue = -1;
    
    for( int i = 0; i<size; i++) {
        do {
            randomValue = rand() % NUMBER_OF_ROOMS + 1;   // random number 1..20
        } while (alreadyFound(randomValue, randomNumbers, i));
        randomNumbers[i] = randomValue;
    }
} 


//--------------------------------------------------------------------------------
// Set the Wumpus, player, bats and pits in distinct random rooms
void initializeGame(struct GameInfo *gameInfo)   // All game settings variables
{
    // Array of 6 unique values 1..20, to be used in initializing cave hazards locations
    int randomNumbers[6];
    
    // Initialize cave room connections
    //       ______18______
    //      /      |       \
    //     /      _9__      \
    //    /      /    \      \
    //   /      /      \      \
    //  17     8        10    19
    // |  \   / \      /  \   / |
    // |   \ /   \    /    \ /  |
    // |    7     1---2     11  |
    // |    |    /     \    |   |
    // |    6----5     3---12   |
    // |    |     \   /     |   |
    // |    \       4      /    |
    // |     \      |     /     |
    //  \     15---14---13     /
    //   \   /            \   /
    //    \ /              \ /
    //    16---------------20
    
      
    // Select some unique random values 1..20 to be used for 2 bats rooms, 2
    // pits rooms, Wumpus room, and initial player room
    setUniqueValues(randomNumbers, 4);
    // Use the unique random numbers to set initial locations of hazards, which
    //    should be non-overlapping.
    gameInfo->pitRoom1 = randomNumbers[0];   
    gameInfo->pitRoom2 = randomNumbers[1];
    gameInfo->wumpusRoom = randomNumbers[2];
    gameInfo->personRoom = randomNumbers[3];
    
    gameInfo->moveNumber = 1;
}// end initializeBoard(...)


//--------------------------------------------------------------------------------
// Returns true if nextRoom is adjacent to current room, else returns false.
int roomIsAdjacent( int tunnels[ 3],     // Array of adjacent tunnels
                    int nextRoom)        // Desired room to move to
{
    return( tunnels[0] == nextRoom ||
            tunnels[1] == nextRoom ||
            tunnels[2] == nextRoom
          );
}


//--------------------------------------------------------------------------------
// Display where everything is on the board.
void displayCheatInfo(struct GameInfo gameInfo)
{
    printf( "Cheating! Game elements are in the following rooms: \n"
    		"Player Wumpus Pit1 Pit2  \n"
    		"%4d %7d %5d %5d \n\n", 
			gameInfo.personRoom,
			gameInfo.wumpusRoom,
			gameInfo.pitRoom1,
			gameInfo.pitRoom2
		);
}// end displayCheatInfo(...)


//--------------------------------------------------------------------------------
// Display room number and hazards detected
void displayRoomInfo( struct GameInfo gameInfo)
{
    // Retrieve player's current room number and display it
    int currentRoom = gameInfo.personRoom;
    printf("You are in room %d. ", currentRoom);
    
    // Retrieve index values of all 3 adjacent rooms
    int room1 = Rooms[currentRoom][0];
    int room2 = Rooms[currentRoom][1];
    int room3 = Rooms[currentRoom][2];
    
    // Display hazard detection message if Wumpus is in an adjacent room
    int wumpusRoom = gameInfo.wumpusRoom;
    if( room1 == wumpusRoom || room2 == wumpusRoom || room3 == wumpusRoom) {
        printf("You smell a stench. ");
    }
    
    // Display hazard detection message if a pit is in an adjacent room
    int pit1Room = gameInfo.pitRoom1;
    int pit2Room = gameInfo.pitRoom2;
    if( room1 == pit1Room || room1 == pit2Room ||
        room2 == pit1Room || room2 == pit2Room ||
        room3 == pit1Room || room3 == pit2Room
      ) {
        printf("You feel a draft. ");
    }
    
    printf("\n\n");
}//end displayRoomInfo(...)


//--------------------------------------------------------------------------------
// If the player just moved into a room with a pit, the person dies.
// If the person just moved into the Wumpus room, then if the room number is odd
// the Wumpus moves to a random adjacent room.
void checkForHazards(
         struct GameInfo *gameInfo, // Hazards location and game info
         bool *personIsAlive)       // Person is alive, but could die depending on the hazards
{
    // retrieve the room the person is in
    int personRoom = gameInfo->personRoom;
    
    // Check whether there is a pit
    if( personRoom == gameInfo->pitRoom1 || personRoom == gameInfo->pitRoom2) {
        // Person falls into pit
        printf("Aaaaaaaaahhhhhh....   \n");
        printf("    You fall into a pit and die. \n");
        *personIsAlive = false;
        return;
    }
    
    // Check for the Wumpus
    if( personRoom == gameInfo->wumpusRoom) {
        // To make it easier to test, in this version of the program the Wumpus always
        // moves if it is currently in an odd-numbered room, and it moves into the
        // lowest-numbered adjacent room.  In the version that is more fun to play
        // (but harder to test), the Wumpus has a 75% chance of moving, and a 25%
        // chance of staying and killing you.  The "more fun" code is commented out below.
        
        // Wumpus is there. 75% change of Wumpus moving, 25% chance of it killing you
        // Generate a random number 1..100
        // if(  (rand() % 100) < 75) {
        if( gameInfo->wumpusRoom %2 == 1) {
            // You got lucky and the Wumpus moves away
            printf( "You hear a slithering sound, as the Wumpus slips away. \n"
					"Whew, that was close! \n");
            // Choose a random adjacent room for the Wumpus to go into
            // gameInfo.wumpusRoom = rooms[ personRoom][ rand() % 3];
            gameInfo->wumpusRoom = Rooms[ personRoom][ 0];  // Choose the lowest-numbered adjacent room
        }
        else {
            // Wumpus kills you
            printf(	"You briefly feel a slimy tentacled arm as your neck is snapped. \n"
					"It is over.\n");
            *personIsAlive = false;
            return;
        }
    }

}//end checkForHazards(...)


//--------------------------------------------------------------------------------
// Prompt for and reset the positions of the game hazards and the person's location,
// useful for testing.  No error checking is done on these values.
void resetPositions(struct GameInfo *theGameInfo)
{
    printf("Enter the room locations (1..20) for player, wumpus, pit1, and pit2: \n");
	// In the scanf below note that we put the space at the beginning of the scanf so  
    // that any newline left over from a previous input is not read in and used as 
	// the next move. Another option is having getchar() after the scanf() statement.
	scanf(" %d %d %d %d", &theGameInfo->personRoom, &theGameInfo->wumpusRoom, 
                          &theGameInfo->pitRoom1, &theGameInfo->pitRoom2);
    printf("\n");
}


//--------------------------------------------------------------------------------
int main(void) {
    struct GameInfo gameInfo;   // Used to more easily pass game info variables around
    bool personIsAlive = true;  // Used in checking for end of game
    bool wumpusIsAlive = true;  // Used in checking for end of game
    char typeOfMove;            // Used to handle user input letter
    int nextRoom;               // User input of destination room number, used on a 'M' type move
    
    // Seed the random number generator.  Change seed to time(0) to change output each time.
    // srand(time(0));
    srand(1);
    
    // Set random initial values for person, Wumpus, bats and pits
    initializeGame( &gameInfo);

	// Main playing loop.  Break when player dies, or player kills Wumpus
    while (personIsAlive && wumpusIsAlive) {
            
		// Display current room information: Room number, hazards detected
		displayRoomInfo( gameInfo);
		
		// Prompt for and handle move
		printf("%d. Enter your move (or 'D' for directions): ", gameInfo.moveNumber);
        // Note the extra space in the scanf below between the opening quote " and the %c.  
        //    This skips leading white space in the input so that the newline left over from
        //    a previous move is not read in and used as the current move.  An alternative 
        //    is using getchar() after the scanf() statement.
		scanf(" %c", &typeOfMove);	
									
		typeOfMove = toupper(typeOfMove);  // Make uppercase to facilitate checking
		
		// Check all types of user input and handle them.  This uses if-else-if code
		//   rather than switch-case, so that we can take advantage of break and continue.
		if( typeOfMove == 'D') {
			displayCave();
			displayInstructions();
			continue;       // Loop back up to reprompt for the same move
		}
		else if( typeOfMove == 'P') {
			// To assist with play, display the cave layout
			displayCave();
			continue;       // Loop back up to reprompt for the same move
		}
		else if( typeOfMove == 'M') {
			// Move to an adjacent room,
            // Note the extra space in the scanf below between the opening quote " and the %c.  
            //    This skips leading white space in the input so that the newline left over from
            //    a previous move is not read in and used as the current move.  An alternative 
            //    is using getchar() after the scanf() statement.
			scanf(" %d", &nextRoom);
			
			if( roomIsAdjacent( Rooms[ gameInfo.personRoom], nextRoom)) {
				gameInfo.personRoom = nextRoom;        // move to a new room
				// Check if pit or wumpus is present in this new room
				checkForHazards(&gameInfo, &personIsAlive);
			}
			else {
				printf("Invalid move.  Please retry. \n");
				continue;        // Doesn't count as a move, so retry same move.
			}
		}
		else if( typeOfMove == 'C') {
			// Display Cheat information
			displayCheatInfo(gameInfo);
			continue;        // Doesn't count as a move, so retry same move.
		}
		else if( typeOfMove == 'R') {
			// Specify resetting the hazards and person positions, useful for testing
			resetPositions( &gameInfo);
			continue;        // Doesn't count as a move, so retry same move.
		}
        else if( typeOfMove == 'G') {
			// Prompt for room number guess and see if user has found the Wumpus
            int wumpusRoomGuess = 0;
			printf("Enter room (1..20) you think Wumpus is in: ");
            scanf(" %d", &wumpusRoomGuess);
            // See if userguess was correct, for a win or loss.
            if( gameInfo.wumpusRoom == wumpusRoomGuess) {
                printf("You won!\n");
            }
            else{ 
                printf("You lost.\n");
            }
			break;
		}
		else if( typeOfMove == 'X') {
			// Exit program
			personIsAlive = false;   // Indicate person is dead as a way to exit playing loop
			break;
		}

		// Increment the move number
		gameInfo.moveNumber = gameInfo.moveNumber + 1;
        
    }//end while(personIsAlive && wumpusIsAlive)
    
    printf("\nExiting Program ...\n");
	
	return 0;
}