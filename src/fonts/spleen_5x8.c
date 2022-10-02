// Created from bdf2c Version 4, (c) 2009, 2010 by Lutz Sammer
//	License AGPLv3: GNU Affero General Public License version 3

#include "font.h"

	/// character bitmap for each encoding
static const unsigned char __spleen_5x8_bitmap__[] = {
//  32 $20 'SPACE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
//  33 $21 'EXCLAMATION'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	________,
	__X_____,
	________,
//  34 $22 'QUOTATION'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	_X_X____,
	_X_X____,
	_X_X____,
	________,
	________,
	________,
	________,
	________,
//  35 $23 'NUMBER'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_X_X____,
	XXXXX___,
	_X_X____,
	_X_X____,
	XXXXX___,
	_X_X____,
	________,
//  36 $24 'DOLLAR'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	__X_____,
	_XXX____,
	X_X_____,
	_XX_____,
	__XX____,
	__XX____,
	XXX_____,
	__X_____,
//  37 $25 'PERCENT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	___X____,
	X__X____,
	X_X_____,
	__X_____,
	_X______,
	_X_X____,
	X__X____,
	X_______,
//  38 $26 'AMPERSAND'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	__X_____,
	_X_X____,
	_X_X____,
	_XX_____,
	X_X_____,
	X__X____,
	_XX_X___,
	________,
//  39 $27 'APOSTROPHE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	__X_____,
	__X_____,
	__X_____,
	________,
	________,
	________,
	________,
	________,
//  40 $28 'LEFT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	___X____,
	__X_____,
	_X______,
	_X______,
	_X______,
	_X______,
	__X_____,
	___X____,
//  41 $29 'RIGHT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	_X______,
	__X_____,
	___X____,
	___X____,
	___X____,
	___X____,
	__X_____,
	_X______,
//  42 $2a 'ASTERISK'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	X__X____,
	_XX_____,
	XXXX____,
	_XX_____,
	X__X____,
	________,
//  43 $2b 'PLUS'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	__X_____,
	__X_____,
	XXXXX___,
	__X_____,
	__X_____,
	________,
//  44 $2c 'COMMA'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	__X_____,
	__X_____,
	_X______,
//  45 $2d 'HYPHEN-MINUS'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	XXXX____,
	________,
	________,
	________,
//  46 $2e 'FULL'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	__X_____,
	________,
//  47 $2f 'SOLIDUS'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	___X____,
	___X____,
	__X_____,
	__X_____,
	_X______,
	_X______,
	X_______,
	X_______,
//  48 $30 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	X_XX____,
	XX_X____,
	X__X____,
	_XX_____,
	________,
//  49 $31 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	__X_____,
	_XX_____,
	__X_____,
	__X_____,
	__X_____,
	_XXX____,
	________,
//  50 $32 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	___X____,
	_XX_____,
	X_______,
	XXXX____,
	________,
//  51 $33 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	__X_____,
	___X____,
	X__X____,
	_XX_____,
	________,
//  52 $34 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X_______,
	X_X_____,
	X_X_____,
	XXXX____,
	__X_____,
	__X_____,
	________,
//  53 $35 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	XXXX____,
	X_______,
	XXXX____,
	___X____,
	___X____,
	XXX_____,
	________,
//  54 $36 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X_______,
	XXX_____,
	X__X____,
	X__X____,
	_XX_____,
	________,
//  55 $37 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	XXXX____,
	X__X____,
	__X_____,
	__X_____,
	_X______,
	_X______,
	________,
//  56 $38 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	_XX_____,
	X__X____,
	X__X____,
	_XX_____,
	________,
//  57 $39 'DIGIT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	X__X____,
	_XXX____,
	___X____,
	_XX_____,
	________,
//  58 $3a 'COLON'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	__X_____,
	________,
	________,
	__X_____,
	________,
//  59 $3b 'SEMICOLON'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	__X_____,
	________,
	__X_____,
	__X_____,
	_X______,
//  60 $3c 'LESS-THAN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	___X____,
	__X_____,
	_X______,
	_X______,
	__X_____,
	___X____,
	________,
//  61 $3d 'EQUALS'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	XXXX____,
	________,
	XXXX____,
	________,
	________,
//  62 $3e 'GREATER-THAN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_X______,
	__X_____,
	___X____,
	___X____,
	__X_____,
	_X______,
	________,
//  63 $3f 'QUESTION'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	_XX_____,
	X__X____,
	___X____,
	__X_____,
	_X______,
	________,
	_X______,
	________,
//  64 $40 'COMMERCIAL'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	X_XX____,
	X_XX____,
	X_______,
	_XXX____,
	________,
//  65 $41 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	X__X____,
	XXXX____,
	X__X____,
	X__X____,
	________,
//  66 $42 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	XXX_____,
	X__X____,
	XXX_____,
	X__X____,
	X__X____,
	XXX_____,
	________,
//  67 $43 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XXX____,
	X_______,
	X_______,
	X_______,
	X_______,
	_XXX____,
	________,
//  68 $44 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	XXX_____,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	XXX_____,
	________,
//  69 $45 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XXX____,
	X_______,
	XXX_____,
	X_______,
	X_______,
	_XXX____,
	________,
//  70 $46 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XXX____,
	X_______,
	X_______,
	XXX_____,
	X_______,
	X_______,
	________,
//  71 $47 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XXX____,
	X_______,
	X_XX____,
	X__X____,
	X__X____,
	_XXX____,
	________,
//  72 $48 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	X__X____,
	XXXX____,
	X__X____,
	X__X____,
	X__X____,
	________,
//  73 $49 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XXX____,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	_XXX____,
	________,
//  74 $4a 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XXX____,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	XX______,
	________,
//  75 $4b 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	X__X____,
	XXX_____,
	X__X____,
	X__X____,
	X__X____,
	________,
//  76 $4c 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X_______,
	X_______,
	X_______,
	X_______,
	X_______,
	_XXX____,
	________,
//  77 $4d 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	XXXX____,
	XXXX____,
	X__X____,
	X__X____,
	X__X____,
	________,
//  78 $4e 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	XX_X____,
	XX_X____,
	X_XX____,
	X_XX____,
	X__X____,
	________,
//  79 $4f 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	_XX_____,
	________,
//  80 $50 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	XXX_____,
	X__X____,
	X__X____,
	XXX_____,
	X_______,
	X_______,
	________,
//  81 $51 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XX_____,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	_XX_____,
	__XX____,
//  82 $52 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	XXX_____,
	X__X____,
	X__X____,
	XXX_____,
	X__X____,
	X__X____,
	________,
//  83 $53 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_XXX____,
	X_______,
	_XX_____,
	___X____,
	___X____,
	XXX_____,
	________,
//  84 $54 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	XXXXX___,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	________,
//  85 $55 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	_XXX____,
	________,
//  86 $56 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	_XX_____,
	_XX_____,
	________,
//  87 $57 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	X__X____,
	X__X____,
	XXXX____,
	XXXX____,
	X__X____,
	________,
//  88 $58 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	X__X____,
	_XX_____,
	_XX_____,
	X__X____,
	X__X____,
	________,
//  89 $59 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	X__X____,
	X__X____,
	X__X____,
	_XXX____,
	___X____,
	XXX_____,
	________,
//  90 $5a 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	XXXX____,
	___X____,
	__X_____,
	_X______,
	X_______,
	XXXX____,
	________,
//  91 $5b 'LEFT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	_XXX____,
	_X______,
	_X______,
	_X______,
	_X______,
	_X______,
	_X______,
	_XXX____,
//  92 $5c 'REVERSE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	X_______,
	X_______,
	_X______,
	_X______,
	__X_____,
	__X_____,
	___X____,
	___X____,
//  93 $5d 'RIGHT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	_XXX____,
	___X____,
	___X____,
	___X____,
	___X____,
	___X____,
	___X____,
	_XXX____,
//  94 $5e 'CIRCUMFLEX'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	__X_____,
	_X_X____,
	X___X___,
	________,
	________,
	________,
	________,
//  95 $5f 'LOW'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	XXXX____,
//  96 $60 'GRAVE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	_X______,
	__X_____,
	________,
	________,
	________,
	________,
	________,
	________,
//  97 $61 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	_XX_____,
	___X____,
	_XXX____,
	X__X____,
	_XXX____,
	________,
//  98 $62 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	X_______,
	X_______,
	XXX_____,
	X__X____,
	X__X____,
	X__X____,
	XXX_____,
	________,
//  99 $63 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	_XXX____,
	X_______,
	X_______,
	X_______,
	_XXX____,
	________,
// 100 $64 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	___X____,
	___X____,
	_XXX____,
	X__X____,
	X__X____,
	X__X____,
	_XXX____,
	________,
// 101 $65 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	_XXX____,
	X__X____,
	XXXX____,
	X_______,
	_XXX____,
	________,
// 102 $66 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	__XX____,
	_X______,
	_X______,
	XXX_____,
	_X______,
	_X______,
	_X______,
	________,
// 103 $67 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	_XXX____,
	X__X____,
	X__X____,
	_XX_____,
	___X____,
	XXX_____,
// 104 $68 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	X_______,
	X_______,
	XXX_____,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	________,
// 105 $69 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	__X_____,
	________,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	________,
// 106 $6a 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	__X_____,
	________,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	XX______,
// 107 $6b 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	X_______,
	X_______,
	X__X____,
	X_X_____,
	XX______,
	X_X_____,
	X__X____,
	________,
// 108 $6c 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	_X______,
	_X______,
	_X______,
	_X______,
	_X______,
	_X______,
	__XX____,
	________,
// 109 $6d 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	X__X____,
	XXXX____,
	XXXX____,
	X__X____,
	X__X____,
	________,
// 110 $6e 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	XXX_____,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	________,
// 111 $6f 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	_XX_____,
	X__X____,
	X__X____,
	X__X____,
	_XX_____,
	________,
// 112 $70 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	XXX_____,
	X__X____,
	X__X____,
	XXX_____,
	X_______,
	X_______,
// 113 $71 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	_XXX____,
	X__X____,
	X__X____,
	_XXX____,
	___X____,
	___X____,
// 114 $72 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	_XXX____,
	X__X____,
	X_______,
	X_______,
	X_______,
	________,
// 115 $73 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	_XXX____,
	X_______,
	_XX_____,
	___X____,
	XXX_____,
	________,
// 116 $74 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	_X______,
	_X______,
	XXX_____,
	_X______,
	_X______,
	_X______,
	__XX____,
	________,
// 117 $75 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	X__X____,
	X__X____,
	X__X____,
	X__X____,
	_XXX____,
	________,
// 118 $76 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	X__X____,
	X__X____,
	X__X____,
	_XX_____,
	_XX_____,
	________,
// 119 $77 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	X__X____,
	X__X____,
	XXXX____,
	XXXX____,
	X__X____,
	________,
// 120 $78 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	X__X____,
	_XX_____,
	_XX_____,
	X__X____,
	X__X____,
	________,
// 121 $79 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	X__X____,
	X__X____,
	X__X____,
	_XXX____,
	___X____,
	XXX_____,
// 122 $7a 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	XXXX____,
	___X____,
	__X_____,
	_X______,
	XXXX____,
	________,
// 123 $7b 'LEFT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	__XX____,
	_X______,
	_X______,
	XX______,
	XX______,
	_X______,
	_X______,
	__XX____,
// 124 $7c 'VERTICAL'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	__X_____,
	________,
// 125 $7d 'RIGHT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	XX______,
	__X_____,
	__X_____,
	__XX____,
	__XX____,
	__X_____,
	__X_____,
	XX______,
// 126 $7e 'TILDE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	_X__X___,
	X_XX____,
	________,
	________,
	________,
	________,
	________,
// 127 $7f '<control>'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 160 $a0 'NO-BREAK'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 161 $a1 'INVERTED'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 162 $a2 'CENT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 163 $a3 'POUND'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 164 $a4 'CURRENCY'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 165 $a5 'YEN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 166 $a6 'BROKEN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 167 $a7 'SECTION'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 168 $a8 'DIAERESIS'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 169 $a9 'COPYRIGHT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 170 $aa 'FEMININE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 171 $ab 'LEFT-POINTING'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 172 $ac 'NOT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 173 $ad 'SOFT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 174 $ae 'REGISTERED'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 175 $af 'MACRON'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 176 $b0 'DEGREE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 177 $b1 'PLUS-MINUS'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 178 $b2 'SUPERSCRIPT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 179 $b3 'SUPERSCRIPT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 180 $b4 'ACUTE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 181 $b5 'MICRO'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 182 $b6 'PILCROW'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 183 $b7 'MIDDLE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 184 $b8 'CEDILLA'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 185 $b9 'SUPERSCRIPT'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 186 $ba 'MASCULINE'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 187 $bb 'RIGHT-POINTING'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 188 $bc 'VULGAR'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 189 $bd 'VULGAR'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 190 $be 'VULGAR'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 191 $bf 'INVERTED'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 192 $c0 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 193 $c1 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 194 $c2 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 195 $c3 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 196 $c4 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 197 $c5 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 198 $c6 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 199 $c7 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 200 $c8 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 201 $c9 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 202 $ca 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 203 $cb 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 204 $cc 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 205 $cd 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 206 $ce 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 207 $cf 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 208 $d0 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 209 $d1 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 210 $d2 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 211 $d3 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 212 $d4 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 213 $d5 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 214 $d6 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 215 $d7 'MULTIPLICATION'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 216 $d8 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 217 $d9 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 218 $da 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 219 $db 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 220 $dc 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 221 $dd 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 222 $de 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 223 $df 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 224 $e0 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 225 $e1 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 226 $e2 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 227 $e3 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 228 $e4 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 229 $e5 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 230 $e6 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 231 $e7 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 232 $e8 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 233 $e9 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 234 $ea 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 235 $eb 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 236 $ec 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 237 $ed 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 238 $ee 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 239 $ef 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 240 $f0 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 241 $f1 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 242 $f2 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 243 $f3 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 244 $f4 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 245 $f5 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 246 $f6 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 247 $f7 'DIVISION'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 248 $f8 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 249 $f9 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 250 $fa 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 251 $fb 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 252 $fc 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 253 $fd 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 254 $fe 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
// 255 $ff 'LATIN'
//	width 5, bbx 0, bby -1, bbw 5, bbh 8
	________,
	________,
	________,
	________,
	________,
	________,
	________,
	________,
};

	/// character width for each encoding
