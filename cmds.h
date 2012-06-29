enum
{
CMD_KEY_UP1,
CMD_KEY_DOWN1,
CMD_KEY_LEFT1,
CMD_KEY_RIGHT1,
CMD_KEY_SHOOT1,
CMD_KEY_JUMP1,
CMD_KEY_CHANGE1,
CMD_KEY_DUCK1,
CMD_KEY_SCORE1,
CMD_KEY_NAMES1,
CMD_KEY_GRAPPLE1,
CMD_KEY_UP2,
CMD_KEY_DOWN2,
CMD_KEY_LEFT2,
CMD_KEY_RIGHT2,
CMD_KEY_SHOOT2,
CMD_KEY_JUMP2,
CMD_KEY_CHANGE2,
CMD_KEY_DUCK2,
CMD_KEY_SCORE2,
CMD_KEY_NAMES2,
CMD_KEY_GRAPPLE2
};

enum
{
CMD_UP,
CMD_DOWN,
CMD_LEFT,
CMD_RIGHT,
CMD_SHOOT,
CMD_JUMP,
CMD_CHANGE,
CMD_DUCK,
CMD_GRAPPLE,
CMD_DIG,
CMD_CHWEAP_0,
CMD_CHWEAP_1,
CMD_CHWEAP_2,
CMD_CHWEAP_3,
CMD_REWEAP_0,
CMD_REWEAP_1,
CMD_REWEAP_2,
CMD_REWEAP_3,
CMD_SPAWN_0,
CMD_SPAWN_1,
CMD_SPAWN_2,
CMD_SPAWN_3,
CMD_SPAWN_4,
CMD_SPAWN_5,
CMD_SPAWN_6,
CMD_SPAWN_7,
CMD_SPAWN_8
};


void enact_commands(void);
void clear_cmd_buffer(void);
void get_movement(void);
void get_keypress(void);
void init_cmds(void);
void init_cmds2(void);
void issue_user_command(int user_commanded, int command_given);
int send_out_commands(void);
void actor_tracer(int sactor);

int actor_shoot(int sactor, char async, int seed, int lock);
void respawn_command(int i, int k);
int pickup_game_thing(int ractor, int pcount, int async);
void actor_grapple(int sactor);
int discharge_weapon(int weapon_type, int x_pos, int y_pos, int x_speed, int y_speed, int owner, int spread_mult, float angle, int seed, int async, int lock);

#ifndef PI
#define PI 3.14159265358979323846
#endif
