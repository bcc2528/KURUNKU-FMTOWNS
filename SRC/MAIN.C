/**************************

		メイン

 **************************/

#include "common.h"
#include "app.h"
#include "graphics.h"
#include "sprite.h"
#include "sound.h"
#include "game.h"

char	mwork[MosWorkSize];

/******************
    フェードイン
 ******************/
void	fade_in(void)
{
	//CONTRAST(-2);
}

/********************
    フェードアウト
 ********************/
void	fade_out(void)
{
	//CONTRAST(0);
}


static char	error_message[128] = "\0";			// エラーメッセージ

/****************************************
    エラー設定
		引数	err = エラーメッセージ
 ****************************************/
void	set_error(const char* mes, ...)
{
	va_list		args;

	va_start(args, mes);
	vsprintf(error_message, mes, args);
	va_end(args);
}

#define stackSize 1000
char EGB_stack[ stackSize ];

volatile uint16_t	vsync_cnt = 0;			// 垂直同期カウンタ
#define VSYNCclear 0x05ca
#define VSYNCintNumber 11

void VSYNChandler( void )
{
	vsync_cnt++;

	/******** ＶＳＹＮＣ割り込み原因クリアレジスタへの書き込み ********/
	_outb( VSYNCclear, 0 );
}


/************
    メイン
 ************/
int	main(int argc, char* argv[])
{

	MOS_start( mwork, MosWorkSize);
	MOS_resolution( 0, 3 );
	MOS_disp( 0 );

	graphics_init();

	sprite_init();				// スプライト管理初期化

	sound_init();				// サウンド管理初期化

	struct _timeb time_t;
	_ftime(&time_t);
	srand((unsigned int)time_t.time);		// 乱数初期化

	// 垂直同期割り込み開始
	HIS_stackArea( EGB_stack , stackSize );
	HIS_setHandler( VSYNCintNumber , VSYNChandler );
	HIS_enableInterrupt( VSYNCintNumber );

	if ( game_init((argc < 2) ? NULL : argv[1]) ) {		// ゲーム初期化
		while ( game_update() ) {		// ゲーム稼働
			rand();
			vsync_cnt = 0;
			while ( vsync_cnt < 1 ) ;	// 垂直同期

			sprite_clear();				// スプライトクリア
			game_draw();				// ゲーム描画
		}
	}

	sprite_quit();					// スプライト管理終了
	graphics_quit();
	sound_quit();					// サウンド管理終了
	clear_text();					// テキスト画面クリア
	MOS_end();					// マウス初期化

	HIS_detachHandler( VSYNCintNumber );		// 垂直同期割込終了

	if ( error_message[0] ) {			// エラー
		puts(error_message);
		return	1;
	}
	return	0;
}
