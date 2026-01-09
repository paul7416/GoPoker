#include "table_import.h"

/*
 * Load a .dat file with format: [uint32_t count][count * element_size bytes]
 * Returns allocated buffer, sets count via output parameter.
 * Errors are printed in red and return NULL.
 */
void *import_dat_file(char filename[], uint32_t *count, size_t element_size){
    FILE *f = fopen(filename, "rb");
    if(f == NULL)
    {
        fprintf(stderr, "\033[31mERROR: File %s failed to open\033[0m\n", filename);
        abort();
    }

    // Read header
    if(fread(count, sizeof(uint32_t), 1, f) != 1){
        fprintf(stderr, "\033[31mERROR: Failed to read count from %s\033[0m\n", filename);
        fclose(f);
        abort();
    }

    // Allocate buffer
    void *buffer = malloc((size_t)(*count) * element_size);
    if(buffer == NULL)
    {
        fprintf(stderr, "\033[31mERROR: %s buffer failed to allocate (%u elements)\033[0m\n",
                filename, *count);
        fclose(f);
        abort();
    }

    // Read elements
    size_t read = fread(buffer, element_size, *count, f);
    if(read != *count)
    {
        fprintf(stderr, "\033[31mERROR: %s header specifies %u elements, but only %zu read\033[0m\n",
                filename, *count, read);
        free(buffer);
        fclose(f);
        abort();
    }
    printf("imported %d elements from %s\n", *count, filename);

    fclose(f);
    return buffer;
}

