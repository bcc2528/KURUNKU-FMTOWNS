#ifndef	___BALL_H___
#define	___BALL_H___

#include "common.h"


/********
    ��
 ********/
typedef struct
{
	uint16_t	color;			// �F
	int16_t		x;				// �\�����W
	int16_t		y;

	uint16_t	pat;
	int16_t		spr_x;			//
	int16_t		spr_y;			//
	bool		display;

	short	rot_cnt;			// ��]�J�E���^
	short	rot_num;			// ��]�ꏊ
	short	erase_cnt;			// �����J�E���^
	short	fall_cnt;			// �����J�E���^
	short	wait_cnt;			// ��������҂��J�E���^
} Ball;

void	Ball_init_position(void);							// ��]���W�쐬
void	Ball_init(Ball*, int16_t, int16_t, uint16_t);		// ������
bool	Ball_update(Ball*);									// �ғ�
void	Ball_draw(Ball* this);

void	Ball_set(Ball*, uint16_t);
void	Ball_rotate(Ball*, short, uint16_t);
void	Ball_erase(Ball*);
void	Ball_fall(Ball*, uint16_t);


/************
    ��]��
 ************/
typedef struct
{
	int16_t		x;				// �\�����W
	int16_t		y;
	short		cnt;			// �v�b�V���J�E���^
} Pivot;

void	Set_pivot_color(uint16_t);
void	Pivot_init(Pivot*, int16_t, int16_t);				// ������
void	Pivot_update(Pivot*);								// �ғ�

#endif
