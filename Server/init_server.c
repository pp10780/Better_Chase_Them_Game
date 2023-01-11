#include "init_server.h"

/******************************************************************************
 * init_server()
 *
 * Arguments: field_status - struct with all the info related to the clients 
 * 							 and prizes
 *            map - array with the info of all map positions
 * Returns:
 *
 * Description: Initializes the server
 *****************************************************************************/
void init_server(field_status_t* field_status, position_t* map){
	init_client(field_status->user);
	init_field(map);
	init_prize(field_status->prize);
	init_client(field_status->bot);

	for (int i = 0; i < 5; i++){
		create_prize(map, field_status->prize);
	}
}

/******************************************************************************
 * init_client()
 *
 * Arguments: client - struct with all the client's info
 * Returns:
 *
 * Description: Initializes the client
 *****************************************************************************/
void init_client(client_t* client){

	for (int i = 0; i < 10; i++){
		client[i].id = '-';
		client[i].pos[0] = -1;
		client[i].pos[1] = -1;
		client[i].hp = -1;
		
	}
}

/******************************************************************************
 * init_prize()
 *
 * Arguments: prize - struct with all the prize's info
 * Returns:
 *
 * Description: Initializes the prize
 *****************************************************************************/
void init_prize(prize_t* prize)
{
	for(int i = 0; i < N_Max_Prizes; i++)
	{
		prize[i].value = -1;
	}
}

/******************************************************************************
 * init_field()
 *
 * Arguments: map - array with the info of all map
 * Returns:
 *
 * Description: Initializes the field
 *****************************************************************************/
void init_field(position_t* map){
	for (int i = 0; i < WINDOW_SIZE*WINDOW_SIZE; i++){
		map[i].occ_status = -1;
	}
}

/******************************************************************************
 * create_user()
 *
 * Arguments: user - struct with all the user's info
 *            map - array with the info of all map
 *            id - ID chosen by the new player
 *			  fd - File descriptor associated with new player
 *            
 * Returns: returns the index of the new user in the array of player clients 
 *
 * Description: Receives the new user that entered the game and puts it in the 
 *              map
 *****************************************************************************/
int create_user(client_t* user, position_t* map, char id, int fd){
	int i;
	for(i = 0; i < N_Max_Players; i++){
		if (user[i].id == '-'){
			user[i].fd = fd;
			user[i].id = id;
			user[i].idx = i;
			generate_valid_pos(map, user[i].pos);
			user[i].hp = 10;
			map[user[i].pos[0]*WINDOW_SIZE + user[i].pos[1]].occ_status = 0;
			map[user[i].pos[0]*WINDOW_SIZE + user[i].pos[1]].idx = i;
			break;
		}
	}
	return i;

}

/******************************************************************************
 * create_bots()
 *
 * Arguments: bot - struct with all the bot's info
 * 		      map - array with the info of all map positions
 *            n_bots - number of bots initialized in the bot_client
 * Returns:
 *
 * Description: Creates the bots and puts them on the map
 *****************************************************************************/
void create_bots(client_t* bot, position_t* map, int n_bots){
	for (int i = 0; i < n_bots; i++){
		bot[i].id = '*';
		bot[i].idx = i;
		generate_valid_pos(map, bot[i].pos);
		map[bot[i].pos[0]*WINDOW_SIZE + bot[i].pos[1]].occ_status = 1;
		map[bot[i].pos[0]*WINDOW_SIZE + bot[i].pos[1]].idx = i;
	}
	
}

/******************************************************************************
 * create_prize()
 *
 * Arguments: map - array with the info of all map
 * 			  prize - struct with all the prize's info
 * Returns:
 *
 * Description: Creates a new prize in a random position of the field
 *****************************************************************************/
void create_prize(position_t* map, prize_t* prize)
{
	srand(time(0));
	for(int i = 0; i < N_Max_Prizes; i++)
	{
		if(prize[i].value == -1)
		{
			generate_valid_pos(map, prize[i].pos);
			prize[i].value = 1 + random()%5;
			map[prize[i].pos[0]*WINDOW_SIZE + prize[i].pos[1]].occ_status = 2;
			map[prize[i].pos[0]*WINDOW_SIZE + prize[i].pos[1]].idx = i;
			break;
		}
	}
}