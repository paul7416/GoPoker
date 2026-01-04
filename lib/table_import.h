#ifndef TABLE_IMPORT_H
#define TABLE_IMPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void *import_dat_file(char filename[], uint32_t *count, size_t element_size);

#endif
