#ifndef UTILS_H
#define UTILS_H
#include <time.h>
#include <stdlib.h>
#include "../common/defines.h"
#include "../common/structs.h"
#include "global_var.h"

void generate_valid_pos(int* pos);
void get_new_pos(int* pos, int key);
void clear_user(int index);
int check_ID_val(client_t* client, char id);
void index_mask();

#endif