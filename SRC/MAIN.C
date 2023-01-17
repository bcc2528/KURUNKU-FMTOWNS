/**************************

		���C��

 **************************/

#include "common.h"
#include "app.h"
#include "graphics.h"
#include "sprite.h"
#include "sound.h"
#include "game.h"

char	mwork[MosWorkSize];

/******************
    �t�F�[�h�C��
 ******************/
void	fade_in(void)
{
	//CONTRAST(-2);
}

/********************
    �t�F�[�h�A�E�g
 ********************/
void	fade_out(void)
{
	//CONTRAST(0);
}


static char	error_message[128] = "\0";			// �G���[���b�Z�[�W

/****************************************
    �G���[�ݒ�
		����	err = �G���[���b�Z�[�W
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

volatile uint16_t	vsync_cnt = 0;			// ���������J�E���^
#define VSYNCclear 0x05ca
#define VSYNCintNumber 11

void VSYNChandler( void )
{
	vsync_cnt++;

	/******** �u�r�x�m�b���荞�݌����N���A���W�X�^�ւ̏������� ********/
	_outb( VSYNCclear, 0 );
}


/************
    ���C��
 ************/
int	main(int argc, char* argv[])
{

	MOS_start( mwork, MosWorkSize);
	MOS_resolution( 0, 3 );
	MOS_disp( 0 );

	graphics_init();

	sprite_init();				// �X�v���C�g�Ǘ�������

	sound_init();				// �T�E���h�Ǘ�������

	struct _timeb time_t;
	_ftime(&time_t);
	srand((unsigned int)time_t.time);		// ����������

	// �����������荞�݊J�n
	HIS_stackArea( EGB_stack , stackSize );
	HIS_setHandler( VSYNCintNumber , VSYNChandler );
	HIS_enableInterrupt( VSYNCintNumber );

	if ( game_init((argc < 2) ? NULL : argv[1]) ) {		// �Q�[��������
		while ( game_update() ) {		// �Q�[���ғ�
			rand();
			vsync_cnt = 0;
			while ( vsync_cnt < 1 ) ;	// ��������

			sprite_clear();				// �X�v���C�g�N���A
			game_draw();				// �Q�[���`��
		}
	}

	sprite_quit();					// �X�v���C�g�Ǘ��I��
	graphics_quit();
	sound_quit();					// �T�E���h�Ǘ��I��
	clear_text();					// �e�L�X�g��ʃN���A
	MOS_end();					// �}�E�X������

	HIS_detachHandler( VSYNCintNumber );		// �������������I��

	if ( error_message[0] ) {			// �G���[
		puts(error_message);
		return	1;
	}
	return	0;
}
