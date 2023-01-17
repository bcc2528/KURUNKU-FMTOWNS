/*****************************

		�Q�[�����C��

 *****************************/

#include "game.h"
#include "app.h"
#include "graphics.h"
#include "sprite.h"
#include "ball.h"
#include "number.h"
#include "effect.h"
#include "sound.h"
#include "tifflib.h"
#include <math.h>

FILE *fp;

#define	FIELD_X		7			// �t�B�[���h�ʒu
#define	FIELD_Y		1
#define	HI_SCORE_X	169			// �n�C�X�R�A�\���ʒu
#define	HI_SCORE_Y	104
#define	SCORE_X		169			// �X�R�A�\���ʒu
#define	SCORE_Y		152
#define	COMBO_X		209			// �R���{�\���ʒu
#define	COMBO_Y		183
#define	TIMER_X		137			// �^�C�}�[�o�[�`��ʒu
#define	TIMER_Y		216
#define	TIMER_W		112
#define	TIMER_H		12
#define	GAUGE_X		151			// �R���{�Q�[�W�`��ʒu
#define	GAUGE_Y		200
#define	GAUGE_W		64
#define	GAUGE_H		5

#define	EFFECT_MAX	8			// �G�t�F�N�g��

#define	FPS		60			// �b�ԃt���[����

#define LOADBUFSIZE	150
#define EXPBUFSIZE	10


/*** ��� *******/
enum
{
	PHASE_INIT,			// ������
	PHASE_START,		// �J�n�҂�
	PHASE_GAME,			// �Q�[����
	PHASE_OVER,			// �Q�[���I�[�o�[
};


static Ball		ball[FIELD_W][FIELD_H];					// ��
#define FIELD_MAX	FIELD_W*FIELD_H
static Pivot	pivot[FIELD_W - 1][FIELD_H - 1];		// ��]��
#define PIVOT_MAX	(FIELD_W-1)*(FIELD_H-1)

static int			score;					// �X�R�A
static Number		bg_score;				// �X�R�A�\��
static int			hi_score;				// �n�C�X�R�A
static Number		bg_hi_score;			// �n�C�X�R�A�\��
static int			combo;					// �R���{��
static int			dec_combo;
static Number		bg_combo;				// �R���{�\��
static int			gauge_len;				// �R���{�Q�[�W�̒���
static uint16_t		forbidden_color;		// ��I��F

static int			timer;					// �c�莞��
static int			timer_len;				// �c�莞�ԃo�[�̒���

static Effect		effect[EFFECT_MAX];		// ���Z�G�t�F�N�g
static int			effect_num;

static uint16_t		warning_color[FPS*2];	// �x���J���[
static int16_t		sin_table[128];			// �T�C���e�[�u��

static uint16_t		ms_button;				// �}�E�X�{�^���̏��
static int			ms_x;
static int			ms_y;
static int			push_x;					// �A���v�b�V������p
static int			push_y;

static int			phase;					// ���
static int			cnt;					// �ėp�J�E���^
static int			exit_cnt;				// �I���J�E���^


/*** �T�E���h�f�[�^ *******/
enum
{
	BGM_TITLE,			// �^�C�g��BGM
	BGM_GAME,			// �Q�[����BGM
};

enum
{
	SE_ROTATE,			// ��]
	SE_ERASE,			// ����
};

static const
char*	EUP_name[] =
{
	"SOUNDS\\AMS2RAM2.EUP",
	"SOUNDS\\PSGD_95.EUP",
	NULL
};

static const
char*	SND_name[] =
{
	"SOUNDS\\ROTATE.SND",
	"SOUNDS\\ERASE.SND",
	NULL
};


#define	SAVE_FILE	"KURUNKU.SAV"			// �Z�[�u�t�@�C��
static char		save_name[0x100];			// �t���p�X�Z�[�u�t�@�C����



static bool		init_sound(void);			// �T�E���h������
static void		load_play_data(char*);		// �v���C�f�[�^�ǂݍ���
static void		save_play_data(void);		// �v���C�f�[�^��������

