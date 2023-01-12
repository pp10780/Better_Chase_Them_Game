#include "init_server.h"
#include "movement.h"


void broadcast()
{
	pthread_mutex_lock(&mut);
	msg_send.type = Field_status;
	msg_send.field_status = field_status;
	for(int i = 0; i < N_Max_Players; i++)
	{	
		if (field_status.user[i].id != '-' && field_status.user[i].hp > 0)
		{
			err = write(field_status.user[i].fd, &msg_send,  sizeof(msg_send));
			if(err == -1)
			{
				fprintf(stderr, "error: %s\n", strerror(errno));            
				exit(0);
			}
		}
	}
	pthread_mutex_unlock(&mut);
	
}


/******************************************************************************
 * draw_player()
 *
 * Arguments: win - window display for the map
 *            player - struct with player information
 *            delete - flag that indicate if it is to draw or delete  
 * Returns: 
 *
 * Description: Draws/deletes a player on the display window 
 *****************************************************************************/
void draw_player(WINDOW *win, client_t * player, int delete){
    int ch;
    if(delete){
        ch = player->id;
    }else{
        ch = ' ';
    }
    int p_x = player->pos[0];
    int p_y = player->pos[1];
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);
}

/******************************************************************************
 * draw_prize()
 *
 * Arguments: win - window display for the map
              prize - struct with prize information
              delete - flag that indicate if it is to draw or delete  
 * Returns: 
 *
 * Description: Draws/deletes a prize on the display window 
 *****************************************************************************/
void draw_prize(WINDOW *win, prize_t * prize, int delete){
    int ch;
    if(delete){
        ch = prize->value + '0';
    }else{
        ch = ' ';
    }
    int p_x = prize->pos[0];
    int p_y = prize->pos[1];
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);
}

void draw_map()
{
	pthread_mutex_lock(&mut);
	for(int i = 0; i < 10; i++)
	{
		//delete previous players postions from main window
		if(prev_field_status.user[i].id != '-'){      
			draw_player(my_win, &prev_field_status.user[i], 0);        
		}
		//draw current player positions on the main window and players healths on message window
		if(field_status.user[i].id != '-'){      
			draw_player(my_win, &field_status.user[i], 1); 
			mvwprintw(message_win, i+1,1,"Player: %c : HP: %d\n", field_status.user[i].id, field_status.user[i].hp);       
		}
		//delete dead players healths from message window
		if(field_status.user[i].id == '-')
		{
			mvwprintw(message_win, i+1,1,"                     ");
		}
		//delete previous bot position from main window
		if(prev_field_status.bot[i].id != '-'){
			draw_player(my_win, &prev_field_status.bot[i], 0);   
		}
		//draw current bot positions on the main window
		if (field_status.bot[i].id != '-')
		{
			draw_player(my_win, &field_status.bot[i], 1);   
		}

		//delete previous prize positions and draw current ones on the main window
		if(field_status.prize[i].value != -1){      
			draw_prize(my_win, &prev_field_status.prize[i], 0);
			draw_prize(my_win, &field_status.prize[i], 1);        
		}

	}
	box(my_win, 0 , 0);
	wrefresh(my_win);
	wrefresh(message_win);

	prev_field_status = field_status;
	pthread_mutex_unlock(&mut);
}



void* prizes_thread()
{
	while(1)
	{
		sleep(5);
		create_prize(map, field_status.prize);
		broadcast();
		draw_map();
	}
}

int random_key(int idx)
{
    srand(time(NULL) + idx);
    int keys[4] = {KEY_DOWN, KEY_RIGHT, KEY_UP, KEY_LEFT};

    return keys[random()%4];   
}

void* bots_thread()
{
	int key;
	create_bots(field_status.bot, map, n_bots);
	while(1)
	{
		sleep(3);
		for(int i = 0; i < n_bots; i++){
			key = random_key(i);
			new_pos[0] = field_status.bot[i].pos[0];
			new_pos[1] = field_status.bot[i].pos[1];
			//printf("Bot Movement\n");
			get_new_pos(new_pos, key);
			update_bot_pos(map, &field_status, new_pos, i);
		}
		broadcast();
		draw_map();

	}
}

void init_map()
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

void* countdown_thread(void * arg)
{
	int fd = *(int*) arg;
	int* flag = (int*) arg;
	sleep(10);
	if(*flag != -1)
	{
		clear_user(&field_status.user[msg_rcv.idx], map);
		close(fd);
	}
	pthread_exit(NULL);
}

