#ifndef COMMON_H
#define COMMON_H

/* Maximum image dimensions supported without dynamic allocation.
   All scratch buffers are sized from these limits at compile time
   instead of being malloc'd/free'd at runtime. Raise them if you
   need to process larger images, but note that filter.c alone keeps
   close to 30 buffers of MAX_PIXELS floats alive, so RAM usage scales
   fast (MAX_PIXELS * 4 bytes * ~30 buffers).

   MAX_ROWS and MAX_COLS bound each dimension independently (e.g.
   filter.c's boxfilter() sizes cumheightvector[MAX_COLS] and
   cumheightvectorcol[MAX_ROWS] for the actual width/height, not just
   the pixel total), so both must cover the largest width AND the
   largest height across the images, including both portrait and
   landscape orientations (e.g. 480x640 and 640x480 samples). */
#define MAX_ROWS   640
#define MAX_COLS   640
#define MAX_PIXELS (MAX_ROWS * MAX_COLS)

#endif
