/*****************************

		サウンド管理

 *****************************/

#include "sound.h"
#include "app.h"

char	swork[16384];

#define PCM_CH		71

/*** サウンドデータ *******/
typedef struct
{
	int flen;
	int tempo;
	int signature;
	char mute[32];
	char port[32];
	char midi_ch[32];
	char bias[32];
	char transpose[32];
	char FM_ch_assign[6];
	char PCM_ch_assign[8];
	char fm_bankpath[128];
	char pcm_bankpath[128];
	char fm_bankname[8];
	char pcm_bankname[8];
	char *eup_buf;
} EUP_Data;

typedef struct
{
	int note;
	char *buffer;
} PCM_Data;

static EUP_Data*	sound_data = NULL;			// サウンドデータ
static PCM_Data*	se_data = NULL;
static bool		init_flag = false;			// 初期化フラグ


/************
    初期化
 ************/
void	sound_init(void)
{
	SND_init(swork);
	SND_elevol_set(0,127,127);
	SND_elevol_mute(0x0f);
	SND_pcm_mode_set ( 2 );

	SND_eup_init();
	SND_rs_midi_init();

	init_flag = true;
	sound_data = NULL;
}

/**********
    終了
 **********/
void	sound_quit(void)
{
	if ( init_flag ) {
		SND_rs_midi_end();
		SND_eup_end();
		SND_end();

		if ( sound_data ) {					// サウンドデータ解放
			EUP_Data*	p = sound_data;
			while ( p->eup_buf ) {
				free(p->eup_buf);
				p++;
			}
			free(sound_data);
		}

		if(se_data)	{
			PCM_Data*	p = se_data;
			while ( p->buffer ) {
				free(p->buffer);
				p++;
			}
			free(se_data);
		}
	}
}


/*************************************
    PCMデータ読み込み
		引数	_file = SNDファイル
 *************************************/
bool	load_pcm(char const** _file)
{
	int i, flen;
	FILE *fp;

	if ( !init_flag ) {
		return	true;
	}

	int	n = 0;
	{
		char const**	p = _file;

		while ( *p++ ) {
			n++;							// データ数
		}
		se_data = malloc(sizeof(PCM_Data)*(n + 1));
	}

	for (i = 0; i < n + 1; i++) {
		se_data[i].buffer = NULL;
	}

	for (i = 0; i < n; i++) {
		if ( (fp = fopen(_file[i], "rb")) == NULL ) {
			set_error("サウンドファイル \"%s\" が開けません\n", _file[i]);
			return	false;
		}

		fseek(fp,0,SEEK_END);					/*	ﾌｧｲﾙの最後にﾎﾟｲﾝﾀを移動	*/		flen = (unsigned int)ftell( fp );
		fseek(fp, 0, SEEK_SET);

		if(( se_data[i].buffer = malloc(flen) ) == 0 ) {
			fclose(fp);
			set_error("サウンドファイル \"%s\" が開けません\n", _file[i]);
			return false;
		}

		memset(se_data[i].buffer, 0, flen);
		fread(se_data[i].buffer, 1, flen, fp);

		se_data[i].note = ( char )se_data[i].buffer[ 28 ];

		fclose(fp);
	}

	return	true;
}

/*****************************************
    サウンドデータ読み込み
			引数	_file = EUPファイル
			戻り値	読み込み成功か
 *****************************************/
