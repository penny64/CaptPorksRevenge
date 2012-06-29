//void move_bullet(int mbull, int move_x, int move_y);
int create_bullet(int bullet_type, int bullet_x, int bullet_y,
    int bullet_x_speed, int bullet_y_speed, unsigned char bullet_owner,
    int bullet_damage, int bullet_timer, int is_bullet, int bweight, float bangle,
    int facing, int status, int seed);

void run_bullets(void);

void init_bullets(void);
void destroy_grapple(int dbull);

void muzzle_flash(int mactor, int shot_type, int centre_x, int centre_y,
     int bull_x, int bull_y, int bull_x2, int bull_y2,
     int bull_x3, int bull_y3, int bspeed_x, int bspeed_y);
     
void slam_actor(int sactor, int x_slam, int y_slam, int weight);

void blast_dirt(int bx, int by, int brad, char digging);
void dirt_splodge(int bx, int by, int brad, int colour);

int team_colours(int which_team);
void actor_die(int dactor, int cause);
int hurt_actor(int hactor, int hurter, int harm, int shielded, int force, int async);
int bullet_rand(int rbull, int max);