static const unsigned char __spleen_5x8_widths__[] = {
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
};

	/// character encoding for each index entry
static const unsigned short __spleen_5x8_index__[] = {
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	44,
	45,
	46,
	47,
	48,
	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	58,
	59,
	60,
	61,
	62,
	63,
	64,
	65,
	66,
	67,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
	75,
	76,
	77,
	78,
	79,
	80,
	81,
	82,
	83,
	84,
	85,
	86,
	87,
	88,
	89,
	90,
	91,
	92,
	93,
	94,
	95,
	96,
	97,
	98,
	99,
	100,
	101,
	102,
	103,
	104,
	105,
	106,
	107,
	108,
	109,
	110,
	111,
	112,
	113,
	114,
	115,
	116,
	117,
	118,
	119,
	120,
	121,
	122,
	123,
	124,
	125,
	126,
	127,
	160,
	161,
	162,
	163,
	164,
	165,
	166,
	167,
	168,
	169,
	170,
	171,
	172,
	173,
	174,
	175,
	176,
	177,
	178,
	179,
	180,
	181,
	182,
	183,
	184,
	185,
	186,
	187,
	188,
	189,
	190,
	191,
	192,
	193,
	194,
	195,
	196,
	197,
	198,
	199,
	200,
	201,
	202,
	203,
	204,
	205,
	206,
	207,
	208,
	209,
	210,
	211,
	212,
	213,
	214,
	215,
	216,
	217,
	218,
	219,
	220,
	221,
	222,
	223,
	224,
	225,
	226,
	227,
	228,
	229,
	230,
	231,
	232,
	233,
	234,
	235,
	236,
	237,
	238,
	239,
	240,
	241,
	242,
	243,
	244,
	245,
	246,
	247,
	248,
	249,
	250,
	251,
	252,
	253,
	254,
	255,
};

	/// bitmap font structure
const struct bitmap_font spleen_5x8 = {
	.Width = 5, .Height = 8,
	.Chars = 192,
	.Widths = __spleen_5x8_widths__,
	.Index = __spleen_5x8_index__,
	.Bitmap = __spleen_5x8_bitmap__,
};

