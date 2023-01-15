#ifndef INIT_H
#define INIT_H

#include "global_var.h"
#include "utils_server.h"

void init_server();
void init_users();
void init_prize();
void init_field();
void init_window();

int create_user(char id, int fd);
void create_bots();
void create_prize();

#endif