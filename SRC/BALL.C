/***************************

		���E��]��

 ***************************/

#include "ball.h"
#include "game.h"
#include "sprite.h"
#include <math.h>

static int16_t	rot_position[6][8][XY];				// ��]���W
static	uint16_t	pivot_color;

/******************
    ��]���W�쐬
 ******************/
void	Ball_init_position(void)
{
	double	_w = BALL_W*1.7320508/(1.0 + 1.7320508),
			_h = BALL_H/1.7320508;

	for (int i = 0; i < 6; i++) {
		double	_cs = cos(i*4*(PI/12.0)), _sn = sin(i*4*(PI/12.0));

		_w = -_w;
		rot_position[i][0][X] = 0;
		rot_position[i][0][Y] = 0;
		for (int j = 1; j < 8; j++) {
			rot_position[i][j][X] = (int16_t)(_w*(cos((i*4 - j)*(PI/12.0)) - _cs));
			rot_position[i][j][Y] = (int16_t)(_h*(sin((i*4 - j)*(PI/12.0)) - _sn));
		}
	}
}


/*******************************
    ������
		����	_x, _y = �ʒu
				_color = �F
 *******************************/
void	Ball_init(Ball* this, int16_t _x, int16_t _y, uint16_t _color)
{
	this->x = _x;
	this->y = _y;
	this->color = _color;

	this->rot_cnt	= 0;				// ��]�J�E���^
	this->erase_cnt	= 0;				// �����J�E���^
	this->fall_cnt	= 0;				// �����J�E���^
	this->wait_cnt	= 0;				// ��������҂��J�E���^
}

// �F�ݒ�
void	Ball_set(Ball* this, uint16_t _color)
{
	this->color = _color;
}

// ��]
void	Ball_rotate(Ball* this, short _num, uint16_t _color)
{
	this->color		= _color | 0x30;	// �F
	this->rot_num	= _num;				// ��]�ꏊ
	this->rot_cnt	= 8;				// ��]�p�J�E���^
}

// ����
void	Ball_erase(Ball* this)
{
	if ( this->erase_cnt == 0 ) {
		this->erase_cnt	= 0xff;
		this->wait_cnt	= 0;
	}
}

// ����
void	Ball_fall(Ball* this, uint16_t _color)
{
	this->color		= _color;
	this->fall_cnt	= 6;
	this->color |= 0x30;
}

/********************************
    �ғ�
		�߂�l	�ғ����Ă��邩
 ********************************/
bool	Ball_update(Ball* this)
{
	int16_t		_x = this->x,
				_y = this->y;
	//uint16_t	_pat = PAL_BALL + (this->color & 0x0f);
	uint16_t	_pat = (this->color & 0x0f);
	bool		_ret = true;
	this->display = false;

	if ( this->rot_cnt > 0 ) {						// ��]��
		if ( --this->rot_cnt == 0 ) {
			this->color &= 0x0f;
			this->wait_cnt = 10;
		}
		else {
			_x += rot_position[this->rot_num][this->rot_cnt][X];
			_y += rot_position[this->rot_num][this->rot_cnt][Y];
		}
		_ret = false;
	}
	else if ( this->erase_cnt > 0 ) {				// ������
		if ( this->erase_cnt == 0xff ) {
			this->erase_cnt = 24;
		}
		else if ( --this->erase_cnt == 0 ) {
			this->color = 0x08;
			return	true;
		}
		else if ( this->erase_cnt % 6 > 2 ) {
			return	true;
		}
		_pat += COLOR_MAX;
	}
	else if ( this->fall_cnt > 0 ) {				// ������
		if ( --this->fall_cnt == 0 ) {
			this->color &= 0x0f;
			this->wait_cnt = 14;
		}
		else {
			_y -= this->fall_cnt*BALL_H/6;
		}
	}
	else if ( this->wait_cnt > 0 ) {				// ��������҂��i�����j
		this->wait_cnt--;
	}
	else {
		_ret = false;
	}

	if ( !(this->color & 0x08)) {
		// �X�v���C�g�`��

		this->pat = _pat * 8;
		this->spr_x = _x - 16;
		this->spr_y = _y - 16;
		this->display = true;
	}

	return	_ret;
}


void	Ball_draw(Ball* this)
{
	if( this->display ) {
		set_sprite_4_32k(this->spr_x, this->spr_y, this->pat, true);
	}
}

void	Set_pivot_color(uint16_t c)
{
	pivot_color = c;
}

/*******************************
    ������
		����	_x, _y = �ʒu
 *******************************/
void	Pivot_init(Pivot* this, int16_t _x, int16_t _y)
{
	this->x		= _x;
	this->y		= _y;
	this->cnt	= 0;
}

/**********
    �ғ�
 **********/
void	Pivot_update(Pivot* this)
{
	if ( this->cnt > 0 ) {
		this->cnt--;
	}
	set_sprite(this->x - 16, this->y - 16, SPR_PIVOT + (this->cnt + 1)/2, PAL_PIVOT + pivot_color, true);			// �X�v���C�g�`��
}