bool	load_sound(char const** _file)
{
	int i, j, k, size;
	FILE *fp;
	char buffer[8];

	if ( !init_flag ) {
		return	true;
	}

	int	n = 0;
	{
		char const**	p = _file;

		while ( *p++ ) {
			n++;							// データ数
		}
		sound_data = malloc(sizeof(EUP_Data)*(n + 1));
	}

	for (i = 0; i < n + 1; i++) {
		sound_data[i].eup_buf = NULL;
	}

	for (i = 0; i < n; i++) {
		if ( (fp = fopen(_file[i], "rb")) == NULL ) {
			set_error("サウンドファイル \"%s\" が開けません\n", _file[i]);
			return	false;
		}

		fseek(fp,0,SEEK_END);					/*	ﾌｧｲﾙの最後にﾎﾟｲﾝﾀを移動	*/
		sound_data[i].flen = (unsigned int)ftell(fp);		/*  EUPﾌｧｲﾙ全体のﾊﾞｲﾄ数を取得	*/
		fseek(fp,0,SEEK_SET);					/*	ﾌｧｲﾙの先頭にﾎﾟｲﾝﾀを戻す	*/
		if (sound_data[i].flen <= 2048+4+1+1)	{		/*  全体の大きさはﾍｯﾀﾞ長以下か?	*/
			fclose(fp);
			set_error("サウンドファイル \"%s\" が開けません\n", _file[i]);
			return false;
		}

									/*  以下全32ﾄﾗｯｸについて設定	*/
		fseek(fp,852,SEEK_SET);
		fread(sound_data[i].mute,1,32,fp);			/*  ミュートの設定		*/
		fread(sound_data[i].port,1,32,fp);			/*  出力ポートの設定		*/
		fread(sound_data[i].midi_ch,1,32,fp);			/*  出力MIDIチャンネルの設定	*/
		fread(sound_data[i].bias,1,32,fp);			/*  キーバイアスの設定		*/
		fread(sound_data[i].transpose,1,32,fp);			/*  トランスポーズの設定	*/
		fseek(fp,1748,SEEK_SET);
		fread(sound_data[i].FM_ch_assign,1,6,fp);		/*  FM音源のMIDIチャンネル設定	*/
		fread(sound_data[i].PCM_ch_assign,1,8,fp);		/*  PCM音源のMIDIチャンネル設定 */

		//sound_data[i].fm_bankpath = 0;

		for(j = 0;_file[i][j] != '\0';j++) {			/*	EUPﾌｧｲﾙのﾊﾟｽ名を得る	*/
			sound_data[i].fm_bankpath[j] = _file[i][j];
			sound_data[i].pcm_bankpath[j] = _file[i][j];
		}

		while((sound_data[i].fm_bankpath[j] != '\\') && (sound_data[i].fm_bankpath[j] != ':') && (j >= 0)) {
			sound_data[i].fm_bankpath[j] = '\0';
			sound_data[i].pcm_bankpath[j] = '\0';
			j--;
		}
		j++;

		fread(&sound_data[i].fm_bankpath[j],1,8,fp);		/*FM音源のﾃﾞｰﾀﾌｧｲﾙ名を読み込む*/
		if(sound_data[i].fm_bankpath[j] != '\0')
		{
			for(k = j;sound_data[i].fm_bankpath[k] != '\0';k++);
			sound_data[i].fm_bankpath[k  ] = '.';
			sound_data[i].fm_bankpath[k+1] = 'f';
			sound_data[i].fm_bankpath[k+2] = 'm';
			sound_data[i].fm_bankpath[k+3] = 'b';
			sound_data[i].fm_bankpath[k+4] = '\0';
		}


		fread(&sound_data[i].pcm_bankpath[j],1,8,fp);		/*PCM音源のﾃﾞｰﾀﾌｧｲﾙ名を読み込む*/
		if(sound_data[i].pcm_bankpath[j] != '\0')
		{
			for(k = j;sound_data[i].pcm_bankpath[k] != '\0';k++);
			sound_data[i].pcm_bankpath[k  ] = '.';
			sound_data[i].pcm_bankpath[k+1] = 'p';
			sound_data[i].pcm_bankpath[k+2] = 'm';
			sound_data[i].pcm_bankpath[k+3] = 'b';
			sound_data[i].pcm_bankpath[k+4] = '\0';
		}

		fseek(fp,2048,SEEK_SET);				/*  演奏ﾃﾞ-ﾀの先頭情報部に移動	*/
		fread(&size,4,1,fp);					/*　演奏ﾃﾞｰﾀの大きさを読み込む	*/
		fread(buffer,1,2,fp);					/*　2つ分のﾃﾞｰﾀを読み込む	*/
		sound_data[i].signature = (int)buffer[0];
		sound_data[i].tempo = (int)buffer[1];

		sound_data[i].flen -= 2048+4+1+1;                 	/*  ファイル長からヘッダ長を除く*/
		if ((sound_data[i].eup_buf = malloc(sound_data[i].flen)) == 0) { 	/*  演奏ﾃﾞｰﾀの読み込み領域を確保*/
			fclose( fp ) ;
			set_error("サウンドファイル \"%s\" が開けません\n", _file[i]);
			return false;
		}
		fread(sound_data[i].eup_buf, 1, sound_data[i].flen, fp);/*  演奏データを読み込む	*/

		fclose( fp );
	}
	return	true;
}

/**********************************
    BGM再生
		引数	n = サウンド番号
 **********************************/
void	play_bgm(int n)
{
	int i;

	if ( init_flag ) {
		for(i=0 ; i<32 ;i++) {
			SND_eup_mute_set(i,(int)sound_data[n].mute[i]);	/*  ミュートの設定		*/
			SND_eup_port_set(i,(int)sound_data[n].port[i]);	/*  出力ポートの設定		*/
			SND_eup_midi_ch_set(i,(int)sound_data[n].midi_ch[i]);
									/*  出力MIDIチャンネルの設定	*/
			SND_eup_bias_set(i,(int)sound_data[n].bias[i]);	/*  キーバイアスの設定		*/
			SND_eup_transpose_set(i,(int)sound_data[n].transpose[i]);
									/*  トランスポーズの設定	*/
		}

		for(i = 0;i < 6;i++) {
			SND_midi_ch_assign(i,(int)sound_data[n].FM_ch_assign[i]);
									/*  FM音源のMIDIチャンネル設定	*/
		}
		for(i = 0;i < 8;i++) {
			SND_midi_ch_assign(i+64,(int)sound_data[n].PCM_ch_assign[i]);
									/*  PCM音源のMIDIチャンネル設定 */
		}

		//SND_fm_bank_load(sound_data[n].bankpath,sound_data[n].fm_bankname);
		SND_fm_bank_load(sound_data[n].fm_bankpath,sound_data[n].fm_bankname);
		SND_eup_tempo_set(sound_data[n].tempo);			/*  曲始めのテンポをセット	*/
		SND_eup_play_start(sound_data[n].eup_buf,sound_data[n].flen,sound_data[n].signature);
	}


}

/************
   BGM停止
 ************/
void	stop_bgm(void)
{
	if ( init_flag ) {
		SND_eup_play_stop();
	}
}

/***********************************
   BGMフェードアウト
		引数	_speed = スピード
 ***********************************/
void	fadeout_bgm(int _speed)
{
	/*if ( init_flag ) {
		m_fadeout(_speed);
	}*/
}

/**********************************
    SE再生
		引数	n = サウンド番号
 **********************************/
void	play_se(int n)
{
	static int c = 0;

	if ( init_flag ) {
		//SND_pcm_play( PCM_CH - (n && 7) , se_data[n].note , 127 , se_data[n].buffer ) ;
		SND_pcm_play( PCM_CH - c , se_data[n].note , 127 , se_data[n].buffer ) ;
	}

	c = 1 - c;
}
