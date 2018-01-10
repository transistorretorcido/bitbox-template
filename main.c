#include "bitbox.h"
#include "lib/blitter/blitter.h" 

void btboxTemplate_init();
void (*do_frame) (void); // pointer to frame action/level / ... 


void game_init()
{
	blitter_init();
	btbxTemplate_init(0); // start
}

void game_frame()
{
	if (do_frame) 
		do_frame();
}