/*
	TIFF���C�u�����̒�����Ă΂��֐�
*/
/* �o�b�t�@�Ƀf�[�^�����[�h���� 					*/
/*    �t�@�C�����烍�[�h����ꍇ�� fread�����ł悢	*/
int read_data( char *bp, int size )
{

	fread( bp, 1, size, fp ) ; 
	if( ferror(fp) != 0 ) {
		return -1 ;
	}
	return 0 ;
}

/* �o�b�t�@�Ƀf�[�^�����[�h����B�I�t�Z�b�g�w��t�� */
int read_file( char *bp, int size, int ofs )
{
	int ret ;

	fseek( fp, ofs ,SEEK_SET ) ;
	ret = fread( bp, 1, size, fp ) ;
	if( ferror(fp) != 0 ) {
		ret =  -1 ;
	}
	return ret ;
}

/*******************************************
    ������
		����	_dir = �Z�[�u�f�B���N�g��
		�߂�l	������������
 *******************************************/
bool	game_init(char* _dir)
{
	int i;

	char *lbp, *dbp, *cbuf ;
	int lbsize, dbsize,bpp, d_line, x, y;
	int comp, fill;
	long strip, clut;

	clear_text();								// �e�L�X�g��ʃN���A
	//B_LOCATE(9, 7);
	//puts("Now Loading...");

	zoom_in();

	lbsize = LOADBUFSIZE * 1024;
	dbsize = EXPBUFSIZE * 1024;
	lbp = malloc(lbsize);
	dbp = malloc(dbsize);
	cbuf = malloc(DECOMP_WORK_SIZE);

	if( (fp = fopen( "GRAPHICS\\FRAME.TIF", "rb" )) == NULL ) { 
		set_error("�t�@�C�� \"GRAPHIC\\FRAME.TIF\" ���J���܂���\n");
		return	false;
	}
	fread( lbp, 1, lbsize, fp ) ;	/* �ŏ��̃f�[�^���[�h */

	/* �ǂݍ��ݗp�֐��̓o�^ */
	TIFF_setReadFunc( read_file ) ;
	
	/* �w�b�_�̉�� */
	if( TIFF_getHead( lbp, lbsize ) < 0 ) {
		set_error("�t�@�C�� \"GRAPHIC\\FRAME.TIF\" ���J���܂���\n");
		return	false;
	}
	/* �^�O�̓��e�̃`�F�b�N */
	if( ( bpp = TIFF_checkMode( &x,&y,&comp,&fill,&strip,&clut )) < 0 ) {
		set_error("�t�@�C�� \"GRAPHIC\\FRAME.TIF\" ���J���܂���\n");
		return	false;
	}
	if( bpp != 16 ) {
		set_error("�t�@�C�� \"GRAPHIC\\FRAME.TIF\" ���J���܂���\n");
		return	false;
	}
	if( x != 256 ) {
		set_error("�t�@�C�� \"GRAPHIC\\FRAME.TIF\" ���J���܂���\n");
		return	false;
	}
	if( y != 256 ) {
		set_error("�t�@�C�� \"GRAPHIC\\FRAME.TIF\" ���J���܂���\n");
		return	false;
	}

	TIFF_setLoadFunc( put_data, read_data );

	d_line = dbsize / ((x * bpp + 7) / 8);

	TIFF_loadImage( bpp, x, y, strip,  fill, comp, dbp, x, d_line, cbuf );


	struct FILLPTR	bar = {GAUGE_X, GAUGE_Y, GAUGE_X + GAUGE_W, GAUGE_Y + GAUGE_H - 1, 0};

	FILL(&bar);


	if ( !load_sprite("GRAPHICS\\KURUNKU.PAT", "GRAPHICS\\BALL.32K", "GRAPHICS\\KURUNKU.CTB") ) {		// �X�v���C�g�ǂݍ���
		return	false;
	}

	if ( !init_sound() ) {						// �T�E���h������
		return	false;
	}
	load_play_data(_dir);						// �v���C�f�[�^�ǂݍ���

	// �}�E�X�̈ړ��͈�
	MOS_horizon(FIELD_X, FIELD_X + FIELD_W*BALL_W - 5);
	MOS_vertical(20, 234);

	Ball_init_position();											// ��]���W�쐬

	for (i = 0; i < FPS*2; i++) {								// �x���J���[
		warning_color[i] = RGB(8.5 + 12.0*(1.0 - cos(i*PI/FPS))/2, 8, 8);
	}
	for (i = 0; i < 128; i++) {									// �T�C���e�[�u��
		sin_table[i] = (int16_t)(256.0*sin(i*PI/64) + 0.5);
	}

	Number_init(&bg_score, 7, SCORE_X, SCORE_Y);					// �X�R�A
	Number_init(&bg_hi_score, 7, HI_SCORE_X, HI_SCORE_Y);			// �n�C�X�R�A
	Number_init(&bg_combo, 2, COMBO_X, COMBO_Y);					// �R���{��

	set_zoom(-1);

	free(lbp);
	free(dbp);
	free(cbuf);

	phase = PHASE_INIT;
	return	true;
}


