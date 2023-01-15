#ifndef THREADS_H
#define THREADS_H
#include "global_var.h"
#include "utils_server.h"
#include "init_server.h"
#include "movement.h"
#include "draw.h"

void* prizes_thread();
int random_key(int idx);
void* bots_thread();
void* countdown_thread(void * arg);
void* client_thread(void * arg);
void broadcast();

#endif