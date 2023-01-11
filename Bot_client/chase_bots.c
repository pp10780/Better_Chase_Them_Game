#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "../common/defines.h"
#include "../common/structs.h"

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


int main(int argc, char** argv){

    int n_bots;
    int err;

    //check if program was lauched using the right number of arguments
    if(argc != 3)
    {
        printf("Invalid input arguments\nFORMAT: ./{EXECUTABLE} {N_BOTS} /tmp/sock_16\n");
        exit(EXIT_FAILURE);
    }
    n_bots = atoi(argv[1]);

    //check if the number of bots introduced is valid
    if(n_bots > 10 || n_bots < 1)
    {
        printf("Invalid number of bots!\n");
        exit(EXIT_FAILURE);
    }

    //open socket for communication
	int sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_un local_client_addr;
	local_client_addr.sun_family = AF_UNIX;
	sprintf(local_client_addr.sun_path, "%s_%d", argv[2], getpid());

	unlink(local_client_addr.sun_path);
	err = bind(sock_fd, (struct sockaddr *)&local_client_addr,
							sizeof(local_client_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

    struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, argv[2]);

    message_c2s msg_send;

    //send connect message to server 
    msg_send.type = Connect_bots;

    msg_send.n_bots = n_bots;

    err = sendto(sock_fd,
	                    &msg_send, sizeof(msg_send), 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
    if(err == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));            
        exit(0);
    }

    msg_send.type = Bot_movement;

    while(1)
    {
        sleep(3);
        for(int i = 0; i < n_bots; i++)
        {
            msg_send.key[i] = random_key(i);
            printf("%d\n",msg_send.key[i]);
        }
        err = sendto(sock_fd,
                    &msg_send, sizeof(msg_send), 0,
                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
        if(err == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));            
            exit(0);
        }
    }
}