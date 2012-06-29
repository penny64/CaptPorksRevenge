unsigned char get_grid(int grid_x, int grid_y);
unsigned char get_specific_grid(int grid_x, int grid_y);
void set_grid(int grid_x, int grid_y, int to_grid);

void set_underlying_grid(int grid_x, int grid_y, int to_grid);
unsigned char get_underlying_grid(int grid_x, int grid_y);
int get_grid_strength(int grid_x, int grid_y);
void set_grid_strength(int grid_x, int grid_y, int str_to);

unsigned char get_dirt(int dirt_x, int dirt_y);
void change_dirt(int dirt_x, int dirt_y, int colour);
void blacken_dirt(BITMAP *bmp, int dirt_x, int dirt_y, int chance);
int check_empty_area(int x1, int y1, int x2, int y2);
int check_free_area(int x1, int y1, int x2, int y2);
void destroy_dirt(BITMAP *bmp, int dirt_x, int dirt_y, int nothing);
void dig_dirt(BITMAP *bmp, int dirt_x, int dirt_y, int nothing);
void change_any_dirt(int dirt_x, int dirt_y, int colour);
void add_dirt(BITMAP *bmp, int dirt_x, int dirt_y, int added);
int get_brick(int brx, int bry);
unsigned char get_dirt_colour(int dirt_x, int dirt_y);

