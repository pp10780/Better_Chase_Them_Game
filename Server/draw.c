#include "draw.h"

/******************************************************************************
 * draw_player()
 *
 * Arguments: player - struct with player information
 *            delete - flag that indicate if it is to draw or delete  
 * Returns: 
 *
 * Description: Draws/deletes a player on the display window 
 *****************************************************************************/
void draw_player(client_t * player, int delete_val){
    int ch;
    if(delete_val){
        ch = player->id;
    }else{
        ch = ' ';
    }
    int p_x = player->pos[0];
    int p_y = player->pos[1];
    wmove(my_win, p_y, p_x);
    waddch(my_win,ch);
    wrefresh(my_win);
}

/******************************************************************************
 * draw_prize()
 *
 * Arguments: prize - struct with prize information
              delete - flag that indicate if it is to draw or delete  
 * Returns: 
 *
 * Description: Draws/deletes a prize on the display window 
 *****************************************************************************/
void draw_prize(prize_t * prize, int delete_val){
    int ch;
    if(delete_val){
        ch = prize->value + '0';
    }else{
        ch = ' ';
    }
    int p_x = prize->pos[0];
    int p_y = prize->pos[1];
    wmove(my_win, p_y, p_x);
    waddch(my_win,ch);
    wrefresh(my_win);
}


/******************************************************************************
 * draw_map()
 *
 * Arguments:  
 * Returns: 
 *
 * Description: Draws the map to be displayed on the window
 *****************************************************************************/
void draw_map()
{
	for(int i = 0; i < N_Max_Players - n_bots - N_Max_Prizes; i++)
	{
		//delete previous players postions from main window
		if(prev_field_status.user[i].id != '-'){      
			draw_player(&prev_field_status.user[i], 0);        
		}
		//draw current player positions on the main window and players healths on message window
		if(field_status.user[i].id != '-'){      
			draw_player(&field_status.user[i], 1); 
			mvwprintw(message_win, i+1,1,"Player: %c : HP: %d\n", field_status.user[i].id, field_status.user[i].hp);       
		}
		//delete dead players healths from message window
		if(field_status.user[i].id == '-')
		{
			mvwprintw(message_win, i+1,1,"                     ");
		}
		if(i < 10)
		{
			//delete previous bot position from main window
			if(prev_field_status.bot[i].id != '-'){
				draw_player(&prev_field_status.bot[i], 0);   
			}
			//draw current bot positions on the main window
			if (field_status.bot[i].id != '-')
			{
				draw_player(&field_status.bot[i], 1);   
			}

			//delete previous prize positions and draw current ones on the main window
			if(field_status.prize[i].value != -1){      
				draw_prize(&prev_field_status.prize[i], 0);
				draw_prize(&field_status.prize[i], 1);        
			}
		}
	}
	box(my_win, 0 , 0);
	wrefresh(my_win);
	wrefresh(message_win);

	prev_field_status = field_status;
}