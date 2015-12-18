/* ncconvert
 * a simple utility to convert text files between the nc100 and utf8
 *
 * Copyright (C) 2015 Gunnar Zötl <gz@tset.de>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mini_utf8.h"

/* from https://en.wikipedia.org/wiki/Code_page_437
 * where there are multiple lines for one eascii char, the first one
 * is the canonical translation from eascii to unicode.
 * Note that the first 32 values are for reference only, control chars
 * are returned as-is in both directions.
 */
struct {
	int easc;
	uint32_t unic;
} chars[] = {
	/*
	{0, 0x0000},
	{1, 0x263A},
	{2, 0x263B},
	{3, 0x2665},
	{4, 0x2666},
	{5, 0x2663},
	{6, 0x2660},
	{7, 0x2022},
	{8, 0x25D8},
	{9, 0x25CB},
	{10, 0x25D9},
	{11, 0x2642},
	{12, 0x2640},
	{13, 0x266A},
	{14, 0x266B},
	{15, 0x263C},
	{16, 0x25BA},
	{17, 0x25C4},
	{18, 0x2195},
	{19, 0x203C},
	{20, 0x00B6},
	{21, 0x00A7},
	{22, 0x25AC},
	{23, 0x21A8},
	{24, 0x2191},
	{25, 0x2193},
	{26, 0x2192},
	{27, 0x2190},
	{28, 0x221F},
	{29, 0x2194},
	{30, 0x25B2},
	{31, 0x25BC},
	*/
	{32, 0x0020},
	{33, 0x0021},
	{34, 0x0022},
	{35, 0x0023},
	{36, 0x0024},
	{37, 0x0025},
	{38, 0x0026},
	{39, 0x0027},
	{40, 0x0028},
	{41, 0x0029},
	{42, 0x002A},
	{43, 0x002B},
	{44, 0x002C},
	{45, 0x002D},
	{46, 0x002E},
	{47, 0x002F},
	{48, 0x0030},
	{49, 0x0031},
	{50, 0x0032},
	{51, 0x0033},
	{52, 0x0034},
	{53, 0x0035},
	{54, 0x0036},
	{55, 0x0037},
	{56, 0x0038},
	{57, 0x0039},
	{58, 0x003A},
	{59, 0x003B},
	{60, 0x003C},
	{61, 0x003D},
	{62, 0x003E},
	{63, 0x003F},
	{64, 0x0040},
	{65, 0x0041},
	{66, 0x0042},
	{67, 0x0043},
	{68, 0x0044},
	{69, 0x0045},
	{70, 0x0046},
	{71, 0x0047},
	{72, 0x0048},
	{73, 0x0049},
	{74, 0x004A},
	{75, 0x004B},
	{76, 0x004C},
	{77, 0x004D},
	{78, 0x004E},
	{79, 0x004F},
	{80, 0x0050},
	{81, 0x0051},
	{82, 0x0052},
	{83, 0x0053},
	{84, 0x0054},
	{85, 0x0055},
	{86, 0x0056},
	{87, 0x0057},
	{88, 0x0058},
	{89, 0x0059},
	{90, 0x005A},
	{91, 0x005B},
	{92, 0x005C},
	{93, 0x005D},
	{94, 0x005E},
	{95, 0x005F},
	{96, 0x0060},
	{97, 0x0061},
	{98, 0x0062},
	{99, 0x0063},
	{100, 0x0064},
	{101, 0x0065},
	{102, 0x0066},
	{103, 0x0067},
	{104, 0x0068},
	{105, 0x0069},
	{106, 0x006A},
	{107, 0x006B},
	{108, 0x006C},
	{109, 0x006D},
	{110, 0x006E},
	{111, 0x006F},
	{112, 0x0070},
	{113, 0x0071},
	{114, 0x0072},
	{115, 0x0073},
	{116, 0x0074},
	{117, 0x0075},
	{118, 0x0076},
	{119, 0x0077},
	{120, 0x0078},
	{121, 0x0079},
	{122, 0x007A},
	{123, 0x007B},
	{124, 0x007C},
	{125, 0x007D},
	{126, 0x007E},
	{127, 0x2302},
	{128, 0x00C7},
	{129, 0x00FC},
	{130, 0x00E9},
	{131, 0x00E2},
	{132, 0x00E4},
	{133, 0x00E0},
	{134, 0x00E5},
	{135, 0x00E7},
	{136, 0x00EA},
	{137, 0x00EB},
	{138, 0x00E8},
	{139, 0x00EF},
	{140, 0x00EE},
	{141, 0x00EC},
	{142, 0x00C4},
	{143, 0x00C5},
	{144, 0x00C9},
	{145, 0x00E6},
	{146, 0x00C6},
	{147, 0x00F4},
	{148, 0x00F6},
	{149, 0x00F2},
	{150, 0x00FB},
	{151, 0x00F9},
	{152, 0x00FF},
	{153, 0x00D6},
	{154, 0x00DC},
	{155, 0x00A2},
	{156, 0x00A3},
	{157, 0x00A5},
	{158, 0x20A7},
	{159, 0x0192},
	{160, 0x00E1},
	{161, 0x00ED},
	{162, 0x00F3},
	{163, 0x00FA},
	{164, 0x00F1},
	{165, 0x00D1},
	{166, 0x00AA},
	{167, 0x00BA},
	{168, 0x00BF},
	{169, 0x2310},
	{170, 0x00AC},
	{171, 0x00BD},
	{172, 0x00BC},
	{173, 0x00A1},
	{174, 0x00AB},
	{175, 0x00BB},
	{176, 0x2591},
	{177, 0x2592},
	{178, 0x2593},
	{179, 0x2502},
	{180, 0x2524},
	{181, 0x2561},
	{182, 0x2562},
	{183, 0x2556},
	{184, 0x2555},
	{185, 0x2563},
	{186, 0x2551},
	{187, 0x2557},
	{188, 0x255D},
	{189, 0x255C},
	{190, 0x255B},
	{191, 0x2510},
	{192, 0x2514},
	{193, 0x2534},
	{194, 0x252C},
	{195, 0x251C},
	{196, 0x2500},
	{197, 0x253C},
	{198, 0x255E},
	{199, 0x255F},
	{200, 0x255A},
	{201, 0x2554},
	{202, 0x2569},
	{203, 0x2566},
	{204, 0x2560},
	{205, 0x2550},
	{206, 0x256C},
	{207, 0x2567},
	{208, 0x2568},
	{209, 0x2564},
	{210, 0x2565},
	{211, 0x2559},
	{212, 0x2558},
	{213, 0x2552},
	{214, 0x2553},
	{215, 0x256B},
	{216, 0x256A},
	{217, 0x2518},
	{218, 0x250C},
	{219, 0x2588},
	{220, 0x2584},
	{221, 0x258C},
	{222, 0x2590},
	{223, 0x2580},
	{224, 0x03B1},
	{225, 0x00DF},
	{225, 0x03B2},
	{226, 0x0393},
	{227, 0x03C0},
	{228, 0x03A3},
	{228, 0x2211},
	{229, 0x03C3},
	{230, 0x00B5},
	{230, 0x03BC},
	{231, 0x03C4},
	{232, 0x03A6},
	{233, 0x0398},
	{234, 0x03A9},
	{234, 0x2126},
	{235, 0x03B4},
	{235, 0x00F0},
	{235, 0x2202},
	{236, 0x221E},
	{237, 0x03C6},
	{237, 0x2205},
	{237, 0x03D5},
	{237, 0x2300},
	{237, 0x00F8},
	{238, 0x03B5},
	{238, 0x2208},
	{238, 0x20AC},
	{239, 0x2229},
	{240, 0x2261},
	{241, 0x00B1},
	{242, 0x2265},
	{243, 0x2264},
	{244, 0x2320},
	{245, 0x2321},
	{246, 0x00F7},
	{247, 0x2248},
	{248, 0x00B0},
	{249, 0x2219},
	{250, 0x00B7},
	{251, 0x221A},
	{252, 0x207F},
	{253, 0x00B2},
	{254, 0x25A0},
	{255, 0x00A0},
	{-1, 0}
};

