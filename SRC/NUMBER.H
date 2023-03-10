#ifndef	___NUMBER_H___
#define	___NUMBER_H___

#include "common.h"


/************
    数値BG
 ************/
typedef struct
{
	uint32_t	value;			// 数値
	short		keta;			// 桁数
	int16_t		x;			// 描画座標x
	int16_t		y;			// 描画座標y
	uint16_t	buf[8];			// BGバッファ
	short		cnt;			// 汎用カウンタ
} Number;


void	Number_init(Number*, short, int16_t, int16_t);		// 初期化
void	Number_set(Number*, uint32_t);						// 数値設定
void	Number_update(Number*);								// 稼働
void	Number_draw(Number*);								// 描画

#endif
