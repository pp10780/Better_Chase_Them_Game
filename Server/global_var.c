#include "global_var.h"

field_status_t field_status, prev_field_status;
position_t map[WINDOW_SIZE*WINDOW_SIZE];
int sock_fd, client_fd;
int err;
int idx = 0;
int new_pos[2];
client_t invalid_id;
int n_bots = 0;
WINDOW * my_win;
WINDOW * message_win;
pthread_mutex_t mut;