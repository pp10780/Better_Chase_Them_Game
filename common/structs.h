#ifndef STRUCT_H
#define STRUCT_H
typedef struct position_t
{
    // status: -1 - empty; 
    //          0 - player; 
    //          1 - bot; 
    //          2 - prize
    int occ_status;
    // player/bot/prize position on the struct
	int idx;
} position_t;

typedef struct prize_t
{
    int value;
    int pos[2];
} prize_t;

typedef struct client_t
{
    int fd;
    char id;
    int idx;
	int hp;
    int pos[2];
} client_t;

typedef struct field_status_t
{
    client_t user[10];
    prize_t prize[10];
    client_t bot[10];
}field_status_t;


//Dont send user idx
typedef struct message_s2c
{
    int type;
    field_status_t field_status;
}message_s2c;

typedef struct message_c2s
{
    int n_bots;
    int type;
    int idx;
    char id;
    int key[10];
} message_c2s;

#endif
