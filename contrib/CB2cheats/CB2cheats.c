#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "cb2_crypto.h"

// Application's name and current version
#define APP_NAME	"CB2cheats"
#define APP_VERSION	"1.0"

void ShowBanner(void)
{
	printf("CodeBreaker PS2 Cheat Code Extractor v%s\n", APP_VERSION);
	printf("THIS PROGRAM IS NOT LICENSED, ENDORSED, NOR SPONSORED BY SONY\n"
		"COMPUTER ENTERTAINMENT, INC. NOR PELICAN ACCESSORIES, INC.\n"
		"ALL TRADEMARKS ARE PROPERTY OF THEIR RESPECTIVE OWNERS.\n");
	printf("Copyright (C) 2006 misfire\n\n");
}

void ShowUsage(void)
{
	printf("Usage: %s <BIN file> <TXT file> [-d]\n\n", APP_NAME);
	printf("Extracts all CB2 cheat codes from a RAM dump to a text file.\n");
	printf("Additionally, use the -d switch to decrypt the codes.\n");
}

int main(int argc, char *argv[])
{
	FILE *fin, *fout;
	u8 *buf;
	char *p;
	int filesize, off = 0;
	int totgames = 0, totcodes = 0, totlines = 0;
	u16 numdesc, numlines;
	u32 code[2];
	int decrypt = FALSE;

	ShowBanner();

	// Check arguments
	if (argc < 3) {
		ShowUsage();
		return -1;
	}

	decrypt = (argc > 3) && !strcmp(argv[3], "-d");

	// Open files
	fin = fopen(argv[1], "rb");
	if (fin == NULL) {
		fprintf(stderr, "Error: Can't open file '%s' for reading\n", argv[1]);
		return -2;
	}

	fout = fopen(argv[2], "w");
	if (fout == NULL) {
		fprintf(stderr, "Error: Can't open file '%s' for writing\n", argv[2]);
		fclose(fin);
		return -3;
	}

	// Get size of binary file
	fseek(fin, 0, SEEK_END);
	filesize = ftell(fin);

	// Allocate memory for file contents
	buf = (u8*)malloc(filesize);
	if (buf == NULL) {
		fprintf(stderr, "Error: Unable to allocate %i bytes\n", filesize);
		fclose(fin);
		fclose(fout);
		return -4;
	}

	// Read from BIN file into buffer
	fseek(fin, 0, SEEK_SET);
	if (fread(buf, filesize, 1, fin) != 1) {
		fprintf(stderr, "Error: Failed to read %i bytes from file\n", filesize);
		fclose(fin);
		fclose(fout);
		return -5;
	}
	fclose(fin);

	if (decrypt)
		printf("Extracting and decrypting cheat codes... ");
	else
		printf("Extracting cheat codes... ");

	fprintf(fout, "\n//--------\n\n");

	// Parse buffer, write results to TXT file
	while (off < filesize) {
		// Check for end marker
		if (*(u32*)&buf[off] == 0xFFFFFFFF) break;

		// Reset CB encryption
		CBReset();

		// Process game title
		// Example: "007 Agent Under Fire"
		p = (char*)&buf[off];
		fprintf(fout, "\"%s\"\n", p);
		off += strlen(p) + 1;

		numdesc = *(u16*)&buf[off];
		off += sizeof(u16);

		// Process code description(s)
		// Example: Infinite Ammo
		while (numdesc--) {
			p = (char*)&buf[off];
			off += strlen(p) + 1;
			// Skip desc type, we won't use it
			off++;

			numlines = *(u16*)&buf[off];
			totlines += numlines;
			off += sizeof(u16);

			// If there is no code line, it's just a heading
			// Example: *Action Mission Codes
			if (!numlines) fprintf(fout, "*%s\n", p);
			else {
				fprintf(fout, "%s\n", p);
				totcodes++;

				// Process code line(s)
				// Example: 1A3EDED4 000003E7
				while (numlines--) {
					code[0] = *(u32*)&buf[off];
					off += sizeof(u32);
					code[1] = *(u32*)&buf[off];
					off += sizeof(u32);

					// Decrypt code
					if (decrypt) CBDecryptCode(&code[0], &code[1]);

					fprintf(fout, "%08X %08X\n", code[0], code[1]);
				}
			}
		}

		fprintf(fout, "\n//--------\n\n");
		totgames++;
	}

	// Append some stats
	fprintf(fout, "// Total Games: %i\n", totgames);
	fprintf(fout, "// Total Codes: %i\n", totcodes);
	fprintf(fout, "// Total Lines: %i\n", totlines);

	fclose(fout);
	free(buf);

	printf("Done!\n");

	return 0;
}

/*
BIN File Format
---------------

Game Title, 0
	# Desc (2 bytes)
	Desc, 0
		Desc Type (0=Code, 4=Heading)
		# Code Lines (2 bytes)
		Code Line
		Code Line
		...
	Desc, 0
		Desc Type (0=Code, 4=Heading)
		# Code Lines (2 bytes)
		Code Line
		...
Game Title, 0
	...
*/