const char* prg_name = "ncrecode";

void usage()
{
	fprintf(stderr, "usage: %s [-tonc|-addln [step]|-fromnc|-remln] [infile [outfile]]\n", prg_name);
	fprintf(stderr, "\n");
	fprintf(stderr, "  converts a textfile between eascii (as used on the nc100) and utf8.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  -tonc   convert from utf8 to eascii for use on the nc100\n");
	fprintf(stderr, "  -addln  as -tonc, but adds line numbers. Takes an optional step\n");
	fprintf(stderr, "          argument which defaults to 1.\n");
	fprintf(stderr, "  -fromnc convert from eascii to utf8 for use on a PC\n");
	fprintf(stderr, "  -remln  as -fromnc, but also removes line numbers.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  infile may be - to use stdin. This is only needed when an output file\n");
	fprintf(stderr, "  is specified, as infile defaults to stdin and outfile defaults to stdout.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  -addnl and -remnl are intended for basic programming. It is an error to\n");
	fprintf(stderr, "  specify -addln with -fromnc or -remln with -tonc, or -addln with -remln.\n");
	exit(1);
}

void error(const char* msg)
{
	fprintf(stderr, "error: %s\n", msg);
	exit(1);
}

uint8_t find_utf8(uint32_t u8)
{
	if (u8 < 32) {
		return u8;
	}
	int i;
	for (i = 0; chars[i].easc >= 0; ++i) {
		if (chars[i].unic == u8) {
			return chars[i].easc;
		}
	}
	return 32;
}

