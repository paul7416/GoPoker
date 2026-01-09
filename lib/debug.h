#ifndef DEBUG_H
#define DEBUG_H
#define WAYPOINT fprintf(stderr, "Waypoint %s:%d %s\n", __FILE__, __LINE__, __func__)
#define DBGX(x) fprintf(stderr, "%s:%d %s = 0x%lx\n", __FILE__, __LINE__, #x, (unsigned long)(x))
#define DBG(x) fprintf(stderr, "%s:%d %s = %ld\n", __FILE__, __LINE__, #x, (long)(x))
#endif

