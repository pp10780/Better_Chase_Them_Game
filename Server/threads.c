#include "threads.h"

/******************************************************************************
 * prizes_thread()
 *
 * Arguments:
 * Returns: 
 *
 * Description: Function that controls the generation prizes on a seperate 
 * 				thread.
 *****************************************************************************/
void* prizes_thread()
{
	while(1)
	{
		//wait 5 seconds
		sleep(5);
		//protect field status from being altered while other threads access it
		pthread_mutex_lock(&mut);
		create_prize();
		broadcast();
		draw_map();
		pthread_mutex_unlock(&mut);
	}
}

/******************************************************************************
 * bots_thread()
 *
 * Arguments:
 * Returns: 
 *
 * Description: Function that controls the movement of bots on a seperate 
 * 				thread.
 *****************************************************************************/

void* bots_thread()
{
	int key;
	int new_pos[2];
	create_bots(field_status.bot, n_bots);
	while(1)
	{
		sleep(3);
		
		for(int i = 0; i < n_bots; i++){
			key = random_key(i);
			new_pos[0] = field_status.bot[i].pos[0];
			new_pos[1] = field_status.bot[i].pos[1];
			get_new_pos(new_pos, key);
			//protect field status from being altered while other threads access it
			pthread_mutex_lock(&mut);
			update_bot_pos(new_pos, i);
			pthread_mutex_unlock(&mut);
		}
		broadcast();
		draw_map();
		
	}
}

/******************************************************************************
 * countdown_thread()
 *
 * Arguments:
 * Returns: 
 *
 * Description: Function with a 10 second timer after a user is killed and 
				eliminates said user if no continue message is received. 
 *****************************************************************************/
void* countdown_thread(void * arg)
{
	int index = *(int*) arg;
	int continue_flag = field_status.user[index].n_deaths;
	sleep(10);
	if(field_status.user[index].n_deaths == continue_flag)
	{
		clear_user(index);
		close(field_status.user[index].fd);
	}
	pthread_exit(NULL);
}


/******************************************************************************
 * client_thread()
 *
 * Arguments:
 * Returns: 
 *
 * Description: Function that manages each user's actions. Each client has is 
 *              run on his own thread.
 *****************************************************************************/
void* client_thread(void * arg)
{
	int tmp_fd = *(int*) arg;
	int index;
	int dead_idx;
	int new_pos[2];
	pthread_t countdown_id;
	message_c2s msg_rcv;
	message_s2c msg_send;

	while(1)
	{
		err = recv(tmp_fd, &msg_rcv, sizeof(msg_rcv), 0);
		if(err == -1)
		{
			exit(0);
		}
		if(msg_rcv.type == Connect){
			//if id received is invalid send message to the client to try again
			if(!check_ID_val(field_status.user, msg_rcv.id))
			{
				invalid_id.id = '-';
				err = send(tmp_fd,
						&invalid_id, sizeof(invalid_id), MSG_NOSIGNAL);
				if(err == -1 || err == 0)
				{
					close(field_status.user[index].fd);
					pthread_exit(NULL);
				}
			}
			else
			{
				//if id is valid create a user
				pthread_mutex_lock(&mut);
				index = create_user(msg_rcv.id, tmp_fd);
				pthread_mutex_unlock(&mut);
				//if server full send message to the client to try again later
				if(index == N_Max_Players)
				{
					invalid_id.id = '/';
					err = send(tmp_fd,
						&invalid_id, sizeof(invalid_id), MSG_NOSIGNAL);
					if(err == -1 || err == 0)
					{
						pthread_mutex_lock(&mut);         
						clear_user(index);
						pthread_mutex_unlock(&mut);
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
						pthread_mutex_lock(&mut);          
						clear_user(index);
						pthread_mutex_unlock(&mut);
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
		//wait for client to send a message to the server
		err = recv(field_status.user[index].fd, &msg_rcv, sizeof(msg_rcv), 0);
		//if there is an error on recv eliminate player and notify all others 
		if(err == -1 || err == 0)
		{
			pthread_mutex_lock(&mut);
			draw_map();
			broadcast();
			pthread_mutex_unlock(&mut);
			//in case the player disconnected instead of dying and not interacting for 10 seconds.
			if(field_status.user[index].hp != 0)
			{
				pthread_mutex_lock(&mut);
				clear_user(index);
				pthread_mutex_unlock(&mut);
				close(field_status.user[index].fd);
			}
			pthread_exit(NULL);
			 
		}
		//if message received different than Continue_game and hp equal or below 0, invalid message received
		else if(msg_rcv.type != Continue_game && field_status.user[index].hp <= 0)
		{
			break;
		}
		//if message received Ball movement, process movement and send updated field status to all connected players
		else if(msg_rcv.type == Ball_movement)
		{
			new_pos[0] = field_status.user[index].pos[0];
			new_pos[1] = field_status.user[index].pos[1];
			
			get_new_pos(new_pos, msg_rcv.key[0]);
			pthread_mutex_lock(&mut);
			dead_idx = update_user_pos(new_pos, index);
			pthread_mutex_unlock(&mut);
			//if a player was killed in this move send health_0 message to said player and initiate 10 second timer
			if(dead_idx != -1)
			{
				msg_send.type = Health_0;
				err = send(field_status.user[dead_idx].fd,
							&msg_send, sizeof(msg_send), MSG_NOSIGNAL);
				//if there is an error in send eliminate player and close thread
				if(err == -1 || err == 0)
				{          
				pthread_mutex_lock(&mut);
				clear_user(index);
				pthread_mutex_unlock(&mut);
					close(field_status.user[dead_idx].fd);
				}
				//creates a thread where the player is eliminated after 10 seconds if no Continue message is received
				pthread_create(&countdown_id, NULL, countdown_thread, &dead_idx);
			}
			//mask other players indexes to avoid cheating
			index_mask();
			//protect field status from being altered by other threads while being broadcasted and drawn on the window
			pthread_mutex_lock(&mut);
			draw_map();
			broadcast();
			pthread_mutex_unlock(&mut);
		}
		//if message received is Continue_game alter n_deaths so that count down thread does not eliminate player and restart said player's health 
		else if(msg_rcv.type == Continue_game)
		{
			field_status.user[index].n_deaths++;
			field_status.user[index].hp = 10;
			map[field_status.user[index].pos[0]*WINDOW_SIZE + field_status.user[index].pos[1]].occ_status= 0;
			broadcast();
		}
		//If message type does not match any of the protocol messages, invalid message received delete 
		else {
			break;
		}
	}
	
	pthread_mutex_lock(&mut);
	clear_user(index);
	draw_map();
	pthread_mutex_unlock(&mut);
	close(field_status.user[index].fd);
	pthread_exit(NULL);
}