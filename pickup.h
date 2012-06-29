void destroy_pickup(int dpick);

int create_pickup(int pickup_type, int contains, int pickup_x, int pickup_y,
 int x_speed, int y_speed, int timeout, int force_index, int wait_clear);

void init_pickups(void);

int drop_pickups(int want_type, int want_contains);

void run_pickups(void);
void run_fruit(void);

