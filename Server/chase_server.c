#include "init_server.h"
#include "movement.h"
#include "threads.h"
#include "draw.h"


int main(int argc, char** argv){
	
	pthread_t prize_id, bot_id, client_id;
	int err;
	int sock_fd, client_fd;
	
	if(argc != 4)
    {
        printf("Invalid input arguments\nFORMAT: ./{EXECUTABLE} {IP_ADDRESS} {PORT} {N_BOTS}\n");
        exit(EXIT_FAILURE);
    }

	pthread_mutex_init(&mut, NULL);	
	init_server();
	n_bots = atoi(argv[3]);

	if(n_bots < 0 || n_bots > N_Max_Bots)
	{
		printf("Invalid number of bots chosen! Please choose a number between 0 and 10.\n");
		exit(0);
	}

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

	//create threads for proze creation and bot control
	pthread_create(&prize_id, NULL, prizes_thread, NULL);
	pthread_create(&bot_id, NULL, bots_thread, NULL);

	//init server display window and draw the intial map
	init_window();
	prev_field_status = field_status;
	pthread_mutex_lock(&mut);
	draw_map();
	pthread_mutex_unlock(&mut);
	
	while(1){
		
		//server listens for potential clients trying to connect
		if (listen(sock_fd, 3) < 0) {
			perror("listen");
			exit(EXIT_FAILURE);
		}

		//server accepts new client connections and launches client threads
		client_fd = accept(sock_fd, (struct sockaddr *)&local_addr, &local_addr_size);
		pthread_create(&client_id, NULL, client_thread, &client_fd);
		
	}
	endwin();
	
	close(sock_fd);
	pthread_mutex_destroy(&mut);
	exit(0);
}
