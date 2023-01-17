#ifndef	___BALL_H___
#define	___BALL_H___

#include "common.h"


/********
    球
 ********/
typedef struct
{
	uint16_t	color;			// 色
	int16_t		x;				// 表示座標
	int16_t		y;

	uint16_t	pat;
	int16_t		spr_x;			//
	int16_t		spr_y;			//
	bool		display;

	short	rot_cnt;			// 回転カウンタ
	short	rot_num;			// 回転場所
	short	erase_cnt;			// 消去カウンタ
	short	fall_cnt;			// 落下カウンタ
	short	wait_cnt;			// 消去判定待ちカウンタ
} Ball;

void	Ball_init_position(void);							// 回転座標作成
void	Ball_init(Ball*, int16_t, int16_t, uint16_t);		// 初期化
bool	Ball_update(Ball*);									// 稼働
void	Ball_draw(Ball* this);

void	Ball_set(Ball*, uint16_t);
void	Ball_rotate(Ball*, short, uint16_t);
void	Ball_erase(Ball*);
void	Ball_fall(Ball*, uint16_t);


/************
    回転軸
 ************/
typedef struct
{
	int16_t		x;				// 表示座標
	int16_t		y;
	short		cnt;			// プッシュカウンタ
} Pivot;

void	Set_pivot_color(uint16_t);
void	Pivot_init(Pivot*, int16_t, int16_t);				// 初期化
void	Pivot_update(Pivot*);								// 稼働

#endif