void utf8_to_eascii(FILE *in, FILE *out, int addln)
{
	char u8buf[5], numbuf[16];
	uint32_t u8ch;
	int byte = fgetc(in), u8len = 0, bol = 1, lineno = addln;
	uint8_t each;
	while (!feof(in)) {
		u8buf[u8len++] = byte;
		u8buf[u8len] = 0;
		if (mini_utf8_check_encoding(u8buf) > -1 || u8len == 4) {
			const char* p = u8buf;
			u8ch = mini_utf8_decode(&p);
			if (addln) {
				if (bol) {
					snprintf(numbuf, 16, "%d ", lineno);
					const char* p = numbuf;
					while (*p) {
						fputc(*p++, out);
					}
					lineno += addln;
					bol = 0;
				} else if (u8ch == '\n') {
					bol = 1;
				}
			}
			each = find_utf8(u8ch);
			fputc(each, out);
			u8len = 0;
		}
		byte = fgetc(in);
	}
}

uint32_t find_eascii(uint8_t ea)
{
	if (ea < 32) {
		return ea;
	}
	int i;
	for (i = 0; chars[i].easc >= 0; ++i) {
		if (chars[i].easc == ea) {
			return chars[i].unic;
		}
	}
	return 32;
}

void eascii_to_utf8(FILE *in, FILE *out, int remln)
{
	char u8buf[4];
	uint32_t u8ch;
	int each = fgetc(in), u8len, cnt, bol = 1;
	while (!feof(in)) {
		u8ch = find_eascii(each);
		if (remln) {
			if (bol) {
				if (u8ch >= '0' && u8ch < '9') {
					each = fgetc(in);
					continue;
				}
				bol = 0;
				if (u8ch == ' ') {
					each = fgetc(in);
					continue;
				}
			} else if (u8ch == '\n') {
				bol = 1;
			}
		}
		u8len = mini_utf8_encode(u8ch, u8buf, 4);
		for (cnt = 0; cnt < u8len; ++cnt) {
			fputc(u8buf[cnt], out);
		}
		each = fgetc(in);
	}
}

#define E_TO_U 1
#define U_TO_E 2

int main(int argc, char** argv)
{
	int direction = 0;
	FILE *in = stdin, *out = stdout;
	int addln = 0, remln = 0;
	int acnt = 1;

	prg_name = argv[0];
	if (argc > 1) {
		while (acnt < argc && *argv[acnt] == '-') {
			if (strcmp(argv[acnt], "-h") == 0) {
				usage();
			} else if (strcmp(argv[acnt], "-tonc") == 0) {
				if (direction && direction == E_TO_U) { usage(); }
				direction = U_TO_E;
				acnt++;
			} else if (strcmp(argv[acnt], "-addln") == 0) {
				if (direction && direction == E_TO_U) { usage(); }
				direction = U_TO_E;
				acnt++;
				if (acnt < argc && *argv[acnt] >= '0' && *argv[acnt] <= '9') {
					addln = atoi(argv[acnt++]);
				} else {
					addln = 1;
				}
			} else if (strcmp(argv[acnt], "-fromnc") == 0) {
				if (direction && direction == U_TO_E) { usage(); }
				direction = E_TO_U;
				acnt++;
			} else if (strcmp(argv[acnt], "-remln") == 0) {
				if (direction && direction == U_TO_E) { usage(); }
				direction = E_TO_U;
				acnt++;
				remln = 1;
			} else if (strcmp(argv[1], "-") == 0) {
				break;
			} else {
				usage();
			}
		}
		if (!direction) {
			direction = E_TO_U;
		}
		if (acnt < argc) {
			const char* ifname = argv[acnt++];
			if (strcmp(ifname, "-") != 0) {
				in = fopen(ifname, "r");
				if (!in) {
					error("can't open input file.");
				}
			}
			if (acnt < argc) {
				const char *ofname = argv[acnt++];
				out = fopen(ofname, "w");
				if (!out) {
					error("can't open output file.");
				}
			}
		}
		if (acnt < argc) {
			usage();
		}
	}

	if (direction == U_TO_E) {
		utf8_to_eascii(in, out, addln);
	} else {
		eascii_to_utf8(in, out, remln);
	}

	if (in != stdin) {
		fclose(in);
	}
	if (out != stdout) {
		fclose(stdout);
	}

	exit(0);
}
