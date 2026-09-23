#ifndef COMMON_H
#define COMMON_H

/* Maximum image dimensions supported without dynamic allocation.
   All scratch buffers are sized from these limits at compile time
   instead of being malloc'd/free'd at runtime. Raise them if you
   need to process larger images, but note that filter.c alone keeps
   close to 30 buffers of MAX_PIXELS floats alive, so RAM usage scales
   fast (MAX_PIXELS * 4 bytes * ~30 buffers). Sized here with a margin
   above images/cat.pgm (276x276). */
#define MAX_ROWS   320
#define MAX_COLS   320
#define MAX_PIXELS (MAX_ROWS * MAX_COLS)

#endif