/********************
    �T�E���h������
 ********************/
static
bool	init_sound(void)
{

	if (load_pcm(SND_name) && load_sound(EUP_name) ) {		// �f�[�^�ǂݍ���
		return	true;
	}

	return	false;
}


/***********************************************
    �v���C�f�[�^�ǂݍ���
			����	_dir = �Z�[�u�f�B���N�g��
 ***********************************************/
static
void	load_play_data(char* _dir)
{
	if ( _dir == NULL ) {
		strcpy(save_name, SAVE_FILE);
	}
	else if ( _dir[strlen(_dir)] == '\\' ) {
		sprintf(save_name, "%s"SAVE_FILE, _dir);
	}
	else {
		sprintf(save_name, "%s\\"SAVE_FILE, _dir);
	}

	FILE*	fp;

	hi_score = 0;
	fp = fopen(save_name, "rb");
	if ( fp ) {
		fread(&hi_score, sizeof(hi_score), 1, fp);			// �v���C�f�[�^�ǂݍ���
		fclose(fp);
	}
}

/**************************
    �v���C�f�[�^��������
 **************************/
static
void	save_play_data(void)
{
	FILE*	fp;

	fp = fopen(save_name, "wb");
	if ( fp ) {
		fwrite(&hi_score, sizeof(hi_score), 1, fp);			// �v���C�f�[�^��������
		fclose(fp);
	}
	else {
		set_error("�t�@�C�� \"%s\" ���쐬�ł��܂���\n");	// ���b�Z�[�W�����œr���I���͂��Ȃ�
	}
}


/******************
    �p�Y��������
 ******************/
