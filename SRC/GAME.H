#ifndef	___GAME_H___
#define	___GAME_H___

#include "common.h"


#define	FIELD_W		6				// �t�B�[���h�̑傫��
#define	FIELD_H		7
#define	BALL_W		20				// ���̑傫��
#define	BALL_H		34

#define	COLOR_MAX	5				// �F��


// BG�ԍ�
enum
{
	BG_SPACE	= 0,							// ��
	BG_NUMBER,									// ����
};

// �X�v���C�g�ԍ�
enum
{
	SPR_BALL	= 11,	// ��
	SPR_PIVOT	= SPR_BALL + 4,					// ��]��
	SPR_CURSOR	= SPR_PIVOT + 3,				// �J�[�\��
	SPR_EFFECT,									// ���Z�G�t�F�N�g
	SPR_START	= SPR_EFFECT + 20,				// "CLICK TO START"
	SPR_OVER	= SPR_START + 6,				// "GAME OVER"
};

// �p���b�g�ԍ�
enum
{
	PAL_CURSOR,									// �J�[�\��
	PAL_NUMBER,									// ����
	PAL_BALL,									// ��
	PAL_START	= (PAL_BALL + COLOR_MAX*2) +1,	// "CLICK TO START"
	PAL_OVER,									// "GAME OVER"
	PAL_EFFECT	= PAL_NUMBER,					// ���Z�G�t�F�N�g
	PAL_PIVOT	= PAL_OVER + 2					// ��]��
};


bool	game_init(char*);			// ������
bool	game_update(void);			// �ғ�
void	game_draw(void);			// �`��

#endif