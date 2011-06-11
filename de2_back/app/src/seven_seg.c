#include "alt_types.h"
#include "seven_seg.h"

#define SEG_TOP       0x01
#define SEG_TOP_RIGHT 0x02
#define SEG_BOT_RIGHT 0x04
#define SEG_BOT       0x08
#define SEG_BOT_LEFT  0x10
#define SEG_TOP_LEFT  0x20
#define SEG_MID       0x40

static alt_u8 numbers[10] = {
	SEG_MID,                                               // 0
	~(SEG_TOP_RIGHT | SEG_BOT_RIGHT),                      // 1
	SEG_TOP_LEFT | SEG_BOT_RIGHT,                          // 2
	SEG_TOP_LEFT | SEG_BOT_LEFT,                           // 3
	SEG_TOP | SEG_BOT | SEG_BOT_LEFT,                      // 4
	SEG_TOP_RIGHT | SEG_BOT_LEFT,                          // 5
	SEG_TOP_RIGHT,                                         // 6
	SEG_MID | SEG_TOP_LEFT | SEG_BOT_LEFT | SEG_BOT,       // 7
	0x00,                                                  // 8
	SEG_BOT_RIGHT                                          // 9
};
static alt_u8 letters[26] = {
	SEG_BOT, // A
	SEG_TOP | SEG_TOP_RIGHT, // B
	SEG_TOP_RIGHT | SEG_BOT_RIGHT | SEG_MID, // C
	SEG_TOP_LEFT | SEG_TOP, // D
	SEG_TOP_RIGHT | SEG_BOT_RIGHT, // E
	SEG_TOP_RIGHT | SEG_BOT_RIGHT | SEG_BOT, // F
	SEG_TOP_RIGHT | SEG_MID, // G
	SEG_TOP | SEG_BOT, // H
	~(SEG_TOP_LEFT | SEG_BOT_LEFT), // I
	SEG_TOP | SEG_TOP_LEFT | SEG_MID, // J
	SEG_BOT | SEG_TOP_RIGHT, // K
	SEG_TOP | SEG_MID | SEG_TOP_RIGHT | SEG_BOT_RIGHT, // L
	SEG_BOT | SEG_TOP_RIGHT | SEG_TOP_LEFT, // M
	SEG_TOP_LEFT | SEG_TOP | SEG_TOP_RIGHT | SEG_BOT, // N
	SEG_TOP_LEFT | SEG_TOP | SEG_TOP_RIGHT, // O
	SEG_BOT | SEG_BOT_RIGHT, // P
	SEG_BOT | SEG_BOT_LEFT, // Q
	~(SEG_BOT_LEFT | SEG_MID), // R
	SEG_TOP_RIGHT | SEG_BOT_LEFT, // S
	SEG_TOP | SEG_TOP_RIGHT | SEG_BOT_RIGHT, // T
	SEG_TOP | SEG_MID, // U
	SEG_TOP_LEFT | SEG_TOP | SEG_TOP_RIGHT | SEG_MID, // v
	SEG_TOP_LEFT | SEG_TOP_RIGHT | SEG_MID, // w (this one looks very weird)
	SEG_TOP_LEFT | SEG_BOT_LEFT | SEG_TOP_RIGHT | SEG_BOT_RIGHT, // x
	SEG_TOP | SEG_BOT_LEFT, // y
	SEG_TOP_LEFT | SEG_BOT_LEFT // z
};

alt_u8 char_to_seven_seg(char c){


	if( c <= '9' && c >= '0')
		return numbers[c-'0'];
	else if( c <= 'z' && c >= 'a')
		return letters[c-'a'];
	else if( c <= 'Z' && c >= 'A')
		return letters[c-'A'];
	else
		return 0xff;
}
alt_u32 str_to_seven_seg (char *s){
	alt_u32 val = ~0;
	alt_u32 index = 0;
	while(*s != '\0' && index < 4){
		val = val<<8 | char_to_seven_seg(*s);
		s++;
		index++;
	}
	return val;
}
