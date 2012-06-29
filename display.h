void display_windows(BITMAP *bmp_p1, BITMAP *bmp_p2);
void init_cloud_pattern(void);
void init_object_sprites(void);

void init_stat_display(int dactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point);
void init_stat_disp2(int dplayer);
void display_actor_health(int dactor, int harmed);
void display_player_health(int dplayer, int harmed);
void reset_health_bar(int rplayer);

void display_actor_ammunition(int dactor);
void display_player_ammunition(int dplayer);

void display_actor_clips(int dactor);
void display_player_clips(int dplayer);

void display_actor_armour(int dactor);
void display_player_armour(int dplayer);

int init_armour_display(int dactor, int x_pos, int y_pos, int window_width, int bar_width, int pix_per_point, int colour1, int colour2);

void message(int user_target, const char *msg, int colour);
void teamed_message(int user_target, const char *msg, int team);

void update_score(void);
void ingame_scorelist(int mplayer, BITMAP *bmp, int centre_x);

