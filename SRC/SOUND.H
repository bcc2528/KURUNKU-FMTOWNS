#ifndef	___SOUND_H___
#define	___SOUND_H___

#include "common.h"
#include <snd.h>

void	sound_init(void);				// 初期化
void	sound_quit(void);				// 終了
bool	load_pcm(char const**);				// PCMデータ読み込み
bool	load_sound(char const**);			// サウンドデータ読み込み
void	play_bgm(int);					// BGM再生
void	stop_bgm(void);					// BGM停止
void	fadeout_bgm(int);				// BGMフェードアウト
void	play_se(int);					// SE再生

#endif
