#include "table_import.h"
/*
 * Load a .dat file with format: [uint32_t count][count * element_size bytes]
 * Returns allocated buffer, sets count via output parameter.
 */
void *import_dat_file(char filename[], uint32_t *count, size_t element_size){
    FILE *f = fopen(filename, "rb");
    if(f == NULL)
    {
        printf("File %s failed to open\n", filename);
        return NULL;
    }
    fread(count, sizeof(uint32_t), 1, f);
    void *buffer = malloc((*count) * element_size);
    if(buffer == NULL)
    {
        printf("%s buffer failed to allocate\n", filename);
        fclose(f);
        return NULL;
    }
    size_t read = fread(buffer, element_size, *count, f);
    if(read != *count)
    {
        printf("header data specified length of %d but only %ld read\n", *count, read);
    }
    fclose(f);
    return buffer;
}
