#include "init_server.h"

/******************************************************************************
 * init_server()
 *
 * Arguments:
 * Returns:
 *
 * Description: Initializes the server
 *****************************************************************************/
void init_server(){
	init_users(field_status.user);
	init_field(map);
	init_prize(field_status.prize);
	init_users(field_status.bot);

	for (int i = 0; i < 5; i++){
		create_prize(map, field_status.prize);
	}
}

/******************************************************************************
 * init_client()
 *
 * Arguments:
 * Returns:
 *
 * Description: Initializes the client
 *****************************************************************************/
void init_users(){

	for (int i = 0; i < N_Max_Players - n_bots - N_Max_Prizes; i++){
		field_status.user[i].id = '-';
		field_status.user[i].pos[0] = -1;
		field_status.user[i].pos[1] = -1;
		field_status.user[i].hp = -1;
		
	}
}

/******************************************************************************
 * init_prize()
 *
 * Arguments: 
 * Returns:
 *
 * Description: Initializes the prize
 *****************************************************************************/
void init_prize()
{
	for(int i = 0; i < N_Max_Prizes; i++)
	{
		field_status.prize[i].value = -1;
	}
}

/******************************************************************************
 * init_field()
 *
 * Arguments: 
 * Returns:
 *
 * Description: Initializes the field
 *****************************************************************************/
void init_field(){
	for (int i = 0; i < WINDOW_SIZE*WINDOW_SIZE; i++){
		map[i].occ_status = -1;
	}
}


/******************************************************************************
 * init_window()
 *
 * Arguments: 
 * Returns:
 *
 * Description: Initializes the ncurses windows
 *****************************************************************************/
void init_window()
{
	initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */
    my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* creates a window and draws a border */
    message_win = newwin(20, 70, WINDOW_SIZE, 0);
	wrefresh(message_win);
}


/******************************************************************************
 * create_user()
 *
 * Arguments: id - ID chosen by the new player
 *			  fd - File descriptor associated with new player
 *            
 * Returns: returns the index of the new user in the array of player clients 
 *
 * Description: Receives the new user that entered the game and puts it in the 
 *              map
 *****************************************************************************/
int create_user(char id, int fd){
	int i;
	for(i = 0; i < N_Max_Players; i++){
		if (field_status.user[i].id == '-'){
			field_status.user[i].fd = fd;
			field_status.user[i].id = id;
			field_status.user[i].idx = i;
			field_status.user[i].n_deaths = 0;
			generate_valid_pos(field_status.user[i].pos);
			field_status.user[i].hp = 10;
			map[field_status.user[i].pos[0]*WINDOW_SIZE + field_status.user[i].pos[1]].occ_status = 0;
			map[field_status.user[i].pos[0]*WINDOW_SIZE + field_status.user[i].pos[1]].idx = i;
			break;
		}
	}
	return i;

}

/******************************************************************************
 * create_bots()
 *
 * Arguments:
 * Returns:
 *
 * Description: Creates the bots and puts them on the map
 *****************************************************************************/
void create_bots(){
	for (int i = 0; i < n_bots; i++){
		field_status.bot[i].id = '*';
		field_status.bot[i].idx = i;
		generate_valid_pos(field_status.bot[i].pos);
		map[field_status.bot[i].pos[0]*WINDOW_SIZE + field_status.bot[i].pos[1]].occ_status = 1;
		map[field_status.bot[i].pos[0]*WINDOW_SIZE + field_status.bot[i].pos[1]].idx = i;
	}
	
}

/******************************************************************************
 * create_prize()
 *
 * Arguments: 
 * Returns:
 *
 * Description: Creates a new prize in a random position of the field
 *****************************************************************************/
void create_prize()
{
	srand(time(0));
	for(int i = 0; i < N_Max_Prizes; i++)
	{
		if(field_status.prize[i].value == -1)
		{
			generate_valid_pos(field_status.prize[i].pos);
			field_status.prize[i].value = 1 + random()%5;
			map[field_status.prize[i].pos[0]*WINDOW_SIZE + field_status.prize[i].pos[1]].occ_status = 2;
			map[field_status.prize[i].pos[0]*WINDOW_SIZE + field_status.prize[i].pos[1]].idx = i;
			break;
		}
	}
}