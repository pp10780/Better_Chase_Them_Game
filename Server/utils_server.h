#ifndef UTILS_H
#define UTILS_H
#include <time.h>
#include <stdlib.h>
#include "../common/defines.h"
#include "../common/structs.h"
#include "global_var.h"

void generate_valid_pos(position_t* map, int* pos);
void get_new_pos(int* pos, int key);
void clear_user(client_t* user, position_t* map);
int check_ID_val(client_t* client, char id);
void index_mask(field_status_t* field_status);

#endif