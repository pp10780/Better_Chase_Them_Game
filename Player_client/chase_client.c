#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "../common/defines.h"
#include "../common/structs.h"

/******************************************************************************
 * draw_player()
 *
 * Arguments: win - window display for the map
              player - struct with player information
              delete - flag that indicate if it is to draw or delete  
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

/******************************************************************************
 * init_prev_field()
 *
 * Arguments: prev_field_status - struct with previous iteration field 
 *                                information  
 * Returns: 
 *
 * Description: Initializes previous filed status values 
 *****************************************************************************/
void init_prev_field(field_status_t* prev_field_status){
    for (int i = 0; i < 10; i++){
        prev_field_status->user[i].id = '-';
        prev_field_status->bot[i].id = '-';
        prev_field_status->prize[i].value = -1;
    }
}

int main(int argc, char** argv){

	int err;
    int key = -1;
    int client_fd;
    message_c2s msg_send;
    message_s2c msg_rcv;
	client_t client;
    client_t personal_info;
    field_status_t field_status;
    field_status_t prev_field_status;

    //check if program was lauched using the right number of arguments
    if(argc != 3)
    {
        printf("Invalid input arguments\nFORMAT: ./{EXECUTABLE} {IP_ADDRESS} {PORT}\n");
        exit(EXIT_FAILURE);
    }

    //open socket for communication
	int sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		fprintf(stderr, "error: %s\n", strerror(errno));
		exit(-1);
	}

    struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr)<= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd = connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {
        printf("\nConnection Failed \n");
        perror("error: ");
        return -1;
    }

    // Connect
    msg_send.type = Connect;
    char buffer[100];

    while(1)
    {
        
        printf("Choose a letter: ");
        fgets(buffer, 100, stdin);
        msg_send.id = buffer[0];
        //validate that the id inserted is a letter
        if((msg_send.id < 'A' || msg_send.id > 'Z') && (msg_send.id < 'a' || msg_send.id > 'z'))
        {
            printf("Invalid letter chosen!\n");
            continue;
        }

        err = write(sock_fd, &msg_send, sizeof(msg_send));                
        if(err == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));            
            exit(0);
        }
        
	    err = read(sock_fd, &client, sizeof(client));
        if(err == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));            
            exit(0);
        }

        //server response indicates that client id alredy exists 
        if(client.id == '-')
            printf("Letter already exists!\n");

        //server response indicates that server is already full
        else if(client.id == '/')
        {
            printf("Server is full! Try again later...\n");
            exit(0);
        }   
        //valid id and connection completed
        else
            break;

        sleep(0.5);

    }
	
    //store personal info recieved from the server
	personal_info = client;

	initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* creates a window and draws a border */
    WINDOW * message_win = newwin(20, 70, WINDOW_SIZE, 0);
	wrefresh(message_win);

    draw_player(my_win, &personal_info, 1);

    //init previous field status
    init_prev_field(&prev_field_status);
    prev_field_status.user[personal_info.idx] = personal_info;
    
	while(1){
        //get key input from user
        key = wgetch(my_win);
        //if the input is a directional key send a ball movement message
        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
            msg_send.type = Ball_movement;
            msg_send.key[0] = key;
            msg_send.id = personal_info.id;
            msg_send.idx = personal_info.idx;

            err = write(sock_fd, &msg_send, sizeof(msg_send));
            if(err == -1)
            {
                endwin();  
                fprintf(stderr, "error: %s\n", strerror(errno));          
                exit(0);
            }
            
        }
        //if the input is the q key send a disconnect message
        else if(key == 'q')
        {
            msg_send.type = Disconnect;
            msg_send.idx = personal_info.idx;
            err = write(sock_fd, &msg_send, sizeof(msg_send));
            if(err == -1)
            {
                endwin();  
                fprintf(stderr, "error: %s\n", strerror(errno));           
                exit(0);
            }
            break;
        }
		
		err = read(sock_fd, &msg_rcv, sizeof(msg_rcv));
        if(err == -1)
        {
            endwin();  
            fprintf(stderr, "error: %s\n", strerror(errno));    
            exit(0);
        }
        //if message recived is a field status update the window 
        if(msg_rcv.type == Field_status){
        
            field_status = msg_rcv.field_status;
            for(int i = 0; i < 10; i++)
            {
                //delete previous players postions from main window
                if(prev_field_status.user[i].id != '-' && prev_field_status.user[i].hp > 0){      
                    draw_player(my_win, &prev_field_status.user[i], 0);        
                }
                //draw current player positions on the main window and players healths on message window
                if(field_status.user[i].id != '-' && field_status.user[i].hp > 0){      
                    draw_player(my_win, &field_status.user[i], 1); 
                    mvwprintw(message_win, i+1,1,"Player: %c : HP: %d\n", field_status.user[i].id, field_status.user[i].hp);       
                }
                //delete dead players healths from message window
                if(field_status.user[i].id != '-' && field_status.user[i].hp == 0)
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
        //if health_0 message recieved print you died on message window
        else if(msg_rcv.type == Health_0){

    mvwprintw(message_win, 1,1,
         "\t__  __   ____    __  __                              \n"
         "\t\\ \\/ /  / __ \\  / / / /                           \n"
         "\t \\  /  / / / / / / / /                              \n"
         "\t / /  / /_/ / / /_/ /                                \n"
         "\t/_/   \\____/  \\____/                               \n"
         "\t                                                     \n"
         "\t                  ____     ____    ______    ____    \n"
         "\t                 / __ \\   /  _/   / ____/   / __ \\ \n"
         "\t                / / / /   / /    / __/     / / / /   \n"
         "\t               / /_/ /  _/ /    / /___    / /_/ /    \n"
         "\t              /_____/  /___/   /_____/   /_____/     \n"
         "\t                                                     \n");
         wrefresh(message_win);	
            sleep(2);
            break;
        }
	}

    //endwin();
    //touchwin(message_win);
    //if player died or disconncted print game over on message window and disconnect
    mvwprintw(message_win, 1,1,
         "\t   ______    ___     __  ___    ______                     \n"
         "\t  / ____/   /   |   /  |/  /   / ____/                     \n"
         "\t / / __    / /| |  / /|_/ /   / __/                        \n"
         "\t/ /_/ /   / ___ | / /  / /   / /___                        \n"
         "\t\\____/   /_/  |_|/_/  /_/   /_____/                       \n"
         "\t                                                           \n"
         "\t                        ____  _    __    ______    ____    \n"
         "\t                       / __ \\| |  / /   / ____/   / __ \\ \n"
         "\t                      / / / /| | / /   / __/     / /_/ /   \n"
         "\t                     / /_/ / | |/ /   / /___    / _, _/    \n"
         "\t                     \\____/  |___/   /_____/   /_/ |_|    \n"
         "\t                                                        \n");
    wrefresh(message_win);	
    sleep(2);
    endwin();

    close(sock_fd);
	exit(0);
}