#include "global_var.h"

field_status_t field_status, prev_field_status;
position_t map[WINDOW_SIZE*WINDOW_SIZE];
int n_bots = 0;
WINDOW * my_win;
WINDOW * message_win;
pthread_mutex_t mut;