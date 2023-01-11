#ifndef INIT_H
#define INIT_H

#include "global_var.h"
#include "utils_server.h"

void init_server(field_status_t* field_status, position_t* map);
void init_client(client_t* client);
void init_prize(prize_t* prize);
void init_field(position_t* map);

int create_user(client_t* user, position_t* map, char id, int fd);
void create_bots(client_t* bot, position_t* map, int n_bots);
void create_prize(position_t* map, prize_t* prize);

#endif