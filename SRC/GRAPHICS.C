#include "graphics.h"

#define MODE_5 0

char	work[EgbWorkSize];
int zoom_i;
int zoom_n;

void Write_CRTC_register(int address, int data)
{
	_outb( 0x0440, address);
	_outw( 0x0442, data);
}

unsigned short Read_CRTC_register(int address)
{
	_outb( 0x0440, address);
	return _inw( 0x0442 );
}

void graphics_init()
{
	zoom_n = 2;

	EGB_init(work, 1536);

#if MODE_5
	EGB_resolution(work, 1, 5);
	EGB_resolution(work, 0, 5);

	EGB_writePage(work, 1);
	EGB_displayStart(work, 0, 0, 0);
	EGB_displayStart(work, 2, 2, 2);
	EGB_displayStart(work, 3, 256, 240);
	EGB_writePage(work, 0);
	EGB_displayStart(work, 0, 0, 0);
	EGB_displayStart(work, 2, 2, 2);
	EGB_displayStart(work, 3, 256, 240);

	// Wide
	Write_CRTC_register( 0, 80);
	Write_CRTC_register( 1, 590);
	Write_CRTC_register( 4, 669);
	Write_CRTC_register( 29, 3);
	Write_CRTC_register( 9, 130);
	Write_CRTC_register( 18, 130);
	Write_CRTC_register( 10, 642);
	Write_CRTC_register( 11, 130);
	Write_CRTC_register( 22, 130);
	Write_CRTC_register( 12, 642);
#else
	EGB_resolution(work, 1, 8);
	EGB_resolution(work, 0, 8);

	EGB_writePage(work, 1);
	EGB_displayStart(work, 0, 0, 0);
	EGB_displayStart(work, 2, 5, 1);
	EGB_displayStart(work, 3, 256, 240);
	EGB_writePage(work, 0);
	EGB_displayStart(work, 0, 0, 0);
	EGB_displayStart(work, 2, 5, 1);
	EGB_displayStart(work, 3, 256, 240);

#endif

	EGB_color(work, 1, 0x8000);		// スプライト画面初期化
	EGB_partClearScreen(work);
	EGB_color(work, 0, 0x7fff);		// グラフィック画面初期化
	EGB_displayPage(work, 1, 3);	// プライオリティ設定

}

void	graphics_quit()
{
	EGB_init(work, 1536);

	EGB_resolution(work, 1, 1);
	EGB_resolution(work, 0, 1);
	EGB_writePage(work, 1);
	EGB_color(work, 1, 0x0);
	EGB_partClearScreen(work);
	EGB_writePage(work, 0);
	EGB_color(work, 0, 0x0);
	EGB_partClearScreen(work);
}

void	set_zoom(int n)
{
	zoom_i = n;
}

void	zoom()
{
#if MODE_5
	static const
	int		zoom_table[9][3] =
			{
				{ 0, 2, 2},
				{ 512, 2, 3}, // (4 * 128)
				{ 562, 3, 3}, // 33 + (4 * 128)
				{ 1074, 3, 4}, // 33 + (8 * 128)
				{ 1330, 3, 5}, // 33 + (10 * 128)
				{ 1603, 4, 5}, // 67 + (12 * 128)
				{ 1859, 4, 6}, // 67 + (14 * 128)
				{ 2115, 4, 7}, // 67 + (16 * 128)
				{ 1987, 4, 7}, // 67 + (15 * 128)
			};
#else
	static const
	int		zoom_table[9][3] =
			{
				{ 0, 5, 1},
				{ 21, 6, 1}, // 21
				{ 149, 6, 2}, // 21 + 128 = 149
				{ 164, 7, 2}, // 36 + 128 = 164
				{ 548, 7, 3}, // 36 + (2 * 128) = 292
				{ 559, 8, 3}, // 47 + (2 * 128) = 303
				{ 1583, 8, 4}, // 47 + (10 * 128) = 1327
				{ 1593, 9, 4}, // 57 + (10 * 128) = 1337
				{ 1600, 10, 4}, // 64 + (10 * 128) = 1334
			};
#endif

	if(zoom_i == 0)
	{
		return;
	}

	if( ((zoom_n >= 16) && (zoom_i >= 1)) || ((zoom_n <= 0) &&  (zoom_i <= -1)) )
	{
		return;
	}


	zoom_n += zoom_i;
	if(zoom_n >= 16)
	{
		zoom_n = 16;
		zoom_i = 0;
	}else if(zoom_n <= 0)
	{
		zoom_n = 0;
		zoom_i = 0;
	}

	int reg;

	reg =  zoom_table[zoom_n / 2][0];
	Write_CRTC_register( 17, reg);
	Write_CRTC_register( 21, reg);

	int zoom_x = zoom_table[zoom_n / 2][1] - 1;
	int zoom_y = zoom_table[zoom_n / 2][2] - 1;
	reg = zoom_y;
	reg = (reg << 4) + zoom_x;
	reg = (reg << 4) + zoom_y;
	reg = (reg << 4) + zoom_x;
	Write_CRTC_register( 27, reg);
}

int		FILL(struct FILLPTR *ptr)
{
	_Far unsigned short *vram;
	_FP_SEG(vram) = 0x104;
	_FP_OFF(vram) = 0x0;

	int offset = ptr->x1 + (ptr->y1 * 256);
	int offset_x = ptr->x2 - ptr->x1;
	int offset_y = ptr->y2 - ptr->y1;

	for(int y = 0; y <= offset_y;y++)
	{

		for(int x = 0; x <= offset_x;x++)
		{
			vram[offset++] = ptr->color;
		}
		offset += 255 - offset_x;
	}

	return 0;
}

void	zoom_in()
{
	zoom_n = 15;
	zoom_i = 1;
	zoom();
}

void	zoom_out()
{
	zoom_n = 1;
	zoom_i = -1;
	zoom();
}

/* 展開されたものを表示する								*/
/*    そのまま表示する場合は, EGB_putBlockを使えばよい　*/
int put_data( char *buf, int lofs, int lines) 
{
	struct {
		char *bp ;
		short sel ;
		short sx,sy,ex,ey ;
	} p ;		/* EGB_putBlockのパラメータ */

	p.bp = buf ;
	p.sel = getds() ;
	p.sx = 0 ;
	p.ex = 256 - 1 ;
	p.sy = lofs ;
	p.ey = lofs + lines -1 ;

	EGB_writePage(work, 0);
	EGB_putBlock( work , 0, (char *)&p ) ;

	return 0 ;
}

void timer_bar(int x, int y)
{

}

void clear_text()
{

}

void combo_gauge(int x, int y, unsigned short c)
{

}
