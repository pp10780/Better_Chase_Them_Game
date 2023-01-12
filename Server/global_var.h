#ifndef GLOBAL_H
#define GLOBAL_H
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ncurses.h>
#include <signal.h>
#include "../common/defines.h"
#include "../common/structs.h"

extern field_status_t field_status, prev_field_status;
extern position_t map[WINDOW_SIZE*WINDOW_SIZE];
extern int sock_fd, client_fd;
extern message_c2s msg_rcv;
extern message_s2c msg_send;
extern int err;
extern int idx;
extern int new_pos[2];
extern client_t invalid_id;
extern int n_bots;
extern WINDOW* my_win;
extern WINDOW* message_win;
extern pthread_mutex_t mut;

#endif