void* client_thread(void * arg)
{
	int tmp_fd = *(int*) arg;
	int index;
	int continue_flag = tmp_fd;
	pthread_t client_id;

	while(1)
	{
		err = read(tmp_fd, &msg_rcv, sizeof(msg_rcv));
		if(err == -1)
		{
			fprintf(stderr, "error: %s\n", strerror(errno));            
			exit(0);
		}
		if(msg_rcv.type == Connect){
			//printf("Connect\n");
			//if id recieved is invalid send message to the client to try again
			if(!check_ID_val(field_status.user, msg_rcv.id))
			{
				invalid_id.id = '-';
				err = write(tmp_fd,
						&invalid_id, sizeof(invalid_id));
				if(err == -1)
				{
					fprintf(stderr, "error: %s\n", strerror(errno));            
					exit(0);
				}
					
				broadcast();
			}
			else
			{
				//if id is valid create a user
				index = create_user(field_status.user, map, msg_rcv.id, tmp_fd);
				//if server full send message to the client to try again later
				if(index == N_Max_Players)
				{
					invalid_id.id = '/';
					err = write(tmp_fd,
						&invalid_id, sizeof(invalid_id));
					if(err == -1)
					{
						fprintf(stderr, "error: %s\n", strerror(errno));            
						exit(0);
					}
				}
				//if server not full send ball information to the client
				else
				{
					err = write(tmp_fd,
						&field_status.user[index], sizeof(field_status.user[index]));
					if(err == -1)
					{
						fprintf(stderr, "error: %s\n", strerror(errno));            
						exit(0);
					}
					break;
				}
			}
		}
		
	}

	while(1)
	{
		draw_map();
		//if the health of the client trying to move alredy reached 0 send Health_0 message for it to disconnect and delete the user from the game
		err = read(field_status.user[index].fd, &msg_rcv, sizeof(msg_rcv));
		if(err == -1)
		{
			if(errno = EBADF)
			{
				break;
			}
			else
			{
				fprintf(stderr, "error: %s\n", strerror(errno));            
				exit(0);
			}

		}
		if(msg_rcv.type == Ball_movement && field_status.user[msg_rcv.idx].hp <= 0)
		{
			//printf("Health_0\n");
			msg_send.type = Health_0;
			err = write(field_status.user[index].fd,
						&msg_send, sizeof(msg_send));
			if(err == -1)
			{
				fprintf(stderr, "error: %s\n", strerror(errno));            
				exit(0);
			}
			pthread_create(&client_id, NULL, countdown_thread, &continue_flag);
			//clear_user(&field_status.user[msg_rcv.idx], map);
			//break;
		}
		//if message received Ball movement, process movement and respond with updated field status
		else if(msg_rcv.type == Ball_movement)
		{
			new_pos[0] = field_status.user[msg_rcv.idx].pos[0];
			new_pos[1] = field_status.user[msg_rcv.idx].pos[1];
			//printf("Ball Movement\n");
			get_new_pos(new_pos, msg_rcv.key[0]);
			update_user_pos(map, &field_status, new_pos, msg_rcv.idx);
			//mask other players indexes to avoid cheating
			index_mask(&field_status);
			broadcast();
		}
		else if(msg_rcv.type == Disconnect)
		{
			//printf("Disconnect\n");
			clear_user(&field_status.user[msg_rcv.idx], map);
			close(field_status.user[index].fd);
			break;
		}
		else if(msg_rcv.type == Continue_game)
		{
			continue_flag = -1;
			field_status.user[msg_rcv.idx].hp = 10;
			map[field_status.user[index].pos[0]*WINDOW_SIZE + field_status.user[index].pos[1]].occ_status= 0;
			//printf("Invalid message received\n");
		}
	}
	

	pthread_exit(NULL);
}


int main(int argc, char** argv){
	
	pthread_t prize_id, bot_id, client_id;
	if(argc != 4)
    {
        printf("Invalid input arguments\nFORMAT: ./{EXECUTABLE} {IP_ADDRESS} {PORT} {N_BOTS}\n");
        exit(EXIT_FAILURE);
    }

	pthread_mutex_init(&mut, NULL);	
	init_server(&field_status, map);
	n_bots = atoi(argv[3]);

	//open socket for communication
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		fprintf(stderr, "error: %s\n", strerror(errno)); 
		exit(-1);
	}

	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(atoi(argv[2]));
	local_addr.sin_addr.s_addr = INADDR_ANY;

	err = bind(sock_fd, (struct sockaddr *)&local_addr,
							sizeof(local_addr));
	if(err == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno)); 
		exit(-1);
	}

	socklen_t local_addr_size = sizeof(local_addr);

	pthread_create(&prize_id, NULL, prizes_thread, NULL);
	pthread_create(&bot_id, NULL, bots_thread, NULL);
	init_map();
	prev_field_status = field_status;
	
	while(1){
		
		if (listen(sock_fd, 3) < 0) {
			perror("listen");
			exit(EXIT_FAILURE);
		}

		client_fd = accept(sock_fd, (struct sockaddr *)&local_addr, &local_addr_size);
		pthread_create(&client_id, NULL, client_thread, &client_fd);
		
	}
	endwin();
	close(sock_fd);

	exit(0);
}
