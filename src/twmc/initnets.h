void init_nets();
int cleanup_nets();
void build_path_array() ;
void init_path_set() ;
void init_net_set() ;
void add_paths_to_cells() ;
void set_net_error();
void add_path( BOOL pathFlag, char *net );
void end_path(int lower_bound, int upper_bound, int priority );
void init_analog( char *net );
void set_cap_upper_bound( double cap );
void set_res_upper_bound( double res );
void set_net_type( int net_type );
void set_max_voltage_drop( double drop );
void add_common_pt();
void add2common( char *cell, char *pin );
void common_cap( char *cell, char *pin );
void common_res( char *cell, char *pin );
void start_net_capmatch( char *netname );
void add_net_capmatch( char *netname );
void start_net_resmatch( char *netname );
void add_net_resmatch( char *netname );
int get_total_paths();
