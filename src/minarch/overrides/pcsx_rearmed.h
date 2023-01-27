#include "overrides.h"

static CoreOverrides pcsx_rearmed_overrides = {
	.core_name = "pcsx_rearmed",
	.button_mapping = (ButtonMapping[]){
		{"UP",			RETRO_DEVICE_ID_JOYPAD_UP,		BTN_ID_UP},
		{"DOWN",		RETRO_DEVICE_ID_JOYPAD_DOWN,	BTN_ID_DOWN},
		{"LEFT",		RETRO_DEVICE_ID_JOYPAD_LEFT,	BTN_ID_LEFT},
		{"RIGHT",		RETRO_DEVICE_ID_JOYPAD_RIGHT,	BTN_ID_RIGHT},
		{"SELECT",		RETRO_DEVICE_ID_JOYPAD_SELECT,	BTN_ID_SELECT},
		{"START",		RETRO_DEVICE_ID_JOYPAD_START,	BTN_ID_START},
		{"CIRCLE",		RETRO_DEVICE_ID_JOYPAD_A,		BTN_ID_A},
		{"CROSS",		RETRO_DEVICE_ID_JOYPAD_B,		BTN_ID_B},
		{"TRIANGLE",	RETRO_DEVICE_ID_JOYPAD_X,		BTN_ID_X},
		{"SQUARE",		RETRO_DEVICE_ID_JOYPAD_Y,		BTN_ID_Y},
		{"L1 BUTTON",	RETRO_DEVICE_ID_JOYPAD_L,		BTN_ID_L1},
		{"R1 BUTTON",	RETRO_DEVICE_ID_JOYPAD_R,		BTN_ID_R1},
		{"L2 BUTTON",	RETRO_DEVICE_ID_JOYPAD_L2,		BTN_ID_L2},
		{"R2 BUTTON",	RETRO_DEVICE_ID_JOYPAD_R2,		BTN_ID_R2},
		{NULL,0,0},
	},
};