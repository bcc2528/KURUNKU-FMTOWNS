/************************

		数値BG

 ************************/

#include "number.h"
#include "game.h"
#include "sprite.h"


/***********************************
    初期化
		引数	_keta  = 桁数
				_x, _y = 描画位置
 ***********************************/
void	Number_init(Number* this, short _keta, int16_t _x, int16_t _y)
{
	this->keta	= _keta;
	this->value	= 0;
	this->x		= _x;
	this->y		= _y;
	this->cnt	= 0;
}

/*****************************
    数値設定
		引数	_val = 数値
 *****************************/
void	Number_set(Number* this, uint32_t _val)
{
	this->value = _val;
}


/**********
    稼働
 **********/
void	Number_update(Number* this)
{
	short		k = this->keta;
	uint32_t	t = this->value;

	
	do {
		this->buf[k] = (t % 10) + 1;

		k--;
		t /= 10;
	} while ( t > 0 ) ;

	for (; k >= 0; k--) {
		this->buf[k] = 0;
	}
}


/**********
    描画
 **********/
void	Number_draw(Number* this)
{

	for (short j = this->keta; j > 0; j--) {
		set_sprite(this->x  + (j * 8), this->y, this->buf[j], PAL_NUMBER, false);
	}
}
