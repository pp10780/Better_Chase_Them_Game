#include "utils_server.h"
#include <ncurses.h>

/******************************************************************************
 * broadcast()
 *
 * Arguments: 
 * Returns:
 *
 * Description: Sends field status update to all connected users
 *****************************************************************************/
void broadcast()
{
	int err;
	message_s2c msg_send;
	msg_send.type = Field_status;
	msg_send.field_status = field_status;
	for(int i = 0; i < N_Max_Players; i++)
	{	
		if (field_status.user[i].id != '-' && field_status.user[i].hp > 0)
		{
			err = send(field_status.user[i].fd, &msg_send,  sizeof(msg_send), MSG_NOSIGNAL);
			if(err == -1 || err == 0)
			{
				continue;
			}
		}
	}
}

/******************************************************************************
 * random_key()
 *
 * Arguments: idx - index of the bot in the struct
 * Returns: A random directional key
 *
 * Description: Generates a random key for the movement of the bot 
 *****************************************************************************/
int random_key(int idx)
{
    srand(time(NULL) + idx);
    int keys[4] = {KEY_DOWN, KEY_RIGHT, KEY_UP, KEY_LEFT};

    return keys[random()%4];   
}

/******************************************************************************
 * generate_valid_pos()
 *
 * Arguments: pos - array that has the x and y coordinates of player/bot/prize
 * Returns:
 *
 * Description: Indicates an open position in the map
 *****************************************************************************/
void generate_valid_pos(int* pos){
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
 * Arguments: index - index of the intended user on the users array
 * Returns:
 *
 * Description: Removes the user after it dies or disconnects
 *****************************************************************************/
void clear_user(int index)
{
	map[field_status.user[index].pos[0]*WINDOW_SIZE + field_status.user[index].pos[1]].occ_status = -1;
	field_status.user[index].id = '-';
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
 * Arguments: 
 * Returns:
 *
 * Description: Hides the indexes of the clients before sending the struct to
                the player, to avoid cheating
 *****************************************************************************/
void index_mask(){
	for(int i = 0; i < N_Max_Players - n_bots - N_Max_Prizes; i++)
	{
		field_status.user[i].idx = -1;
		if(i<10)
			field_status.bot[i].idx = -1;
	}
}