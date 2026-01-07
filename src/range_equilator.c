#include "range_equilator.h"

EqualatorTables *initialize_equilator_tables()
{
    eval_tables->Flushes = (uint16_t*)import_dat_file("./DataFiles/flush_ranks.bin", &count, sizeof(uint16_t));


}
