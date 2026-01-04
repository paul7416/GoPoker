#include "range_equilator.h"
int main()
{


    int no_players = 3;
    Ranges *ranges = create_ranges(no_players);
    EquilatorTables *eq_tables = initialize_equilator_tables();
    
    for(int i = 0; i < 3; i++)
    {
        add_hand_to_range(i, eq_tables->hole_tables, ranges, 0); 
        add_hand_to_range(i, eq_tables->hole_tables, ranges, 1); 
        add_hand_to_range(i, eq_tables->hole_tables, ranges, 2); 
    }
    range_equilator(ranges, eq_tables);

}
