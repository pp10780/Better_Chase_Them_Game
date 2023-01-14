#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "../common/defines.h"
#include "../common/structs.h"
#include <stdio.h>
#include "utils_server.h"
#include "global_var.h"

int update_user_pos(position_t* map, field_status_t* field_status, int* new_pos, int idx);
int update_bot_pos(position_t* map, field_status_t* field_status, int* new_pos, int idx);

#endif