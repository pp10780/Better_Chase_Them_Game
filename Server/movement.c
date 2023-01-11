#include "movement.h"

/******************************************************************************
 * update_user_pos()
 *
 * Arguments: map - array with the info of all map positions
 *			  field_status - struct with info on all players/bots/prizes
 *			  new_pos - array that has the position to where the player 
 *			  wants to move 
 *			  idx - index of the intended player on client vector
 *			  
 * Returns:
 *
 * Description: Checks the status of the intended position for the player 
 *				and produces the correct outcome given the rules 
 *****************************************************************************/
void update_user_pos(position_t* map, field_status_t* field_status, int* new_pos, int idx)
{
	//if position empty move the player to said position
	if (map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == -1) 
	{	
		//mark previous position as empty
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].occ_status = -1;
		//move to new position and mark as occupied by player
		field_status->user[idx].pos[0] = new_pos[0];
		field_status->user[idx].pos[1] = new_pos[1];
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].occ_status= 0;
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].idx = idx;
	}
	//if position occupied by player damage the player occupying the position, gain health and remain in the same position
	else if(map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == 0 && field_status->user[idx].id != field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].id)
	{	
		//check if health full
		if (field_status->user[idx].hp < Max_Health){
			field_status->user[idx].hp++;
		}
		//damage the player in the intended position
		field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp--;
		//printf("HP LOSS: %d\n", field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp);
		
		//if damaged player's health reaches 0 mark position as empty
		if (field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp == 0){
			map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status = -1;
		}
	}
	//if position occupied by prize, collect it and gain health equivalent to the prize's value
	else if(map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == 2)
	{	
		//printf("HP GAINED: %d + %d\n", field_status->user[idx].hp, field_status->prize[map[new_pos[0] * WINDOW_SIZE + new_pos[1]].idx].value);
		field_status->user[idx].hp += field_status->prize[map[new_pos[0] * WINDOW_SIZE + new_pos[1]].idx].value;
		
		//check if health full
		if (field_status->user[idx].hp > Max_Health){
			field_status->user[idx].hp = 10;
		}

		//printf("HP ACTUAL: %d \n", field_status->user[idx].hp);
		
		//remove prize from the field
		field_status->prize[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].value = -1;

		//mark previous position as empty and move to prize's position
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].occ_status = -1;
		field_status->user[idx].pos[0] = new_pos[0];
		field_status->user[idx].pos[1] = new_pos[1];
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].occ_status = 0;
		map[field_status->user[idx].pos[0]*WINDOW_SIZE + field_status->user[idx].pos[1]].idx = idx;
	}	
	//if occupied by bot remain in the same position, nothing else happens
}


/******************************************************************************
 * update_bot_pos()
 *
 * Arguments: map - array with the info of all map
 *            field_status - struct with all the info related to the clients
 * 			  new_pos - array that has the position to where the bot wants to
 * 			  move
 *            idx - index of the bot that is moving
 * 
 * Returns:
 *
 * Description: Checks the status of the intended position for the bot 
 *				and produces the correct outcome given the rules 
 *****************************************************************************/
void update_bot_pos(position_t* map, field_status_t* field_status, int* new_pos, int idx)
{	//if position empty move the bot to said position
	if(map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == -1) 
	{	
		//mark previous position as empty
		map[field_status->bot[idx].pos[0]*WINDOW_SIZE + field_status->bot[idx].pos[1]].occ_status = -1;
		//move to new position and mark as occupied by bot
		field_status->bot[idx].pos[0] = new_pos[0];
		field_status->bot[idx].pos[1] = new_pos[1];
		map[field_status->bot[idx].pos[0]*WINDOW_SIZE + field_status->bot[idx].pos[1]].occ_status = 1;
		map[field_status->bot[idx].pos[0]*WINDOW_SIZE + field_status->bot[idx].pos[1]].idx = idx;
	}
	//if postiton occupied by player damage player and remain in same position
	else if(map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status == 0)
	{
		field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp--;
		//printf("USER: %c HP LOSS: %d\n", field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].id, field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp);
		
		//if damaged player's health reaches 0 mark position as empty
		if (field_status->user[map[new_pos[0]*WINDOW_SIZE + new_pos[1]].idx].hp == 0){
			map[new_pos[0]*WINDOW_SIZE + new_pos[1]].occ_status= -1;
		}
	//if position occupied either by prize or other bot nothing happens
	}
}

