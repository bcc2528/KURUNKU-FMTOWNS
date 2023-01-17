#ifndef	___GRAPHICS_H___
#define	___GRAPHICS_H___

#include "common.h"

struct	FILLPTR {
		short	x1;
		short	y1;
		short	x2;
		short	y2;
		unsigned short color;
};

void	graphics_init();
void	graphics_quit();
void	set_zoom(int);
void	zoom();
void	zoom_in();
void	zoom_out();
int		FILL(struct FILLPTR *);
int		put_data();
void	timer_bar(int, int);
void	clear_text();
void	combo_gauge(int, int, unsigned short);

#endif
