void init_connect(void);
int init_serial_port(int which_port);


int get_serial_commands(int cmd_buffer [NO_CMDS]);
//int send_serial_commands(int cmd_buffer [NO_CMDS], unsigned char user_sending);
int send_remote_commands(unsigned char user_sending, int server_state, int from_which);
int serial_game_init(void);
int get_remote_cmds(int user_controlled, int server_state, int to_which);
void synch_gamestart(void);
void wait_on_port(void);
char anything_received(NET_CHANNEL *rec_channel);
//void sendval(unsigned char val);
//unsigned char receive_byte(void);

int get_buffer_channel(NET_CHANNEL *buf_channel);
char send_buffer_channel(NET_CHANNEL *send_channel, int buffer_size);

int add_byte_to_send_buffer(unsigned char add, int position);
unsigned char get_byte_from_rec_buffer(int position);

int add_int_to_send_buffer(int add, int position);
int get_int_from_rec_buffer(int position);


void server_get_connections(void);
int client_connect(void);

int conn_to_channel(void);
void shutdown_network(void);

enum
{
SERVER_BUFFER,
USER_BUFFER
};
