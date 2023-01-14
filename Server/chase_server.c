#include "init_server.h"
#include "movement.h"

struct sigaction act;


void broadcast()
{
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
				perror("write: ");
				continue;
			}
		}
	}
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
}



void* prizes_thread()
{
	while(1)
	{
		sleep(5);
		pthread_mutex_lock(&mut);
		create_prize(map, field_status.prize);
		broadcast();
		draw_map();
		pthread_mutex_unlock(&mut);
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
		pthread_mutex_lock(&mut);
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
		pthread_mutex_unlock(&mut);
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
	int index = *(int*) arg;
	int continue_flag = field_status.user[index].n_deaths;
	sleep(10);
	if(field_status.user[index].n_deaths == continue_flag)
	{
		close(field_status.user[index].fd);
	}
	pthread_exit(NULL);
}

void* client_thread(void * arg)
{
	int tmp_fd = *(int*) arg;
	int index;
	int dead_idx;
	pthread_t client_id;
	message_c2s msg_rcv;
	message_s2c msg_send;

	while(1)
	{
		err = recv(tmp_fd, &msg_rcv, sizeof(msg_rcv), 0);
		if(err == -1)
		{
			//fprintf(stderr, "read_client_thread: %s\n", strerror(errno));            
			exit(0);
		}
		if(msg_rcv.type == Connect){
			//printf("Connect\n");
			//if id recieved is invalid send message to the client to try again
			if(!check_ID_val(field_status.user, msg_rcv.id))
			{
				invalid_id.id = '-';
				err = send(tmp_fd,
						&invalid_id, sizeof(invalid_id), MSG_NOSIGNAL);
				if(err == -1 || err == 0)
				{
					//fprintf(stderr, "write_connect_client_thread_1: %s\n", strerror(errno));            
					close(field_status.user[index].fd);
					pthread_exit(NULL);
				}
			}
			else
			{
				//if id is valid create a user
				index = create_user(field_status.user, map, msg_rcv.id, tmp_fd);
				//if server full send message to the client to try again later
				if(index == N_Max_Players)
				{
					invalid_id.id = '/';
					err = send(tmp_fd,
						&invalid_id, sizeof(invalid_id), MSG_NOSIGNAL);
					if(err == -1 || err == 0)
					{
						//fprintf(stderr, "write_connect_client_thread_2: %s\n", strerror(errno));            
						clear_user(&field_status.user[index], map);
						close(field_status.user[index].fd);
						pthread_exit(NULL);
					}
				}
				//if server not full send ball information to the client
				else
				{
					err = send(tmp_fd,
						&field_status.user[index], sizeof(field_status.user[index]), MSG_NOSIGNAL);
					if(err == -1 || err == 0)
					{
						//fprintf(stderr, "write_connect_client_thread_3: %s\n", strerror(errno));            
						clear_user(&field_status.user[index], map);
						close(field_status.user[index].fd);
						pthread_exit(NULL);
					}
					pthread_mutex_lock(&mut);
					broadcast();
					pthread_mutex_unlock(&mut);
					break;
				}
			}
		}
		
	}

	while(1)
	{		
		//if the health of the client trying to move alredy reached 0 send Health_0 message for it to disconnect and delete the user from the game
		err = recv(field_status.user[index].fd, &msg_rcv, sizeof(msg_rcv), 0);
		if(err == -1 || err == 0)
		{
			//fprintf(stderr, "read_client_thread_1: %s\n", strerror(errno));   
			clear_user(&field_status.user[index], map);
			//prev_field_status = field_status;
			pthread_mutex_lock(&mut);
			draw_map();
			broadcast();
			pthread_mutex_unlock(&mut);
			if(field_status.user[index].hp != 0)
			{
				close(field_status.user[index].fd);
			}
			pthread_exit(NULL);
			 
		}
		//if message received different than Continue_game and hp equal or below 0, invalid message received
		else if(msg_rcv.type != Continue_game && field_status.user[index].hp <= 0)
		{
			break;
		}
		else if(msg_rcv.type == Ball_movement)
		{
			new_pos[0] = field_status.user[index].pos[0];
			new_pos[1] = field_status.user[index].pos[1];
			//printf("Ball Movement\n");
			pthread_mutex_lock(&mut);
			get_new_pos(new_pos, msg_rcv.key[0]);
			dead_idx = update_user_pos(map, &field_status, new_pos, index);
			pthread_mutex_unlock(&mut);
			if(dead_idx != -1)
			{
				msg_send.type = Health_0;
				err = send(field_status.user[dead_idx].fd,
							&msg_send, sizeof(msg_send), MSG_NOSIGNAL);
				if(err == -1 || err == 0)
				{
					//fprintf(stderr, "write_ball_movement_client_thread_1: %s\n", strerror(errno));            
					clear_user(&field_status.user[dead_idx], map);
					close(field_status.user[dead_idx].fd);
				}
				
				pthread_create(&client_id, NULL, countdown_thread, &dead_idx);
			}
			//mask other players indexes to avoid cheating
			index_mask(&field_status);
			pthread_mutex_lock(&mut);
			draw_map();
			broadcast();
			pthread_mutex_unlock(&mut);
		}
		else if(msg_rcv.type == Continue_game)
		{
			field_status.user[index].n_deaths++;
			field_status.user[index].hp = 10;
			map[field_status.user[index].pos[0]*WINDOW_SIZE + field_status.user[index].pos[1]].occ_status= 0;
			//printf("Invalid message received\n");
		}
		//If message type does not match any of the protocol messages, invalid message received delete 
		else {
			break;
		}
	}
	//prev_field_status = field_status;
	clear_user(&field_status.user[index], map);
	pthread_mutex_lock(&mut);
	draw_map();
	pthread_mutex_unlock(&mut);
	close(field_status.user[index].fd);
	pthread_exit(NULL);
}


int main(int argc, char** argv){

    // set SIGPIPE action to ignore
    memset(&act, 0, sizeof act);
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &act, NULL) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
    }
	
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
		fprintf(stderr, "socket: %s\n", strerror(errno)); 
		exit(-1);
	}

	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(atoi(argv[2]));
	local_addr.sin_addr.s_addr = INADDR_ANY;

	err = bind(sock_fd, (struct sockaddr *)&local_addr,
							sizeof(local_addr));
	if(err == -1) {
		fprintf(stderr, "bind: %s\n", strerror(errno)); 
		exit(-1);
	}

	socklen_t local_addr_size = sizeof(local_addr);

	pthread_create(&prize_id, NULL, prizes_thread, NULL);
	pthread_create(&bot_id, NULL, bots_thread, NULL);
	init_map();
	prev_field_status = field_status;
	pthread_mutex_lock(&mut);
	draw_map();
	pthread_mutex_unlock(&mut);
	
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