static
void	init_puzzle(void)
{
	int i, j;

	for (i = 0; i < FIELD_W; i++) {			// ��
		for (j = 0; j < FIELD_H; j++) {
			uint16_t	_t;

			while (1) {
				_t = (uint16_t)(rand() % COLOR_MAX);
				if ( i % 2 == 0 ) {
					if ( i == 0 ) {
						break;
					}
					if ( (_t != ball[i - 1][j].color) || (((j == 0) || (_t != ball[i][j - 1].color)) && ((j == FIELD_H - 1) || (_t != ball[i - 1][j + 1].color))) ) {
						break;
					}
				}
				else {
					if ( j == 0 ) {
						break;
					}
					if ( (_t != ball[i - 1][j - 1].color) || ((_t != ball[i][j - 1].color) && (_t != ball[i - 1][j].color)) ) {
						break;
					}
				}
			}
			Ball_init(&ball[i][j], FIELD_X + 16 + i*BALL_W, FIELD_Y + 16 + i*(BALL_H/2) + (FIELD_H - 1 - i/2 - j)*BALL_H, _t);
		}
	}

	for (i = 0; i < FIELD_W - 1; i++) {		// ��]��
		for (j = 0; j < FIELD_H - 1; j++) {
			Pivot_init(&pivot[i][j], FIELD_X + 8 + 7 + 16 + i*BALL_W, FIELD_Y - BALL_H/2 + 8 + 16 + ((i + 1) & 1)*BALL_H/2 + (FIELD_H - 1 - j)*BALL_H);
		}
	}

	Set_pivot_color(0);

	score		= 0;							// �X�R�A
	Number_set(&bg_score, 0);
	Number_set(&bg_hi_score, hi_score);
	combo		= 0;							// �R���{��
	dec_combo	= 0;
	Number_set(&bg_combo, 0);
	gauge_len	= 0;							// �R���{�Q�[�W�̒���
	push_x		= -1;							// �A���v�b�V������p
	forbidden_color	= COLOR_MAX;				// ��I��F

	for (i = 0; i < EFFECT_MAX; i++) {		// �G�t�F�N�g
		Effect_init(&effect[i]);
	}
	effect_num = 0;

	sprite_terminate();

	timer		= 62*FPS;						// �c�莞��
	timer_len	= TIMER_W;						// �c�莞�ԃo�[�̒���
	{											// �^�C�}�[�o�[
		static
		struct FILLPTR	bar = {TIMER_X, TIMER_Y, TIMER_X + TIMER_W - 1, TIMER_Y, RGB(0, 0, 1)};

		for (i = 0; i < TIMER_H; i++) {
			bar.y1 = bar.y2 = TIMER_Y + i;
			bar.color = RGB(8 - i*4/TIMER_H, 16 - i*8/TIMER_H, 31 - i*16/TIMER_H);
			FILL(&bar);
		}
	}

	ms_button	= 0;							// �}�E�X�{�^���̏��
	cnt			= 0;							// �A�j���[�V�����J�E���^
	exit_cnt	= -1;							// �t�F�[�h�C���\��

	play_bgm(BGM_TITLE);						// �^�C�g��BGM
}


static void		control(void);				// ����
static bool		check_erase(void);			// �����`�F�b�N
static void		check_fall(void);			// �����`�F�b�N
static void		select_color();				// ��I��F
static void		draw_mes_start(void);		// �X�^�[�g���b�Z�[�W�`��
static void		draw_mes_over(void);		// �Q�[���I�[�o�[���b�Z�[�W�`��

/****************************
    �ғ�
		�߂�l	���s�p����
 ****************************/
