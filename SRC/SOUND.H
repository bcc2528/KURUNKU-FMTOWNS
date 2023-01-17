#ifndef	___SOUND_H___
#define	___SOUND_H___

#include "common.h"
#include <snd.h>

void	sound_init(void);				// ������
void	sound_quit(void);				// �I��
bool	load_pcm(char const**);				// PCM�f�[�^�ǂݍ���
bool	load_sound(char const**);			// �T�E���h�f�[�^�ǂݍ���
void	play_bgm(int);					// BGM�Đ�
void	stop_bgm(void);					// BGM��~
void	fadeout_bgm(int);				// BGM�t�F�[�h�A�E�g
void	play_se(int);					// SE�Đ�

#endif
