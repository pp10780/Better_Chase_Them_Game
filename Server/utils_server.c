#include "utils_server.h"
#include <ncurses.h>

/******************************************************************************
 * generate_valid_pos()
 *
 * Arguments: map - array with the info of all map positions
 *			  pos - array that has the x and y coordinates of player/bot/prize
 * Returns:
 *
 * Description: Indicates an open position in the map
 *****************************************************************************/
void generate_valid_pos(position_t* map, int* pos){
	int val_pos = 0;
	srand(time(0));

	do{
		pos[0] = 1 + rand() % (WINDOW_SIZE - 2);
		pos[1] = 1 + rand() % (WINDOW_SIZE - 2);

		if (map[pos[0]*WINDOW_SIZE + pos[1]].occ_status == -1) val_pos = 1;
	} while(!val_pos);

}


/******************************************************************************
 * get_new_pos()
 *
 * Arguments: pos - array that has the x and y coordinates of player/bot/prize
 *			  key - represents the directional key pressed 
 * Returns:
 *
 * Description: Gets new position based on previous position and directional
 *				key pressed by the client
 *****************************************************************************/
void get_new_pos(int* pos, int key)
{
	if (key == KEY_UP){
        if (pos[1] != 1){
            pos[1]--;
        }
    }
    else if (key == KEY_DOWN){
        if (pos[1] != WINDOW_SIZE-2){
            pos[1]++;
        }
	}
    else if (key == KEY_LEFT){
        if (pos[0] != 1){
            pos[0]--;
        }
    }
    else if (key == KEY_RIGHT){
        if (pos[0] != WINDOW_SIZE-2){
            pos[0]++;
		}
    }
	
}

/******************************************************************************
 * clear_user()
 *
 * Arguments: client - struct with all the client's info
 *            map - array with the info of all map positions
 * Returns:
 *
 * Description: Removes the user after it dies or disconnects
 *****************************************************************************/
void clear_user(client_t* user, position_t* map)
{
	map[user->pos[0]*WINDOW_SIZE + user->pos[1]].occ_status = -1;
	user->id = '-';
}

/******************************************************************************
 * check_ID_val()
 *
 * Arguments: client - struct with all the client's info
 * 			  id - char inputted by the user to be its ID
 * 			  
 * Returns: 0 if there is already a player with ID chosen
 * 		    1 if it is an unused ID
 *
 * Description: Checks if the ID chosen by the user is available or not in 
 *              order to avoid multiple players with the same ID
 *****************************************************************************/
int check_ID_val(client_t* client, char id)
{
	for(int i = 0; i < N_Max_Players; i++)
	{
		if(client[i].id == id)
			return 0;
	}
	return 1;
}

/******************************************************************************
 * index_mask()
 *
 * Arguments: field_status - struct with all the info related to the clients 
 * 							 and prizes
 * Returns:
 *
 * Description: Hides the indexes of the clients before sending the struct to
                the player, to avoid cheating
 *****************************************************************************/
void index_mask(field_status_t* field_status){
	for(int i = 0; i < 10; i++)
	{
		field_status->user[i].idx = -1;
		field_status->bot[i].idx = -1;
	}
}