bool	game_update(void)
{
	int i;

	if ( phase == PHASE_INIT ) {
		ms_x = 0;
		ms_y = 0;
		init_puzzle();							// �p�Y��������
		phase = PHASE_START;
	}else if(phase != PHASE_OVER) {
		set_sprite(ms_x, ms_y, SPR_CURSOR, PAL_CURSOR,  false);		// �J�[�\���\��
	}
	


	if ( timer == 20*FPS ) {					// ��ɍł����Ȃ��F�̋����o�Ȃ��Ȃ�
		select_color();
	}

	bool	_f = false;

	check_erase();								// �����`�F�b�N
	check_fall();								// �����`�F�b�N
	control();									// ����
	{											// ��
		Ball*	_ball = &ball[0][0];

		for (i = FIELD_W*FIELD_H; i > 0; i--) {
			if ( Ball_update(_ball++) ) {
				_f = true;
			}
		}
	}

	{											// �G�t�F�N�g
		Effect*		_effect = &effect[0];

		for (int i = EFFECT_MAX; i > 0; i--) {
			Effect_update(_effect++);
		}
	}

	if ( (timer == 0) && (phase == PHASE_GAME) && !_f ) {
		phase = PHASE_OVER;						// �Q�[���I�[�o�[
		cnt = 0;
		exit_cnt = 0;

		if ( score > hi_score ) {				// �n�C�X�R�A�X�V
			hi_score = score;
			save_play_data();
		}
	}

	if ( !_f && (dec_combo > 0) ) {				// �R���{�Q�[�W���Z
		dec_combo--;
	}
	if ( bg_combo.value < combo ) {				// �R���{�\��
		if ( --bg_combo.cnt == 0 ) {
			bg_combo.value++;
			bg_combo.cnt = 5;
		}
	}
	else {
		bg_combo.value = combo;
	}
	Number_update(&bg_combo);					// �R���{��

	if ( bg_score.cnt > 0 ) {					// �X�R�A�A�b�v
		bg_score.value += (score - bg_score.value + bg_score.cnt - 1)/bg_score.cnt;
		bg_score.cnt--;
	}
	Number_update(&bg_score);					// �X�R�A
	if ( bg_score.value > bg_hi_score.value ) {
		bg_hi_score.value = bg_score.value;		// �n�C�X�R�A�X�V
	}
	Number_update(&bg_hi_score);				// �n�C�X�R�A

	Number_draw(&bg_hi_score);				// �n�C�X�R�A
	Number_draw(&bg_score);					// �X�R�A
	Number_draw(&bg_combo);					// �R���{

	if ( (phase == PHASE_GAME) && (timer > 0) ) {			// �c�莞��
		if ( --timer == 0 ) {
			fadeout_bgm(30);
		}
	}

	static int		_btn;

	switch ( phase ) {
	  case PHASE_START :			// �J�n�҂�
		cnt++;
		{
			MOS_rdpos ( &_btn, &ms_x, &ms_y);

			if ( ms_button == 0 ) {
				draw_mes_start();				// �X�^�[�g���b�Z�[�W�`��
				if ( _btn ) {
					stop_bgm();
				}
			}
			else if ( _btn == 0 ) {
				play_bgm(BGM_GAME);
				phase = PHASE_GAME;				// �Q�[���J�n
			}
			if ( cnt > 30 ) {
				ms_button = _btn;
			}
		}
		break;

	  case PHASE_OVER :				// �Q�[���I�[�o�[
		cnt++;
		draw_mes_over();
		if ( exit_cnt == 0 ) {
			MOS_rdpos ( &_btn, &ms_x, &ms_y);

			if ( (cnt > FPS*3) && (~_btn & ms_button) ) {
				stop_bgm();
				set_zoom(1);
				exit_cnt = 30;
			}
			ms_button = _btn;
		}
		else if ( --exit_cnt == 0 ) {
			phase = PHASE_INIT;
		}
		break;
	}

	{											// ��]��
		Pivot*	_pivot = &pivot[0][0];

		for (i = PIVOT_MAX; i > 0; i--) {
			Pivot_update(_pivot++);
		}
	}

	{
		Ball*	_ball = &ball[0][0];

		for (i = FIELD_MAX; i > 0; i--) {
			Ball_draw(_ball++);
		}
	}

	return	(_kbhit() == 0);			// ESC�L�[
}

/**********
    ����
 **********/
static
void	control(void)
{
	if ( (timer == 0) && (phase != PHASE_START) ) {
		return;
	}

	int		_x, _y, _btn;

	MOS_rdpos ( &_btn, &_x, &_y);

	ms_x = _x;
	ms_y = _y;

	if ( phase != PHASE_GAME ) {
		return;
	}

	uint32_t	_tmp;

	_tmp = _btn & ~ms_button;
	ms_button = _btn;
	if ( _tmp ) {
		_x -= FIELD_X + 16 + 7 - BALL_W/2;
		if ( (_x >= 0) && (_x < BALL_W*(FIELD_W - 1)) ) {
			_x /= BALL_W;

			_y = FIELD_Y + 16 + (FIELD_H - 1)*BALL_H + ((_x + 1) & 1)*BALL_H/2 - _y;
			if ( (_y >= 0) && (_y < BALL_H*(FIELD_H - 1)) ) {
				_y /= BALL_H;

				Ball	*_b0 = &ball[_x][_y + (_x & 1)],
						*_b1 = &ball[_x + 1][_y];

				if ( _b0->erase_cnt | _b1->erase_cnt | (_b1 + 1)->erase_cnt ) {
					return;						// ��]�ł��Ȃ���������
				}

				play_se(SE_ROTATE);				// SE�Đ�

				if ( _tmp & 0x2 ) {			// �E��]
					_tmp = _b0->color;
					Ball_rotate(_b0, 3, _b1->color);
					Ball_rotate(_b1, 1, (_b1 + 1)->color);
					Ball_rotate((_b1 + 1), 5, _tmp);
				}
				else {							// ����]
					_tmp = _b0->color;
					Ball_rotate(_b0, 0, (_b1 + 1)->color);
					Ball_rotate((_b1 + 1), 4, _b1->color);
					Ball_rotate(_b1, 2, _tmp);
				}

				if ( (_x != push_x) || (_y != push_y) ) {
					if ( dec_combo > 0 ) {		// �R���{�Q�[�W���Z
						dec_combo -= 16;
						if ( dec_combo < 0 ) {
							dec_combo = 0;
						}
					}
					else if ( combo > 0 ) {		// �R���{���Z
						combo--;
					}
					push_x = _x;
					push_y = _y;
				}
				pivot[_x][_y].cnt = 5;			// ��]��
			}
		}
	}
}


