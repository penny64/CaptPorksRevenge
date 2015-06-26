void run_async(void);
void init_async(void);

void async_dirt(int x, int y, int radius, int colour);
void async_user_shoots(int shuser, int shactor, int wpn_fired);


void async_client_join(void);
//int async_new_client(char *client_address, NET_CHANNEL *cl_channel);
void async_init_clients(void);
void async_client_get_clients(void);

void async_hurt(int hactor, int hurter, int harm);
void async_kill(int kactor, int cause);
void async_spawn(int sactor, int soldier);
void async_pickup(int chpick, int type, int contains, int x,
      int y, int timeout, int wait_clear);
void async_take_pickup(int chactor, int chpick);
void async_score_message(int stype, int user1, int user2);
void async_smart(int abull);
void async_smart_init(int abull, int client_no, int weapon);
void async_smart_destroy(int dbull);

void async_end_game(int end_status);

