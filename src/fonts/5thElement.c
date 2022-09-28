// Created from bdf2c Version 4, (c) 2009, 2010 by Lutz Sammer
//	License AGPLv3: GNU Affero General Public License version 3

#include "font.h"

	/// character bitmap for each encoding
static const unsigned char ___5thelement_bitmap__[] = {
//  32 $20 'char32'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	________,
	________,
	________,
	________,
//  33 $21 'char33'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	_X______,
	________,
	_X______,
	________,
//  34 $22 'char34'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	X_X_____,
	________,
	________,
	________,
//  35 $23 'char35'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	XXXX____,
	XXXX____,
	_XX_____,
	________,
//  36 $24 'char36'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	XXX_____,
	XXX_____,
	_X______,
	________,
//  37 $25 'char37'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X__X____,
	__X_____,
	_X______,
	X__X____,
	________,
//  38 $26 'char38'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XX______,
	X_XX____,
	XXX_____,
	XX_X____,
	________,
//  39 $27 'char39'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	__X_____,
	_X______,
	________,
	________,
	________,
//  40 $28 'char40'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	__X_____,
	_X______,
	_X______,
	__X_____,
	________,
//  41 $29 'char41'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	__X_____,
	__X_____,
	_X______,
	________,
//  42 $2a 'char42'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	XXX_____,
	_X______,
	X_X_____,
	________,
//  43 $2b 'char43'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	_X______,
	XXX_____,
	_X______,
	________,
//  44 $2c 'char44'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	________,
	________,
	_XX_____,
	__X_____,
//  45 $2d 'char45'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	________,
	XXX_____,
	________,
	________,
//  46 $2e 'char46'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	________,
	________,
	_X______,
	________,
//  47 $2f 'char47'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	__X_____,
	__X_____,
	_X______,
	_X______,
	________,
//  48 $30 'char48'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	X_X_____,
	X_X_____,
	XXX_____,
	________,
//  49 $31 'char49'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XX______,
	_X______,
	_X______,
	XXX_____,
	________,
//  50 $32 'char50'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	__X_____,
	XX______,
	XXX_____,
	________,
//  51 $33 'char51'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	_XX_____,
	__X_____,
	XXX_____,
	________,
//  52 $34 'char52'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	X_X_____,
	XXX_____,
	__X_____,
	________,
//  53 $35 'char53'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	XX______,
	__X_____,
	XXX_____,
	________,
//  54 $36 'char54'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XX______,
	X_______,
	XXX_____,
	XXX_____,
	________,
//  55 $37 'char55'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	__X_____,
	_X______,
	_X______,
	________,
//  56 $38 'char56'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	XXX_____,
	X_X_____,
	XXX_____,
	________,
//  57 $39 'char57'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	XXX_____,
	__X_____,
	_XX_____,
	________,
//  58 $3a 'char58'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	_X______,
	________,
	_X______,
	________,
//  59 $3b 'char59'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	_X______,
	________,
	_XX_____,
	__X_____,
//  60 $3c 'char60'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	__X_____,
	_X______,
	__X_____,
	________,
//  61 $3d 'char61'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XXX_____,
	________,
	XXX_____,
	________,
//  62 $3e 'char62'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	_X______,
	__X_____,
	_X______,
	________,
//  63 $3f 'char63'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	__X_____,
	_XX_____,
	________,
	_X______,
//  64 $40 'char64'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXXX____,
	X__X____,
	___X____,
	XX_X____,
	XXXX____,
//  65 $41 'char65'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	X_X_____,
	XXX_____,
	X_X_____,
	________,
//  66 $42 'char66'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XX______,
	XXX_____,
	X_X_____,
	XXX_____,
	________,
//  67 $43 'char67'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	X_______,
	X_______,
	XXX_____,
	________,
//  68 $44 'char68'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XX______,
	X_X_____,
	X_X_____,
	XX______,
	________,
//  69 $45 'char69'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	XX______,
	X_______,
	XXX_____,
	________,
//  70 $46 'char70'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	XX______,
	X_______,
	X_______,
	________,
//  71 $47 'char71'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	X_______,
	X_X_____,
	XXX_____,
	________,
//  72 $48 'char72'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	X_X_____,
	XXX_____,
	X_X_____,
	________,
//  73 $49 'char73'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	_X______,
	_X______,
	XXX_____,
	________,
//  74 $4a 'char74'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	__X_____,
	X_X_____,
	XXX_____,
	________,
//  75 $4b 'char75'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	XX______,
	XX______,
	X_X_____,
	________,
//  76 $4c 'char76'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_______,
	X_______,
	X_______,
	XXX_____,
	________,
//  77 $4d 'char77'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	XXX_____,
	XXX_____,
	X_X_____,
	________,
//  78 $4e 'char78'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	X_X_____,
	X_X_____,
	X_X_____,
	________,
//  79 $4f 'char79'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	X_X_____,
	X_X_____,
	XXX_____,
	________,
//  80 $50 'char80'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	X_X_____,
	XXX_____,
	X_______,
	________,
//  81 $51 'char81'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	X_X_____,
	XXX_____,
	XXXX____,
	________,
//  82 $52 'char82'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	X_X_____,
	XX______,
	X_X_____,
	________,
//  83 $53 'char83'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	X_______,
	_XX_____,
	XXX_____,
	________,
//  84 $54 'char84'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	_X______,
	_X______,
	_X______,
	________,
//  85 $55 'char85'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	X_X_____,
	X_X_____,
	XXX_____,
	________,
//  86 $56 'char86'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	X_X_____,
	XX______,
	X_______,
	________,
//  87 $57 'char87'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	XXX_____,
	XXX_____,
	XXX_____,
	________,
//  88 $58 'char88'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	_X______,
	X_X_____,
	X_X_____,
	________,
//  89 $59 'char89'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_X_____,
	XXX_____,
	_X______,
	_X______,
	________,
//  90 $5a 'char90'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XXX_____,
	__X_____,
	_X______,
	XXX_____,
	________,
//  91 $5b 'char91'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	_X______,
	_X______,
	_XX_____,
	________,
//  92 $5c 'char92'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	_X______,
	__X_____,
	__X_____,
	________,
//  93 $5d 'char93'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	__X_____,
	__X_____,
	_XX_____,
	________,
//  94 $5e 'char94'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	X_X_____,
	________,
	________,
	________,
//  95 $5f 'char95'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	________,
	________,
	________,
	XXXX____,
//  96 $60 'char96'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_______,
	_X______,
	________,
	________,
	________,
//  97 $61 'char97'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	_XX_____,
	X_X_____,
	XXX_____,
	________,
//  98 $62 'char98'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_______,
	XXX_____,
	X_X_____,
	XXX_____,
	________,
//  99 $63 'char99'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	_XX_____,
	X_______,
	XXX_____,
	________,
// 100 $64 'char100'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	__X_____,
	XXX_____,
	X_X_____,
	XXX_____,
	________,
// 101 $65 'char101'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XXX_____,
	X_X_____,
	XX______,
	________,
// 102 $66 'char102'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	__X_____,
	_X______,
	_XX_____,
	_X______,
	________,
// 103 $67 'char103'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	_XX_____,
	X_X_____,
	_XX_____,
	XX______,
// 104 $68 'char104'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_______,
	XXX_____,
	X_X_____,
	X_X_____,
	________,
// 105 $69 'char105'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	________,
	_X______,
	_X______,
	________,
// 106 $6a 'char106'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	________,
	_X______,
	_X______,
	XX______,
// 107 $6b 'char107'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	X_______,
	X_X_____,
	XX______,
	X_X_____,
	________,
// 108 $6c 'char108'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XX______,
	_X______,
	_X______,
	_XX_____,
	________,
// 109 $6d 'char109'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XXX_____,
	XXX_____,
	X_X_____,
	________,
// 110 $6e 'char110'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XXX_____,
	X_X_____,
	X_X_____,
	________,
// 111 $6f 'char111'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XXX_____,
	X_X_____,
	XXX_____,
	________,
// 112 $70 'char112'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XXX_____,
	X_X_____,
	XXX_____,
	X_______,
// 113 $71 'char113'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XXX_____,
	X_X_____,
	XXX_____,
	__X_____,
// 114 $72 'char114'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XXX_____,
	X_______,
	X_______,
	________,
// 115 $73 'char115'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	_XX_____,
	_X______,
	XX______,
	________,
// 116 $74 'char116'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	_XX_____,
	_X______,
	_XX_____,
	________,
// 117 $75 'char117'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	X_X_____,
	X_X_____,
	XXX_____,
	________,
// 118 $76 'char118'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	X_X_____,
	XXX_____,
	_X______,
	________,
// 119 $77 'char119'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	X_X_____,
	XXX_____,
	XXX_____,
	________,
// 120 $78 'char120'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	X_X_____,
	_X______,
	X_X_____,
	________,
// 121 $79 'char121'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	X_X_____,
	XXX_____,
	__X_____,
	XX______,
// 122 $7a 'char122'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	________,
	XX______,
	_X______,
	_XX_____,
	________,
// 123 $7b 'char123'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_XX_____,
	XX______,
	_X______,
	_XX_____,
	________,
// 124 $7c 'char124'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	_X______,
	_X______,
	_X______,
	_X______,
	_X______,
// 125 $7d 'char125'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	XX______,
	_XX_____,
	_X______,
	XX______,
	________,
// 126 $7e 'char126'
//	width 4, bbx 0, bby 0, bbw 4, bbh 5
	__X_____,
	XXX_____,
	X_______,
	________,
	________,
};

	/// character width for each encoding
static const unsigned char ___5thelement_widths__[] = {
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
};

	/// character encoding for each index entry
static const unsigned short ___5thelement_index__[] = {
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
};

	/// bitmap font structure
const struct bitmap_font _5thelement = {
	.Width = 4, .Height = 5,
	.Chars = 95,
	.Widths = ___5thelement_widths__,
	.Index = ___5thelement_index__,
	.Bitmap = ___5thelement_bitmap__,
};