/*********************************************
    �X�R�A���Z
		����	_x, _y = �G�t�F�N�g�\���ʒu
				_t     = �G�t�F�N�g�f�B���C
		�߂�l	�X�R�A���Z�l
 *********************************************/
static
uint32_t	add_score(int16_t _x, int16_t _y, short _t)
{
	if ( combo < 99 ) {
		combo++;
		bg_combo.cnt = 1;
	}
	dec_combo = dec_combo*4/5 + 36;

	uint32_t	_d = (uint32_t)(combo*10);						// ���Z�_

	Effect_set(&effect[effect_num], combo, _x - 16, _y, _t);			// ���Z�G�t�F�N�g
	effect_num = ++effect_num % EFFECT_MAX;

	return	_d;
}

/************************************
    �����`�F�b�N
		�߂�l	���̏�������������
 ************************************/
static
bool	check_erase(void)
{
	uint32_t	d_score = 0;
	short		d = 0;
	Ball*		b0 = &ball[FIELD_W - 1][FIELD_H - 2];

	for (short i = FIELD_W - 1; i >= 0; i--) {
		for (short j = FIELD_H - 2, k = j + 1 - (i & 1); j >= 0; j--, k--) {
			uint16_t	t = b0->color;

			if ( !(t & 0xe8) && (t == (b0 + 1)->color) ) {		// �󔒁E�ғ���������
				if ( i > 0 ) {
					Ball*	b2 = &ball[i - 1][k];

					if ( (t == b2->color) && (((b0->wait_cnt | (b0 + 1)->wait_cnt | b2->wait_cnt) == 0) || ((b0->erase_cnt | (b0 + 1)->erase_cnt | b2->erase_cnt) > 8)) ) {
						Ball_erase(b0);
						Ball_erase(b0 + 1);
						Ball_erase(b2);
						if ( (b0->erase_cnt | (b0 + 1)->erase_cnt | b2->erase_cnt) == 0xff ) {
							d_score += add_score((int16_t)(FIELD_X - BALL_W*2/5 + 19 + i*BALL_W), (int16_t)(FIELD_Y - BALL_H/2 + 16 + (FIELD_H - 1)*BALL_H + i*(BALL_H/2) - (i/2 + j)*BALL_H), d++);
						}
					}
				}
				if ( i < FIELD_W - 1 ) {
					Ball*	b2 = &ball[i + 1][k];

					if ( (t == b2->color) && (((b0->wait_cnt | (b0 + 1)->wait_cnt | b2->wait_cnt) == 0) || ((b0->erase_cnt | (b0 + 1)->erase_cnt | b2->erase_cnt) > 8)) ) {
						Ball_erase(b0);
						Ball_erase(b0 + 1);
						Ball_erase(b2);
						if ( (b0->erase_cnt | (b0 + 1)->erase_cnt | b2->erase_cnt) == 0xff ) {
							d_score += add_score((int16_t)(FIELD_X + BALL_W*2/5 + 19 + i*BALL_W), (int16_t)(FIELD_Y - BALL_H/2 + 16 + (FIELD_H - 1)*BALL_H + i*(BALL_H/2) - (i/2 + j)*BALL_H), d++);
						}
					}
				}
			}
			b0--;
		}
		b0--;
	}
	if ( d_score > 0 ) {						// ������������
		score += d_score;
		bg_score.cnt = 20;
		play_se(SE_ERASE);						// SE�Đ�
		return	true;
	}
	return	false;
}


/******************
    �����`�F�b�N
 ******************/
