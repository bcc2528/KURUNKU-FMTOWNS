#include "app.h"
#include "sprite.h"

static int sprite_number;

/***************************************************
    �X�v���C�g�ǂݍ���
			����	sp_name  = �X�v���C�g�t�@�C��
					pal_name = �p���b�g�t�@�C��
			�߂�l	����������
 ***************************************************/
bool	load_sprite(const char* sp_name, const char* sp_32k_name, const char* pal_name)
{
	int i;
	uint8_t*	buf;

	if ( (buf = malloc(0xffff)) == NULL ) {
		set_error("������������܂���\n");
		return	false;
	}

	FILE*	fp;
	size_t	size;

	if ( (fp = fopen(sp_name, "rb")) == NULL ) {
		set_error("16 colors �p�^�[���t�@�C�� \"%s\" ���J���܂���\n", sp_name);
		free(buf);
		return	false;
	}
	size = fread(buf, 0x80, 0x100, fp);
	fclose(fp);

	for (i = 0; i < size; i++) {
		SPR_define(0, 128 + i, 1, 1, (char *)buf + (i * 128));		// �X�v���C�g��`
	}

	if ( (fp = fopen(sp_32k_name, "rb")) == NULL ) {
		set_error("32k colors �p�^�[���t�@�C�� \"%s\" ���J���܂���\n", sp_32k_name);
		free(buf);
		return	false;
	}
	size = fread(buf, 0x200, 0x2f, fp);
	fclose(fp);

	for (i = 0; i < size; i++) {
		SPR_define(1, 576 + (i * 4), 1, 1, (char *)buf + (i * 512));		// �X�v���C�g��`
	}

	if ( (fp = fopen(pal_name, "rb")) == NULL ) {
		set_error("�p���b�g�t�@�C�� \"%s\" ���J���܂���\n", pal_name);
		free(buf);
		return	false;
	}
	size = fread(buf, sizeof(uint16_t)*0x10, 0x1f, fp);
	fclose(fp);

	for (i = 0; i < size; i++) {
		SPR_setPaletteBlock(256 + i, 1, (char *)buf + (i * 32));	// �p���b�g��`
	}

	free(buf);
	return	true;
}

// �X�v���C�g�ݒ�
void set_sprite(int16_t x, int16_t y, uint16_t p, uint16_t c, bool s)
{
	_Far unsigned short *sprram;
	_FP_SEG(sprram) = 0x130;
	_FP_OFF(sprram) = 0x0;

	if (sprite_number >= 1024)
	{
		return;
	}

	int sprram_offset = (1023 - sprite_number) << 2;

	sprram[sprram_offset++] = x;
	sprram[sprram_offset++] = y;
	sprram[sprram_offset++] = ((p + 128) & 0x3ff);
	sprram[sprram_offset] = 0x8000 + ((c + 256) & 0xfff);
	//sprram[sprram_offset] = 0x8000 + ((s & 1) << 14) + ((c + 256) & 0xfff);

	sprite_number++;
}

// �X�v���C�g�ݒ�
void set_sprite_32k(int16_t x, int16_t y, uint16_t p, bool s)
{
	_Far unsigned short *sprram;
	_FP_SEG(sprram) = 0x130;
	_FP_OFF(sprram) = 0x0;

	if (sprite_number >= 1024)
	{
		return;
	}

	int sprram_offset = (1023 - sprite_number) << 2;

	sprram[sprram_offset++] = x;
	sprram[sprram_offset++] = y;
	sprram[sprram_offset++] = ((p + 576) & 0x3ff);
	sprram[sprram_offset] = 0x0;
	//sprram[sprram_offset++] = 0x8000 + ((s & 1) << 14) + ((c + 256) & 0xfff);
	sprite_number++;
}

void set_sprite_32k_2(int16_t x, int16_t y, uint16_t p, bool s)
{
	_Far unsigned short *sprram;
	_FP_SEG(sprram) = 0x130;
	_FP_OFF(sprram) = 0x0;

	if (sprite_number >= 1023)
	{
		return;
	}

	int sprram_offset = ((1023 - sprite_number) << 2) + 3;

	sprram[sprram_offset--] = 0x0;
	sprram[sprram_offset--] = ((p + 576) & 0x3ff);
	sprram[sprram_offset--] = y;
	sprram[sprram_offset--] = x;
	sprram[sprram_offset--] = 0x0;
	sprram[sprram_offset--] = ((p + 580) & 0x3ff);
	sprram[sprram_offset--] = y;
	sprram[sprram_offset] = x + 16;

	sprite_number += 2;
}

void set_sprite_4(int16_t x, int16_t y, uint16_t p, uint16_t c, bool s)
{
	set_sprite(x, y, p, c, s);
	set_sprite(x + 16, y, p + 1, c, s);
	set_sprite(x, y + 16, p + 2, c, s);
	set_sprite(x + 16, y + 16, p + 3, c, s);
}

void set_sprite_4_32k(int16_t x, int16_t y, uint16_t p, bool s)
{
	set_sprite_32k_2(x, y, p, s);

	y += 16;
	if(y >= 238)
	{
		return;
	}
	set_sprite_32k_2(x, y, p + 80, s);
}

// �I�[����
void sprite_terminate(void)
{
	_Far unsigned short *sprram;
	_FP_SEG(sprram) = 0x130;
	_FP_OFF(sprram) = 0x0;

	int sprram_offset = 0;

	for(int i = 0; i < 1024;i++)
	{
		sprram[sprram_offset++] = 0;
		sprram[sprram_offset++] = 0;
		sprram[sprram_offset++] = 0;
		sprram[sprram_offset++] = 0x2000;
	}
}

// �X�v���C�g�o�b�t�@�N���A
void sprite_clear(void)
{

	SPR_display( 1, 224);

	sprite_number = 0;
}

// �X�v���C�g�Ǘ�������
void sprite_init(void)
{
	SPR_init();
	SPR_setOffset(0, 0);
	sprite_number = 0;
}

void sprite_quit(void)
{
	SPR_init();
	SPR_display(0, 1024);
	SPR_setOffset(0, 0);
}