
typedef struct {
	uint8 r;
	uint8 g;
	uint8 b;
} Flash_Color;




// Kinda stolen from SDL

typedef struct Flash_Surface {
	int w, h;
	uint32 *pixels;			/* Read-write */
} Flash_Surface;