static
void	check_fall(void)
{
	Ball*	b = &ball[0][0];

	for (int i = FIELD_W; i > 0; i--) {
		for (int j = FIELD_H - 1; j > 0; j--) {
			if ( ((b->color & 0xe8) == 0x08) && (((b + 1)->color & 0xe8) == 0) && ((b + 1)->erase_cnt == 0) ) {
				Ball_fall(b, (b + 1)->color);
				Ball_set(b + 1, 0x08);
			}
			b++;
		}
		if ( b->color == 0x08 ) {
			uint16_t	t;
			do {
				t = (uint16_t)(rand() % COLOR_MAX);
			} while ( (t == forbidden_color) || (t == ((b - 1)->color & 0x0f)) );
			Ball_fall(b, t);
		}
		b++;
	}
}

/**************
    ��I��F
 **************/
static
void	select_color()
{
	int i;

	short	num[COLOR_MAX] = {0, 0, 0, 0, 0};
	Ball*	_ball = &ball[0][0];				// ��

	for (i = FIELD_W*FIELD_H; i > 0; i--) {
		if ( (_ball->color & 0xf8) == 0 ) {
			num[_ball->color]++;
		}
		_ball++;
	}
	short	t = FIELD_W*FIELD_H;
	for (i = COLOR_MAX - 1; i >= 0; i--) {		// �ł����Ȃ��F
		if ( num[i] < t ) {
			forbidden_color = (uint16_t)i;
			t = num[i];
		}
	}
}


/****************************
    �X�^�[�g���b�Z�[�W�`��
 ****************************/
static
void	draw_mes_start(void)
{
	int16_t		_y = 108 + 16 + sin_table[cnt % 128]/48;

	for (int i = 0; i < 6; i++) {
		set_sprite(20 + i*16, _y, SPR_START + i, PAL_START, true);
	}
}

/**********************************
    �Q�[���I�[�o�[���b�Z�[�W�`��
 **********************************/
static
void	draw_mes_over(void)
{
	static const
	int		pat[] =
			{
				12,			// G
				14,			// A
				13,			// M
				18,			// E
				14,			// O
				12,			// V
				13,			// E
				13,			// R
			};

	int const*	p = pat;
	int16_t		x = 12;

	for (int i = 0; i < 8; i++) {
		int		t = cnt - i*8 - 120;
		set_sprite(x, 116 - ((t < 0) ? t*t/8 : (256 + sin_table[(t + 96) % 128])/32), SPR_OVER + i, PAL_OVER,  false);
		x += *p++;
	}
}


/**********
    �`��
 **********/
void	game_draw(void)
{

	if ( timer < 10*FPS ) {						// �x��
		Set_pivot_color((timer % FPS) / 5);
	}

	int		len = (timer*TIMER_W + 62*FPS - 1)/(62*FPS);		// �^�C�}�[�o�[

	if ( len < timer_len ) {
		static
		struct FILLPTR	bar = {TIMER_X, TIMER_Y, TIMER_X + TIMER_W - 1, TIMER_Y + TIMER_H - 1, 0};

		bar.x1 = TIMER_X + len;
		FILL(&bar);

		timer_len = len;
	}

	len = (dec_combo*GAUGE_W + 180 - 1)/180;					// �R���{�Q�[�W

	if ( len > gauge_len ) {
		static
		struct FILLPTR	bar = {GAUGE_X, GAUGE_Y, GAUGE_X, GAUGE_Y + GAUGE_H - 1, RGB(31, 24, 0)};

		bar.x2 = GAUGE_X + len - 1;
		FILL(&bar);
	}
	else if ( len < gauge_len ) {
		static
		struct FILLPTR	bar = {GAUGE_X, GAUGE_Y, GAUGE_X + GAUGE_W, GAUGE_Y + GAUGE_H - 1, 0};

		bar.x1 = GAUGE_X + gauge_len - 1;
		FILL(&bar);
	}
	gauge_len = len;

	if ( exit_cnt < 0 ) {
		set_zoom(-1);								// �t�F�[�h�C��
		exit_cnt = 0;
	}

	zoom();

}
