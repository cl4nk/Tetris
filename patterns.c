#include <stdio.h>
#include <stdlib.h>
#include "info.h"

#define U __attribute__((unused))

static int U colors[14] = {
		0xf44336, 0xffeb3b, 0x4caf50, 0x2196f3, 0x673ab7, 0x9c27b0, 0xff5722, 0xe91e63, 0x009688, 0x00bcd4, 0x795548,
		0x424242, 0xeeeeee, 0x33B5E5
};
static int U colors_light[14] = {
		0xffcdd2, 0xfff9c4, 0xc8e6c9, 0xbbdefb, 0xd1c4e9, 0xe1bee7, 0xffccbc, 0xf8bbd0, 0xb2dfdb, 0xb2ebf2, 0xd7ccc8,
		0x616161, 0xfafafa, 0x00ddff
};
static int U colors_dark[14] = {
		0xd32f2f, 0xfbc02d, 0x388e3c, 0x1976d2, 0x512da8, 0x7b1fa2, 0xe64a19, 0xc2185b, 0x00796b, 0x0097a7, 0x5d4037,
		0x212121, 0xbdbdbd, 0x0099cc
};

static int U colors_background[NBR_SKINS] = { 0x283593, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, 0x323232, COLOR_WHITE };
static int U colors_font[NBR_SKINS] = { 0xff9100, COLOR_WHITE, COLOR_BLACK, 0x2196f3, 0x33B5E5, 0x8B00FF };
static int U colors_line[NBR_SKINS] = { COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, 0xF44336, 0x0099CC, 0xFF0000 };
static int U colors_block_index[NBR_SKINS] = { -1, COLOR_INDEX_WHITE_BLOCK, -1, -1, COLOR_INDEX_HOLO_BLOCK, -1 };

static char * U skin_ressources_folder[NBR_SKINS] = { "", "", "MARIO/", "DRAGON_BALL/", "", "UNICORN/" };

static char * U skins_labels[NBR_SKINS] = { "Classic", "Black", "Mario", "Dragon Ball", "Holo Dark", "Unicorn" };
static char * U modes_labels[NBR_MODES] = { "Classic", "Bad Blocks", "From Below", "Strange Blocks" };

static Configs const type1_configs = (Configs) {
	.patterns = {
		{

			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{1, 1, 1, 1},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{1, 1, 1, 1},
			{0, 0, 0, 0}
		}
	}
};

static Configs const type2_configs = (Configs) {
	.patterns = {
		{
			{0, 0, 0, 0},
			{0, 1, 1, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 1, 1, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 1, 1, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 1, 1, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0}
		}
	}
};

static Configs const type3_configs = (Configs) {
	.patterns = {
		{
			{0, 0, 0, 0},
			{1, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{1, 1, 1, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{1, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}

		}
	}
};

static Configs const type4_configs = (Configs) {
	.patterns = {
		{
			{0, 0, 0, 0},
			{0, 1, 1, 1},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 1, 0},
			{0, 0, 1, 0},
			{0, 0, 1, 1},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 1},
			{0, 1, 1, 1},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 1, 0},
			{0, 0, 1, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 0}
		}
	}
};
static Configs const type5_configs = (Configs) {
	.patterns = {
		{
			{0, 0, 1, 0},
			{0, 0, 1, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{0, 1, 1, 1},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 1, 1},
			{0, 0, 1, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 1, 1, 1},
			{0, 0, 0, 1},
			{0, 0, 0, 0}
		}
	}
};
static Configs const type6_configs = (Configs) {
	.patterns = {
		{
			{0, 0, 1, 0},
			{0, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{1, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 1, 0},
			{0, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{1, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0}
		}
	}
};
static Configs const type7_configs = (Configs) {
	.patterns = {
		{
			{0, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 1, 1},
			{0, 1, 1, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 1, 1},
			{0, 1, 1, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		}
	}
};

static Configs const type8_configs = (Configs) {
	.patterns = {
		{
			{0, 1, 0, 0},
			{1, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0}
		},
		{
			{0, 0, 1, 0},
			{1, 1, 1, 1},
			{0, 0, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 1, 0},
			{0, 0, 1, 0},
			{0, 1, 1, 1},
			{0, 0, 1, 0}
		},
		{
			{0, 1, 0, 0},
			{1, 1, 1, 1},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		}
	}
};

static Configs const type9_configs = (Configs) {
	.patterns = {
		{
			{1, 1, 1, 0},
			{1, 1, 1, 0},
			{1, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{1, 1, 1, 0},
			{1, 1, 1, 0},
			{1, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{1, 1, 1, 0},
			{1, 1, 1, 0},
			{1, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{1, 1, 1, 0},
			{1, 1, 1, 0},
			{1, 1, 1, 0},
			{0, 0, 0, 0}
		}
	}
};

static Configs const type10_configs = (Configs) {
	.patterns = {
		{
			{0, 1, 0, 0},
			{1, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{1, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{1, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{1, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		}
	}
};

static Configs const type11_configs = (Configs) {
	.patterns = {
		{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		},
		{
			{0, 0, 0, 1},
			{0, 0, 1, 0},
			{0, 1, 0, 0},
			{1, 0, 0, 0}
		},
		{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		},
		{
			{0, 0, 0, 1},
			{0, 0, 1, 0},
			{0, 1, 0, 0},
			{1, 0, 0, 0}
		}
	}
};
