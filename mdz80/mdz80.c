/***************************************************************************
 *   Copyright (C) 2008 by Piergiorgio Betti   *
 *   pbetti@lpconsul.net   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*
 * Z80 Disassembler
 * Copyright (C) 1990-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * dz80.c - Main File
 *
 * Version 3.4.1 - 2007/09/02
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "mdz80.h"
#include "analyze.h"
#include "pass1.h"
#include "pass2.h"
#include "opcodes_z80.h"
#include "opcodes_8080.h"
#include "zmac.h"

//
// Global variables
//
int	d8080;				// 0 for z80, 1 for 8080
int	dotpseudo = 0;
char	src[FN_LEN], dst[FN_LEN];	// file name buffers
char	app[FN_LEN];
char	esrc[FN_LEN];			// asm equate source file
char	baseFileName[FN_LEN];		// source file name without extension
char	ctl[FN_LEN];			// control file name
char	linebuffer[MAX_LINE];		// input line buffer
FILE	* fp;				// input file struct
int	hexflag;			// append hex flag
int	fileflag;			// file type flag
int	upperflag;			// upper case output flag
int	baseflag;
int	traceflag;			// trace and analyze code
int	prsmcode;			// presume code flag
int	verbose;			// verbose flag
int	altflag;			// alternate hex format
int	equflag;			// process equates/asm file
int	kcnt;				// output char counter
int	pc;				// current program counter
int	himark;				// highest data adrs
int	offset;				// program counter offset
byte	* pgmmem;			// program data pointer
int	* pgmflags;			// pointer to program flags

#ifdef	EXTENDED_MEM
byte	* epgmmem[EXT_PGM_SEGS];	// extended program memory pointers
int	* epgmflags[EXT_PGM_SEGS];	// extended program flags pointers
#endif

char	string[ASCLIMIT];		// ascii data for defb
int	asc_cnt;			// count for string data
byte	byte_data[BYTELIMIT];		// binary data for defb
int	byte_cnt;			// count for binary data
int	word_data[WORDLIMIT];		// binary data for defw
int	word_cnt;			// count for word data
byte	dump;				// dump just done flag
byte	ascii_flag;			// use ascii string flag
char	defbstr[8];			// string for defined bytes
char	defwstr[8];			// string for defined words
char	ascistr[8];			// string for defined ascii
char	orgstr[8] = "ORG";		// org pseudo-op string
char	equstr[8] = "EQU";		// equ pseudo-op string

SYM_PTR sym_tab;			// symbol table pointer
SYM_PTR lab_tab;			// label table pointer
SYM_PTR name_tab;			// operand names pointer
SYM_PTR sym_tab_last;			// last symbol table pointer
SYM_PTR lab_tab_last;			// lastlabel table pointer
SYM_PTR name_tab_last;			// last name table pointer
int	symbol_count;			// number of symbols
int	label_count;			// number of labels
int	name_count;			// number of operand names

SYM_PTR	* sym_val_index;		// array of pointers
SYM_PTR	* lab_val_index;		//  for binary search
SYM_PTR	* name_val_index;
SYM_PTR tail_ptr, head_ptr;		// sort pointers

COMMENT_PTR comment_list;		// header comment list
COMMENT_PTR icomment_list;		// inline comment list
COMMENT_PTR patch_list;			// patch list

int	newline;			// just output newline flag
struct tm	*date_time;		// disassembly time

bool	cycleflag;
bool	cycle_exclude;
bool	cycle_alwaystake;
CYCLE_RANGE_PTR	cycle_r;		// cycles
CYCLE_RANGE_PTR	cycle_current;

char licenseText[] = "\nReleased under the GNU General Public License Version 3\n";

void usage( void )
{
	printf( "\nUsage: mdz80 [options] <filename> [-o <ofile>] [-c <cfile>] [-e <efile>]\n"
	        "\nFile control options:\n"
	        "\t-o use 'ofile' as output.\n"
	        "\t-c use 'cfile' as control.\n"
	        "\t-C disassemble CP/M .com file (implies -x100).\n"
	        "\t-b force .bin format/extension on input file.\n"
	        "\t-h force .hex format/extension on input file.\n"
	        "\t   If neither 'b', 'c', nor 'h' is specified, mdz80 will first search\n"
	        "\t   for a .hex file, then a .bin file and then the filename alone.\n"
	        "\t-t trace and analyze code before disassembly.\n"
	        "\t-e use asm source 'efile' to add/evaluate symbols\n"
	        "\t   (Altoghether with -t option will create/modify control file).\n"
	        "\nDisassembling options:\n"
	        "\t-a use ascii macro instead of db/defb for text.\n"
	        "\t-d include address and data in comment field.\n"
	        "\t-n use C style for hexadecimal operands\n"
	        "\t-f format hex values as '$xx' instead of xxH.\n"
	        "\t-p put dot '.' at beginning of pseudo ops\n"
	        "\t-s change 'defb' and 'defw' to 'db' and 'dw'.\n"
	        "\t-l output labels, symbols, and mnemonics in lower case.\n"
	        "\t-x [nnnn] add hexadecimal offset to program addresses.\n"
	        "\t-8 generate 8080 mnemonics.\n"
	        "\t-T during trace (-t) presume unidentified binary is code\n"
	        "\nGeneral options:\n"
	        "\t-v be verbose.\n"
	        "\t-V print version and exit.\n"
	        "\nOptions are case sensitive and may be entered in a freeform\n"
	        "fashion as long as a dash (-) preceeds any option (group)."
	        "\nExamples:\n"
	        "\tmdz80 -bd filename\n"
	        "\tmdz80 -d filename -b -o out_file\n\n" );
	exit( GOOD_EXIT );
}

void version()
{
	printf( "\nMDZ80 Z80/8080 Disassembler v %d.%d.%d"
#ifdef ALPHA
	        "-alpha %d"
#else
#ifdef BETA
	        "-beta %d"
#endif
#endif
	        "\nCopyright (C) %d by P. Betti <pbetti@lpconsul.net>\n"
	        "Original copyrights: (C) 1990-2007 by J. L. Post for D52 code\n"
	        "Bruce Norskog, Tim Mann, Mark Rison, Thierry Jouin and others for ZMAC code%s\n",
	        DVERSION, MAJORREV, MINORREV,
#ifdef ALPHA
	        ALPHA,
#else
#ifdef BETA
	        BETA,
#endif
#endif
	        YEAR, licenseText );
}

//
//  The Main Program
//

int main( int argc, char *argv[] )
{
	char	c;
	int	count;
	char	*inp;
	int	line;
	char	tempstr[16];

	if ( argc < 2 )
		usage();

	strcpy( defbstr, "defb" );		// init define byte and word strings
	strcpy( defwstr, "defw" );
	strcpy( ascistr, "defb" );		// init define ascii string

	sym_tab = NULL;				// no symbols or labels yet
	lab_tab = NULL;
	name_tab = NULL;
	fp = NULL;
	fileflag = EITHERFILE;			// assume search for either file type
	hexflag = FALSE;			// no data in comment field
	upperflag = TRUE;
	altflag = FALSE;
	baseflag = FALSE;
	traceflag = FALSE;
	prsmcode = FALSE;
	offset = 0;				// default start at address 0
	ascii_flag = FALSE;
	equflag = FALSE;
	d8080 = 0;
	line = 0;
	baseFileName[0] = '\0';

// process command line options

	for ( count = 1; count < argc; count++ ) {
		inp = argv[count];				// to avoid modifying pointer in argv
		c = *inp++;

		if ( c != '-' && c != '/' &&
				c != '?' && line == 0 ) {	/* not an option */
			line = count;
			strcpy( src, argv[line] );		/* init filenames */
			strip_ext( src );			/* clean extensions (if any) */
			strcpy( baseFileName, src );
			strcpy( dst, src );
			strcat( dst, ".z80" );			/* by default... */
			strcpy( ctl, src );
			strcat( ctl, ".ctl" );
			strcpy( src, argv[line] );
			continue;
		}

		while ( c ) {
			if ( c == '-' || c == '/' )
				c = *inp++;			// skip over option specifier

			if ( c == '?' )
				usage();

			/* there can be only one...*/
			if ( count == line )			// skip if already identified
				break;				// as the file name

			if ( c == '8' ) {
				d8080 = 1;
				strcpy( dst, baseFileName );
				strcat( dst, ".d80" );
				// use ascii macro
			} else if ( c == 'a' ) {
				strcpy( ascistr, "ascii" );
				ascii_flag = TRUE;
				// binary instead of hex file
			} else if ( ( c == 'b' ) && ( fileflag == EITHERFILE ) ) {
				fileflag = BINFILE;
				/*					strcpy( src, baseFileName );
									strcat( src, ".bin" );*/
				// CP/M .com file
			} else if ( c == 'C' && fileflag == EITHERFILE ) {
				fileflag = CPMFILE;
				strcpy( src, baseFileName );
				strcat( src, ".com" );
				offset = 0x100;
				// add data in comment field
			} else if ( c == 'd' )
				hexflag = TRUE;
			// force search for hex file
			else if ( ( c == 'h' ) && ( fileflag == EITHERFILE ) ) {
				fileflag = HEXFILE;
				//strcpy( src, baseFileName );
				//strcat( src, ".hex" );
			} else if ( c == 'n' ) {
				if ( altflag ) {
					printf( "You can't specify both -n and -f options.\n" );
					exit ( GOOD_EXIT );
				}
				baseflag = TRUE;
				strcpy( mnemtbl[0xd7].mnem, "RST 0x10" );
				strcpy( mnemtbl[0xdf].mnem, "RST 0x18" );
				strcpy( mnemtbl[0xe7].mnem, "RST 0x20" );
				strcpy( mnemtbl[0xef].mnem, "RST 0x28" );
				strcpy( mnemtbl[0xf7].mnem, "RST 0x30" );
				strcpy( mnemtbl[0xff].mnem, "RST 0x38" );
			} else if ( c == 'p' )
				dotpseudo = 1;
			// change db/dw strings
			else if ( c == 's' ) {
				strcpy( defbstr, "db" );
				strcpy( defwstr, "dw" );
				strcpy( ascistr, "db" );
			} else if ( c == 't' )
				traceflag = TRUE;
			else if ( c == 'l' )
				upperflag = FALSE;
			// add hex offset to program
			else if ( c == 'x' ) {
				offset = atox( inp );
				break;
			}
			// output file
			else if ( c == 'o' ) {
				if ( ++count < argc ) {
					strcpy( dst, argv[count] );
				} else
					usage();
			}
			// control file
			else if ( c == 'c' ) {
				if ( ++count < argc ) {
					strcpy( ctl, argv[count] );
				} else
					usage();
			}
			// verbose
			else if ( c == 'v' ) {
				++verbose;
				printf("Verbose mode.\n");
			}
			// alternate hex format
			else if ( c == 'f' ) {
				++altflag;
			}
			// verbose
			else if ( c == 'V' ) {
				version();
				exit( GOOD_EXIT );
			}
			// equate/asm file
			else if ( c == 'e' ) {
				if ( ++count < argc ) {
					strcpy( esrc, argv[count] );
					equflag = TRUE;
				} else
					usage();
			}
			// presume code in analysis
			else if ( c == 'T' ) {
				prsmcode = TRUE;
			}

			c = *inp++;
		}
	}

	if (!traceflag) prsmcode = FALSE;

	if (strlen(baseFileName) == 0) {
		printf("No input filename specified.\n");
		exit(FILE_ERROR);
	}

	if ( dotpseudo ) {
		strcpy( tempstr, defbstr );
		defbstr[0] = '.';
		defbstr[1] = 0;
		strcat( defbstr, tempstr );

		strcpy( tempstr, defwstr );
		defwstr[0] = '.';
		defwstr[1] = 0;
		strcat( defwstr, tempstr );

		strcpy( tempstr, ascistr );
		ascistr[0] = '.';
		ascistr[1] = 0;
		strcat( ascistr, tempstr );

		strcpy( tempstr, equstr );
		equstr[0] = '.';
		equstr[1] = 0;
		strcat( equstr, tempstr );

		strcpy( tempstr, orgstr );
		orgstr[0] = '.';
		orgstr[1] = 0;
		strcat( orgstr, tempstr );
	}

	if (verbose) {
		printf("\n");
		if (upperflag == FALSE) printf("Lowering output case.\n");
		if (altflag == TRUE) printf("Using $xx hex format.\n");
		if (baseflag == TRUE) printf("Using C-style hex format.\n");
		if (dotpseudo == TRUE) printf("Prefixing defs with a dot.\n");
		if (traceflag == TRUE) printf("Tracing and analyze mode is ON.\n");
		if (prsmcode == TRUE) printf("... and presume code for unidentified data.\n");
		if (d8080 == TRUE) printf("Using 8080 mnemonics for output.\n");
		if (hexflag == TRUE) printf("Including address and data in comments.\n");
		if (ascii_flag == TRUE) printf("Using ASCII macros.\n");
		if (fileflag == CPMFILE) printf("CP/M mode is ON.\n");
		if (strcmp(defbstr, "db") == 0) printf("Using short form for defs.\n");
		if (offset > 0) printf("Adding offset %04X to source.\n", offset);
		if (equflag == TRUE) printf("Processing symbols from asm source [%s]\n", esrc);
		printf("\n");
	}
// allocate memory for program and flags

	if ( !init_memory() )
		exit( MEM_ERROR );

	getCTLoffset();		// get offset value from control file, if any
	readfile( src );	// read disassembly file

// Got the program in data array, now let's go to work...

	symbol_count = 0;
	label_count = 0;
	name_count = 0;
	comment_list = NULL;
	icomment_list = NULL;
	patch_list = NULL;

	if ( traceflag ) {
		if (verbose) printf( "\nAnalyzing code..." );

		if ( analyzeCode( "MDZ80" ) ) {
			if (verbose) printf( "done\n" );
		}
		else
			printf( "\nAnalysis error!\n" );
	}

	if (equflag) {
		if (verbose) printf( "\nProcessing equate/source file..." );

		if ( !zmac( esrc, ctl, 0, 0 ) ) {
			if (verbose) printf( "done\n" );
		}
		else
			printf( "\nAssembler error!\n" );
	}

	pass0();			// read control file
	pass1();			// find internal references
	pass2();			// disassemble to output file
	pass3();			// generate equ's for references
	printf( "\nDone\n\n" );		// go bye-bye
	return( GOOD_EXIT );
}	//  End of Main

//
// Strip extension from filename
//

char * strip_ext( char * fname )
{
	int name_len = strlen( fname );
	char * p = fname + name_len - 4;

	if ( name_len < 4 )		/* too short */
		return fname;

	if ( ! strcasecmp( p, ".hex" ) ) {
		*p = '\0';
		return fname;
	}
	if ( ! strcasecmp( p, ".bin" ) ) {
		*p = '\0';
		return fname;
	}

	return fname;
}

//
// File open. Try with/without extensions
//

FILE * ext_open( char * fname, char * hex_e, char * bin_e )
{
	FILE * fp;

	if ( verbose ) printf( "Input basename [%s]\n", fname );

	/* no extension */
	strcpy( app, fname );
	fp = fopen( app, "rb" );
	if ( fp != NULL ) {
		fileflag = BINFILE;
		return fp;
	}

	/* hex extension */
	if ( hex_e ) {
		strcpy( app, fname );
		strcat( app, hex_e );
		if ( verbose ) printf( "Trying .hex [%s]\n", app );
		fp = fopen( app, "r" );
		if ( fp != NULL ) {
			fileflag = HEXFILE;
			strcpy( src, app );
			return fp;
		}
	}

	/* bin extension */
	if ( bin_e ) {
		strcpy( app, fname );
		strcat( app, bin_e );
		if ( verbose ) printf( "Trying .bin [%s]\n", app );
		fp = fopen( app, "rb" );
		if ( fp != NULL ) {
			fileflag = BINFILE;
			strcpy( src, app );
			return fp;
		}
	}

	return 0;	/* ouch ! */
}



bool init_memory( void )
{
	int	count;

#ifdef EXTENDED_MEM
	for ( count = 0; count < EXT_PGM_SEGS; count++ ) {
		epgmmem[count] = NULL;
		epgmflags[count] = NULL;
	}
#endif

	if ( ( pgmmem = ( byte * ) malloc( PMEMSIZE ) ) == NULL ) {
		printf( "INTERNAL ERROR! - Can't allocate program space!\n" );
		return FALSE;
	}

	if ( ( pgmflags = ( int * ) malloc( PMEMSIZE * sizeof( int ) ) ) == NULL ) {
		printf( "INTERNAL ERROR! - Can't allocate flag space!\n" );
		return FALSE;
	}

	printf( "Initializing program spaces...\n" );

	for ( count = WORD_MASK; count; count-- ) {	// fill code space with
		pgmmem[count] = NO_DATA;		// invalidate data
		pgmflags[count] = PF_NOINIT;		// invalidate flags
	}

	pgmmem[0] = NO_DATA;				// include first location
	pgmflags[0] = PF_NOINIT;
	return TRUE;
}

#ifdef	EXTENDED_MEM

// Get extended program or flag memory.
// Width = 1 for program memory or 2 for flag memory.

byte *get_extended_mem( int width )
{
	byte	*ptr;

	ptr = ( byte * ) malloc( PMEMSIZE * width );

	if ( !ptr )
		printf( "\nCan't allocate extended memory!\n" );

	return ptr;
}
#endif

char * makeupper( char *str )
{
	char	*ret = str;

	while ( *str ) {
		*str = toupper( *str );
		str++;
	}

	return ret;
}

//
// Read bin, hex, or com file into program memory.
//

int readfile( char *filename )
{
	int	i, j, rectype, page, line, readsize;

// open source file

	switch ( fileflag ) {
	case EITHERFILE:				// if no type specified...
		fp = ext_open( filename, ".hex", ".bin" );
		if ( fp == NULL ) {			// if not found, search for bin file
			printf( "* Can't open either '%s.hex' nor '%s.bin' nor without extension *\n",
			        baseFileName, baseFileName );
			exit( FILE_ERROR );
		}
		break;

	case HEXFILE:					// force hex file
		fp = ext_open( filename, ".hex", 0 );
		break;

	case CPMFILE:					// CP/M file
	case BINFILE:					// force bin file
		fp = ext_open( filename, 0, ".bin" );
		break;
	}
	if ( fp == NULL ) {					// if file not found...
		printf( "* Can't open file '%s' *\n", filename );
		exit( FILE_ERROR );
	}

// read input file and set up data array

	himark = 0;
	line = 0;
	pc = offset;
	printf( "\nreading %s\n", src );
	readsize = MAX_LINE;

	if ( fileflag == BINFILE || fileflag == CPMFILE ) {		// if binary file...
		while ( !feof( fp ) ) {					// until end of file...
			i = fread( linebuffer, 1, readsize, fp );	// read a block of data

			if ( !i )
				break;

			for ( j = 0; j < i; j++ ) {
				if ( pc >= PMEMSIZE ) {			// exceeded 64K limit
					printf( "\nInput file too large!\n\n" );
					break;
				}

				pgmmem[pc] = linebuffer[j];		// copy to program space
				pgmflags[pc] = PF_DATA;
				pc++;

				if ( ( pc & 0xff ) == 0 )
					printf( "\r%04X", pc );		// show progress
			}

			if ( pc & WORD_MASK )
				himark = pc;
			else
				himark = WORD_MASK;			// flag highest location
		}
	}

	else {								// else hex file...
		page = 0;
		rectype = 0;

		while ( !feof( fp ) ) {					// until end of file...
			*linebuffer = '\0';				// clear previous line
			fgets( linebuffer, MAX_LINE - 1, fp );		// read one line
			line++;
			// get count and address
			if ( sscanf( linebuffer, "%*c%2x%4x%2x", &i, &pc, &rectype ) != EOF ) {
				pc += offset;				// add offset to address
				pc += page;				// add segment to address

				if ( rectype == 1 )
					break;				// done if end of hex record

				if ( ( pc + i ) > PMEMSIZE ) {
					printf( "\nInput file too large!\n\n" );
					break;
				}

				if ( rectype == 2 ) {			// extended segment address record
					sscanf( ( char * ) &linebuffer[9], "%4x", &page );
					page <<= 4;
				} else if ( rectype == 0 ) {		// data record
					if ( i > 64 ) {			// oops! line too long
						printf( "invalid count (%d) in line %d:\n", i, line );
						printf( "%s", linebuffer );
						exit( FILE_ERROR );
					}

					for ( j = 0; j < i ; j++ ) {	// now move data to program area
									// code to program space
						getcode( linebuffer + 9 + j * 2, &pgmmem[pc] );
						pgmflags[pc] = PF_DATA;		// flag valid data
						pc++;

						if ( ( pc & 0xff ) == 0 )	// show progress
							printf( "\r%04X", pc );
					}

					if ( pc > himark )
						himark = pc;
				}
			}
		}
	}

	if ( himark >= PMEMSIZE )
		himark = PMEMSIZE - 1;

	fclose( fp );				// done reading input file
	printf( "\rHighest location = " );	// show last location

	if ( himark == WORD_MASK )
		printf( "%04X\n", himark );
	else
		printf( "%04X\n", himark - 1 );

	return 0;
}

// Read control file and search for Offset directive

void getCTLoffset( void )
{
	FILE	*fpc;
	char	*inp;

	if ( !offset && !traceflag ) {	// get possible offset from control file
		fpc = fopen( ctl, "r" );

		if ( fpc ) {
			while ( !feof( fpc ) ) {
				inp = fgets( linebuffer, MAX_LINE - 1, fpc );

				if ( !inp )
					break;

				if ( toupper( *inp ) == 'O' ) {
					inp++;

					while ( *inp != ' ' && *inp != '\t' )
						inp++;

					get_adrs( inp, &offset );
					break;
				}
			}

			fclose( fpc );
		}
	}
}

//
//	Put ascii hex data into binary array
//

void getcode( char *from, byte *loc )
{
	byte	c, i;

	c = *from++ - 0x30;
	c = ( c > 10 ) ? c - 7 : c;
	i = c << 4;
	c = *from++ - 0x30;
	c = ( c > 10 ) ? c - 7 : c;
	*loc = i | c;
}

//
// Get hexadecimal number from line in control file.
// Return updated character pointer.
//

char *get_adrs( char *text, int *val )
{
	int	result, start;
	char	c;

	result = start = 0;
	c = toupper( *text );

	while ( c ) {
		if ( c == ';' )				// beginning of comment, ignore all else
			break;

		if ( c == '\n' )			// necessary because isspace() includes \n
			break;

		if ( isspace( c ) ) {			// skip leading whitespace
			text++;

			if ( start )			// if result already begun...
				break;
		} else if ( isxdigit( c ) ) {
			start = 1;			// flag beginning of result conversion
			c = ( c > '9' ) ? c - 0x37 : c - 0x30;
			result <<= 4;
			result |= ( ( int ) c & 0xf );
			text++;
		} else if ( ( c == 'X' ) &&
			    !result && ( start == 1 ) ) {	// allow for prefix "0x"
			start++;
			text++;
		} else					// done if not hexadecimal character
			break;

		c = toupper( *text );			// get next digit
	}

	*val = result;		// pass number back to caller
	return( text );		// and return updated text pointer
}

void error( char *str1, char *str2 )			// fatal error trap
{
	printf( "\n%s%s", str1, str2 );
	exit( FILE_ERROR );
}

//	Sort label or symbol table
//	First sort by name so that we can check for duplicates,
//	then sort by value, check for duplicates, and set up
//	pointer array for binary search.
//

SYM_PTR sort( SYM_PTR list, SYM_PTR *array, int count )
{
	int	i;
	struct sym	*sptr, *temp;

	sptr = sort_by_name( list );

	if ( list == name_tab )
		chk_dup_op_name( sptr, count );
	else
		chk_dup_name( sptr, count );

	sptr = sort_by_value( sptr );

	if ( list == name_tab )
		chk_dup_op_value( sptr, count );
	else
		chk_dup_value( sptr, count );

	temp = sptr;

	for ( i = 0; i < count; i++ ) {	// set up array of pointers sorted by value
		array[i] = temp;
		temp = temp->next;
	}

	return( sptr );
}

//
// In-place non-recursive merge sort using label text as key
//

SYM_PTR sort_by_name( SYM_PTR list )
{
	int			i, n;
	SYM_PTR a, b, todo, t;

	head_ptr = ( SYM_PTR  ) malloc( sizeof( SYM ) );
	head_ptr->next = list;
	a = tail_ptr;

	for ( n = 1; a != head_ptr->next; n = n + n ) {
		todo = head_ptr->next;
		list = head_ptr;

		while ( todo != tail_ptr ) {
			t = todo;
			a = t;

			for ( i = 1; i < n; i++ )
				t = t->next;

			b = t->next;
			t->next = tail_ptr;
			t = b;

			for ( i = 1; i < n; i++ )
				t = t->next;

			todo = t->next;
			t->next = tail_ptr;
			list->next = merge_by_name( a, b );

			for ( i = 1; i <= n + n; i++ )
				list = list->next;
		}
	}

	return( head_ptr->next );
}

//
// In-place non-recursive merge sort using value as key
//

SYM_PTR sort_by_value( SYM_PTR list )
{
	int i, n;
	SYM_PTR a, b, todo, t;

	head_ptr = ( SYM_PTR  ) malloc( sizeof( SYM ) );
	head_ptr->next = list;
	a = tail_ptr;

	for ( n = 1; a != head_ptr->next; n = n + n ) {
		todo = head_ptr->next;
		list = head_ptr;

		while ( todo != tail_ptr ) {
			t = todo;
			a = t;

			for ( i = 1; i < n; i++ )
				t = t->next;

			b = t->next;
			t->next = tail_ptr;
			t = b;

			for ( i = 1; i < n; i++ )
				t = t->next;

			todo = t->next;
			t->next = tail_ptr;
			list->next = merge_by_value( a, b );

			for ( i = 1; i <= n + n; i++ )
				list = list->next;
		}
	}

	return( head_ptr->next );
}

//
// Merge sub-lists by text field
//

SYM_PTR merge_by_name( SYM_PTR a, SYM_PTR b )
{
	int i;
	SYM_PTR c;

	c = tail_ptr;

	do {
		i = strcasecmp( a->name, b->name );

		if ( i <= 0 ) {
			c->next = a;
			c = a;
			a = a->next;
		} else {
			c->next = b;
			c = b;
			b = b->next;
		}
	} while ( c != tail_ptr );

	c = tail_ptr->next;
	tail_ptr->next = tail_ptr;
	return( c );
}

//
// Merge sub-lists by value field
//

SYM_PTR merge_by_value( SYM_PTR a, SYM_PTR b )
{
	SYM_PTR c;

	c = tail_ptr;

	do {
		if ( a->val < b->val ) {
			c->next = a;
			c = a;
			a = a->next;
		} else {
			c->next = b;
			c = b;
			b = b->next;
		}
	} while ( c != tail_ptr );

	c = tail_ptr->next;
	tail_ptr->next = tail_ptr;
	return( c );
}

//
// Check for redefinitions of label/symbol names
//

void chk_dup_name( SYM_PTR list, int count )
{
	int i;

	for ( i = 0; i < count; i++ ) {
		if ( !strcasecmp( list->name, list->next->name ) ) {
			printf( "\nAttempted redefinition of name (%s), value 0x%X,"
			        " as value 0x%X.\n",
			        list->name, list->val, list->next->val );
			exit( USER_ERROR );
		}

		list = list->next;
	}
}

//
// Check for redefinitions of operand names
//

void chk_dup_op_name( SYM_PTR list, int count )
{
	int i, adrs, val, next_adrs, next_val;

	for ( i = 0; i < count; i++ ) {
		if ( !strcasecmp( list->name, list->next->name ) ) {
			adrs = list->val;
			val = pgmmem[adrs];

			if ( pgmflags[adrs] & PF_WORD ) {
				val <<= 8;
				val |= pgmmem[adrs + 1];
			}

			next_adrs = list->next->val;
			next_val = pgmmem[next_adrs];

			if ( pgmflags[next_adrs] & PF_WORD ) {
				next_val <<= 8;
				next_val |= pgmmem[next_adrs + 1];
			}

			if ( val != next_val ) {
				printf( "\nAttempted redefinition of operand name (%s) at address 0x%X, value 0x%X,"
				        "\n as value 0x%X, previously defined at address 0x%X.\n",
				        list->name, next_adrs, val, next_val, adrs );
				exit( USER_ERROR );
			}
		}

		list = list->next;
	}
}

//
// Check for redefinitions of values
//

void chk_dup_value( SYM_PTR list, int count )
{
	int i;

	for ( i = 0; i < count; i++ ) {
		if ( list->val == list->next->val ) {
			printf( "\nAttempted redefinition of value 0x%X, '%s', as '%s'.\n",
			        list->val, list->name, list->next->name );
			exit( USER_ERROR );
		}

		list = list->next;
	}
}

//
// Check for redefinitions of operand values
//

void chk_dup_op_value( SYM_PTR list, int count )
{
	int i;

	for ( i = 0; i < count; i++ ) {
		if ( list->val == list->next->val ) {
			if ( strcasecmp( list->name, list->next->name ) ) {
				printf( "\nAttempted redefinition of operand 0x%X at 0x%X, (%s), as '%s'.\n",
				        pgmmem[list->val], list->val, list->name, list->next->name );
				exit( USER_ERROR );
			}
		}

		list = list->next;
	}
}

// Find symbol or label by value
//
// Binary search using table of pointers to list nodes.
// This search is based on the fact that:
// list[first-1] <= key < list[last+1]
// is an invariant. This allows the while loop to involve
// only one boolean expression, rather than two. The 'if'
// statement also involves only one boolean expression.
// The test for equality (has it been found?) is not done
// until the loop is complete. This significantly speeds
// up the search compared to a standard binary search.
//

char * find_entry( int val, int count, SYM_PTR *table )
{
	SYM_PTR ptr;
	int first, mid, last;
	char * ret;

	first = 1;
	last = count;

	while ( first <= last ) {			// while more to search...
		mid = ( first + last ) >> 1;		// begin in middle of remaining array
		ptr = table[mid - 1];			// get pointer to node

		if ( ptr->val > val )			// determine which way to go
			last = mid - 1;			// search before midpoint
		else
			first = mid + 1;		// search after midpoint
	}

	ret = NULL;					// assume not found

	if ( last > 0 ) {				// are we still within the array?
		ptr = table[last - 1];			// if so, get last pointer

		if ( val == ptr->val ) {		// is it what we're searching for?
			if ( ptr->used == FALSE )
				ptr->used = TRUE;	// label/symbol has been used

			ret = ptr->name;		// return pointer to caller
		}
	}

	return( ret );
}

//
// Find symbol or label by name
// Table must be sorted by name
//

SYM_PTR find_name( char *name, int count, SYM_PTR *table )
{
	int i;
	SYM_PTR ptr;
	int first, mid, last;

	first = 1;
	last = count;

	while ( first <= last ) {			// while more to search...
		mid = ( first + last ) >> 1;		// begin in middle of remaining array
		ptr = table[mid - 1];			// get pointer to node

		i = strcasecmp( ptr->name, name );

		if ( i > 0 )
			last = mid - 1;			// search before midpoint
		else
			first = mid + 1;		// search after midpoint
	}

	ptr = NULL;					// assume not found

	if ( last > 0 ) {				// are we still within the array?
		ptr = table[last - 1];			// if so, get last pointer

		if ( strcasecmp( name, ptr->name ) )
			ptr = NULL;
	}

	return( ptr );
}

//
// Allocate new entry in symbol table or label table
//

SYM_PTR get_smem( int type, int req_size )
{
	SYM_PTR ptr;

	ptr = ( SYM_PTR ) malloc( sizeof( SYM ) + req_size + 1 );
	// get memory from OS
	if ( ptr == NULL ) {				// what? out of memory?...
		printf( "\nINTERNAL ERROR! - No memory for " );

		switch ( type ) {
		case LABEL_TYPE:
			printf( "label" );
			break;

		case SYMBOL_TYPE:
			printf( "symbol" );
			break;

		case NAME_TYPE:
			printf( "name" );
			break;

		default:
			printf( "\nUnknown table type: %d\n", type );
			exit( PROGRAM_ERROR );
			break;
		}

		printf( " table!\n" );
		exit( MEM_ERROR );
	}

	ptr->next = NULL;
	return( ptr );					// give caller the address
}

//
// Add symbol or label to table
//

SYM_PTR add_entry( int val, char *symbol, int type )
{
	SYM_PTR nptr, tbl_ptr;
	char * cptr;
	int isquote = 0;
	char tbl_name[8];

	cptr = symbol;						// ensure that input string is null terminated

	if ( ( *symbol == '\'' ) || ( *symbol == '"' ) ) {	// quoted character or string
		isquote = *symbol;
		cptr++;
	}

	while ( *cptr ) {
		if ( !isgraph( *cptr ) ) {			// if not a visible character...
			if ( !isquote )				// and not in quoted char or string
				break;				// we're done
		} else if ( isquote ) {				// else if in quoted char or string...
			if ( isquote == *cptr )			// if end of quote...
				isquote = 0;			// terminate at next non-visible character
		}

		cptr++;
	}

	*cptr = '\0';

	if ( upperflag )
		makeupper( symbol );

	switch ( type ) {
	case LABEL_TYPE:
		tbl_ptr = lab_tab;				// get pointer to correct table
		strcpy( tbl_name, "label" );
		label_count++;
		break;

	case SYMBOL_TYPE:
		tbl_ptr = sym_tab;
		strcpy( tbl_name, "symbol" );
		symbol_count++;
		break;

	case NAME_TYPE:
		tbl_ptr = name_tab;
		strcpy( tbl_name, "name" );
		name_count++;
		break;

	default:
		printf( "\nUnknown table type: %d\n", type );
		exit( PROGRAM_ERROR );
		break;
	}

	nptr = get_smem( type, strlen( symbol ) );

	if ( tbl_ptr == NULL ) {				// if first symbol or label...
		switch ( type ) {
		case LABEL_TYPE:
			lab_tab = nptr;
			break;

		case SYMBOL_TYPE:
			sym_tab = nptr;
			break;

		case NAME_TYPE:
			name_tab = nptr;
			break;

		default:
			printf( "\nUnknown table type: %d\n", type );
			exit( PROGRAM_ERROR );
			break;
		}
	} else {
		switch ( type ) {
		case LABEL_TYPE:
			lab_tab_last->next = nptr;
			break;

		case SYMBOL_TYPE:
			sym_tab_last->next = nptr;
			break;

		case NAME_TYPE:
			name_tab_last->next = nptr;
			break;

		default:
			printf( "\nUnknown table type: %d\n", type );
			exit( PROGRAM_ERROR );
			break;
		}
	}

	switch ( type ) {
	case LABEL_TYPE:
		lab_tab_last = nptr;
		break;

	case SYMBOL_TYPE:
		sym_tab_last = nptr;
		break;

	case NAME_TYPE:
		name_tab_last = nptr;
		break;

	default:
		printf( "\nUnknown table type: %d\n", type );
		exit( PROGRAM_ERROR );
		break;
	}

	nptr->used = FALSE;
	nptr->val = val;						// set value in new entry
	nptr->name = malloc( strlen( symbol ) + 1 );

	if ( !nptr->name ) {
		printf( "\nCan't allocate memory for " );

		switch ( type ) {
		case LABEL_TYPE:
			printf( "label: %s\n", symbol );
			break;

		case SYMBOL_TYPE:
			printf( "symbol: %s\n", symbol );
			break;

		case NAME_TYPE:
			printf( "name: %s\n", symbol );
			break;

		default:
			printf( "Unknown table type: %d: %s\n", type, symbol );
			break;
		}

		exit( MEM_ERROR );
	}

	strcpy( nptr->name, symbol );	// and copy text to entry
	return nptr;
}

//
// Output header comment(s) associated with 'adrs'
// Expand newlines
//

void output_comment( int adrs )
{
	COMMENT_PTR cmt;
	char * str;

	cmt = comment_list;

	while ( cmt )				// search through list for all
	{					// entries that match adrs
		if ( cmt->adrs == adrs ) {
			str = cmt->str;
			fprintf( fp, "\n;" );

			if ( strlen( str ) > 3 && str[2] == '\\' && str[3] == 'n' ) {
				fprintf( fp, "\n; " );
				str += 4;
			} else
				str++;

			while ( *str ) {
				if ( *str == '\\' && *( str + 1 ) && ( *( str + 1 ) == 'n' ) ) {
					str++;
					fprintf( fp, "\n;" );

					if ( *( str + 1 ) )
						fputc( ' ', fp );
				} else
					fputc( *str, fp );

				str++;
			}
		}

		cmt = cmt->next;
	}
}

//
// Output inline comment associated with 'adrs'
//

void output_icomment( int adrs )
{
	COMMENT_PTR cmt;

	cmt = icomment_list;

	while ( cmt )				// search through list for all
	{					// entries that match adrs
		if ( cmt->adrs == adrs )
			fprintf( fp, "\t%s", cmt->str );

		cmt = cmt->next;
	}
}

//
// Output patch string(s) associated with 'adrs'
// Expand newlines
//

void output_patch( int adrs )
{
	COMMENT_PTR patch;
	char * str;

	patch = patch_list;

	while ( patch )				// search through list for all
	{					// entries that match adrs
		if ( patch->adrs == adrs ) {
			str = patch->str;
			fprintf( fp, "\n" );

			while ( *str ) {
				if ( *str == '\\' && *( str + 1 ) && ( *( str + 1 ) == 'n' ) ) {
					str++;
					fprintf( fp, "\n" );
				} else
					fputc( *str, fp );

				str++;
			}
		}

		patch = patch->next;
	}
}

//
// Add comment string to linked list in memory
//

void add_comment( int adrs, char *str )
{
	int len;
	char * ctext;
	COMMENT_PTR cmt_ptr;

	len = strlen( str ) - 1;

	if ( len >= 0 && ( str[len] == '\n' || str[len] == '\r' ) )
		str[len] = '\0';					// get rid of newline

	if ( !comment_list ) {						// first comment
		comment_list = malloc( sizeof( struct comment ) );

		if ( comment_list == NULL ) {
			printf( "\nNo memory for comment struct" );
			exit( MEM_ERROR );
		}

		cmt_ptr = comment_list;
	} else {							// add comment to list
		cmt_ptr = comment_list;

		while ( cmt_ptr->next )					// find end of list
			cmt_ptr = cmt_ptr->next;

		cmt_ptr->next = malloc( sizeof( struct comment ) );

		if ( cmt_ptr->next == NULL ) {
			printf( "\nNo memory for comment struct" );
			exit( MEM_ERROR );
		}

		cmt_ptr = cmt_ptr->next;
	}

	cmt_ptr->adrs = adrs;
	ctext = malloc( strlen( str ) + 3 );

	if ( ctext == NULL ) {
		printf( "\nNo memory for comment string" );
		exit( MEM_ERROR );
	}

	cmt_ptr->str = ctext;

	if ( len >= 0 ) {
		strcpy( ctext, "; " );
		strcat( ctext, str );
	} else
		strcpy( ctext, ";" );

	cmt_ptr->next = NULL;
}

void add_icomment( int adrs, char *str )
{
	int len;
	char * ctext;
	COMMENT_PTR	cmt_ptr;

	len = strlen( str ) - 1;

	if ( str[len] == '\n' || str[len] == '\r' )
		str[len] = '\0';					// get rid of newline

	if ( !icomment_list ) {						// first comment
		icomment_list = malloc( sizeof( struct comment ) );

		if ( icomment_list == NULL ) {
			printf( "\nNo memory for comment struct" );
			exit( MEM_ERROR );
		}

		cmt_ptr = icomment_list;
	} else {							// add comment to list
		cmt_ptr = icomment_list;

		while ( cmt_ptr->next )					// find end of list
			cmt_ptr = cmt_ptr->next;

		cmt_ptr->next = malloc( sizeof( struct comment ) );

		if ( cmt_ptr->next == NULL ) {
			printf( "\nNo memory for comment struct" );
			exit( MEM_ERROR );
		}

		cmt_ptr = cmt_ptr->next;
	}

	cmt_ptr->adrs = adrs;
	ctext = malloc( strlen( str ) + 3 );

	if ( ctext == NULL ) {
		printf( "\nNo memory for comment string" );
		exit( MEM_ERROR );
	}

	cmt_ptr->str = ctext;
	strcpy( ctext, "; " );
	strcat( ctext, str );
	cmt_ptr->next = NULL;
}

//
// Add patch string to linked list in memory
//

void add_patch( int adrs, char *str )
{
	int len;
	char * ctext;
	COMMENT_PTR	patch_ptr;

	len = strlen( str ) - 1;

	if ( len >= 0 && ( str[len] == '\n' || str[len] == '\r' ) )
		str[len] = '\0';					// get rid of newline

	if ( !patch_list ) {						// first patch
		patch_list = malloc( sizeof( struct comment ) );

		if ( patch_list == NULL ) {
			printf( "\nNo memory for patch struct" );
			exit( MEM_ERROR );
		}

		patch_ptr = patch_list;
	} else {							// add patch to list
		patch_ptr = patch_list;

		while ( patch_ptr->next )				// find end of list
			patch_ptr = patch_ptr->next;

		patch_ptr->next = malloc( sizeof( struct comment ) );

		if ( patch_ptr->next == NULL ) {
			printf( "\nNo memory for patch struct" );
			exit( MEM_ERROR );
		}

		patch_ptr = patch_ptr->next;
	}

	patch_ptr->adrs = adrs;
	ctext = malloc( strlen( str ) + 3 );

	if ( ctext == NULL ) {
		printf( "\nNo memory for patch string" );
		exit( MEM_ERROR );
	}

	patch_ptr->str = ctext;

	if ( len >= 0 )
		strcpy( ctext, str );
	else
		strcpy( ctext, ";" );

	patch_ptr->next = NULL;
}

//
// Output hexadecimal operand
//

void puthex( int j )
{
	j &= WORD_MASK;

	if ( altflag ) {
		if ( j < 0x100 )
			if ( upperflag )
				kcnt += fprintf( fp, "$%02X", j );
			else
				kcnt += fprintf( fp, "$%02X", j );
		else
			if ( upperflag )
				kcnt += fprintf( fp, "$%04X", j );
			else
				kcnt += fprintf( fp, "$%04X", j );

		return;
	}

	if ( baseflag ) {
		if ( upperflag )
			kcnt += fprintf( fp, "0x%X", j );
		else
			kcnt += fprintf( fp, "0x%x", j );

		return;
	}

	if ( j < 10 ) {
		if ( upperflag )
			kcnt += fprintf( fp, "%X", j );
		else
			kcnt += fprintf( fp, "%x", j );
	} else if ( j < 16 ) {
		if ( upperflag )
			kcnt += fprintf( fp, "0%XH", j );
		else
			kcnt += fprintf( fp, "0%xh", j );
	} else if ( j < 0xa0 ) {
		if ( upperflag )
			kcnt += fprintf( fp, "%XH", j );
		else
			kcnt += fprintf( fp, "%xh", j );
	} else if ( j < 0x100 ) {
		if ( upperflag )
			kcnt += fprintf( fp, "0%XH", j );
		else
			kcnt += fprintf( fp, "0%xh", j );
	} else if ( j < 0xa00 ) {
		if ( upperflag )
			kcnt += fprintf( fp, "%XH", j );
		else
			kcnt += fprintf( fp, "%xh", j );
	} else if ( j < 0x1000 ) {
		if ( upperflag )
			kcnt += fprintf( fp, "0%XH", j );
		else
			kcnt += fprintf( fp, "0%xh", j );
	} else if ( j < 0xa000 ) {
		if ( upperflag )
			kcnt += fprintf( fp, "%XH", j );
		else
			kcnt += fprintf( fp, "%xh", j );
	} else {
		if ( upperflag )
			kcnt += fprintf( fp, "0%XH", j );
		else
			kcnt += fprintf( fp, "0%xh", j );
	}
}


//
// Convert code to printable ascii
//

int ascii( int i )
{
	i = i & 0x7f;

	if ( i == 0x7f )
		return ( '.' );
	else if ( i < 0x20 )
		return ( '.' );
	else
		return ( i );
}

//
// Check if data is printable ascii other than the string delimiter
//

int is_ascii( byte data )
{
	if ( data < ' ' || data > 0x7e || data == '\'' )
		return( 0 );
	return( 1 );
}

//
// Convert ascii hex to hexadecimal for X option
//

int atox( char *str )
{
	char	c;
	int	i;

	i = 0;
	c = ( char ) toupper( *str++ );

	while ( c ) {
		if ( isxdigit( ( int ) c ) ) {
			c = ( c > '9' ) ? c - 0x37 : c & 0xf;
			i = ( i << 4 ) | c;
		} else
			break;

		c = ( char ) toupper( *str++ );
	}
	return( i );
}

//
// Check for reference to address in middle of
// code and flag split reference if true
//

void splitcheck( int i )
{
	if ( pgmflags[i] & PF_REF )			// ignore if not referenced
		pgmflags[i] |= PF_SPLIT;		// else flag split ref
}

//
// Add label to output line if current location marked as referenced
//

void chk_ref( int i )
{
	int	cnt;
	char	*cptr;

	if ( ( pgmflags[i] & ( PF_REF | PF_NOLABEL ) ) == PF_REF ) {
		pgmflags[i] |= PF_LABGEN;
		cptr = find_entry( i, label_count, lab_val_index );	// see if label exists

		if ( cptr == NULL ) {					// if not, output hex value
			if ( upperflag )
				cnt = fprintf( fp, "\nX%04X:", i );
			else
				cnt = fprintf( fp, "\nX%04x:", i );
		} else
			cnt = fprintf( fp, "\n%s:", cptr );		// else output label text

		if ( cnt > 8 )
			fprintf( fp, "\n\t" );
		else
			fprintf( fp, "\t" );
	} else
		fprintf( fp, "\n\t" );
}

//
// Add label to output line if current location marked as referenced
// For PF_WORD and PF_ADRS data
//

void chk_label( int i )
{
	char	*cptr;

	if ( ( pgmflags[i] & ( PF_REF | PF_NOLABEL ) ) == PF_REF ) {
		pgmflags[i] |= PF_LABGEN;
		cptr = find_entry( i, label_count, lab_val_index );	// see if label exists

		if ( cptr == NULL ) {					// if not, output hex value
			if ( upperflag )
				fprintf( fp, "\nX%04X:", i );
			else
				fprintf( fp, "\nX%04x:", i );
		} else
			fprintf( fp, "\n%s:", cptr );			// else output label text

		fprintf( fp, "\t" );
	} else
		fprintf( fp, "\n\t" );
}

//
// Output opcode for current code
//

void doopcode( char *mnem )
{
	char	c;

	c = *mnem++;

	if ( upperflag == FALSE )
		c = tolower( c );

	while ( c ) {					// output text from opcode table
		if ( c == ' ' ) {			// convert spaces to tabs
			putc( '\t', fp );
			kcnt = ( kcnt + 8 ) & 0x78;
		} else {
			putc( c, fp );
			kcnt++;
		}

		c = *mnem++;

		if ( upperflag == FALSE )
			c = tolower( c );
	}
}

//
// Output ascii data accumulated in buffer
//

void dump_ascii( int adrs )
{
	int	padrs, off, cnt;
	char	*cptr;

	padrs = adrs - asc_cnt;			// print address for comment field
	adrs = padrs;				// address in program array
	cnt = off = 0;				// cnt = char count, off = buffer offset

	while ( asc_cnt ) {			// while data in ascii buffer...
		if ( !( pgmflags[adrs] & PF_NOLABEL ) && ( pgmflags[adrs] & PF_REF ) ) {	// if addresss is referenced...
			if ( cnt ) {
				putc( '\'', fp );			// terminate line
				kcnt++;

				if ( hexflag ) {			// if comment field requested...
					do {				// show hex address
						putc( '\t', fp );	// but tab out to field first
						kcnt = ( kcnt + 8 ) & 0x78;
					} while ( kcnt < XSTOP );

					fprintf( fp, "; %04X", padrs );
				}

				padrs += cnt;				// update print address
				cnt = 0;				// clear char count for this line
			}

			pgmflags[adrs] |= PF_LABGEN;
			cptr = find_entry( adrs, label_count, lab_val_index );
			// see if label exists for this adrs
			if ( cptr == NULL )				// if not, show address in hex
				fprintf( fp, "\nX%04X:\t%s\t'", adrs, ascistr );
			else {						// else show label name
				if ( strlen( cptr ) > 8 )
					fprintf( fp, "\n%s:\n\t%s\t'", cptr, ascistr );
				else
					fprintf( fp, "\n%s:\t%s\t'", cptr, ascistr );
			}

			kcnt = 17;
		} else if ( !cnt ) {
			fprintf( fp, "\n\t%s\t'", ascistr );
			kcnt = 17;
		}

		putc( string[off], fp );	// output data in ascii
		kcnt++;				// character position in this line
		cnt++;				// increment char in this line
		off++;				// increment offset into asci buffer
		adrs++;				// offset into program memory

		if ( cnt >= ASCLINE ) {		// if max characters per line...
			putc( '\'', fp );	// terminate line
			kcnt++;

			if ( hexflag ) {	// if comment field requested
				do {		// show hex address
					putc( '\t', fp );
					kcnt = ( kcnt + 8 ) & 0x78;
				} while ( kcnt < XSTOP );

				fprintf( fp, "; %04X", padrs );
			}

			padrs += cnt;		// update print address
			cnt = 0;
		}

		--asc_cnt;
	}

	putc( '\'', fp );			// terminate line
	kcnt++;

	if ( hexflag && cnt ) {			// if comment field requested...
		do {				// show address
			putc( '\t', fp );
			kcnt = ( kcnt + 8 ) & 0x78;
		} while ( kcnt < XSTOP );

		fprintf( fp, "; %04X", padrs );
	}

	dump = 1;
}

//
// Output binary data accumulated in buffer
//

void dump_bytes( int adrs )
{
	int	padrs, bcnt, off, k;
	char	*cptr, chr;

	padrs = adrs - byte_cnt;			// compute adrs to print in ascii part
	adrs = padrs;
	bcnt = off = 0;					// no bytes output yet

	while ( byte_cnt ) {				// while data in binary buffer...
		if ( !( pgmflags[adrs] & PF_NOLABEL ) && ( pgmflags[adrs] & PF_REF ) ) {	// if addresss is referenced...
			if ( off && hexflag ) {		// dump any remaining ascii first
				do {
					putc( '\t', fp );
					kcnt = ( kcnt + 8 ) & 0x78;
				} while ( kcnt < XSTOP );

				fprintf( fp, "; %04X ", padrs );

				for ( k = 0; k < off; k++ )
					putc( ascii( pgmmem[padrs + k] ), fp );

				padrs += k;		// update print address
				off = 0;
			}

//			if (pgmflags[adrs] & PF_REF)
//			{
			pgmflags[adrs] |= PF_LABGEN;
			cptr = find_entry( adrs, label_count, lab_val_index );	// then do a label

			if ( cptr == NULL ) {
				if ( upperflag )
					fprintf( fp, "\nX%04X:\t%s\t", adrs, defbstr );
				else
					fprintf( fp, "\nX%04x:\t%s\t", adrs, defbstr );
			} else {
				if ( strlen( cptr ) > 8 )
					fprintf( fp, "\n%s:\n\t%s\t", cptr, defbstr );
				else
					fprintf( fp, "\n%s:\t%s\t", cptr, defbstr );
			}
//			}

			kcnt = 16;
			bcnt = 0;
		} else if ( !bcnt ) {			// else if first byte...
			kcnt = 16;
			fprintf( fp, "\n\t%s\t", defbstr );
		} else {
			putc( ',', fp );		// else separate bytes
			kcnt++;
		}

		if ( pgmflags[adrs] & PF_NAME )
			cptr = find_entry( adrs, name_count, name_val_index );
		else if ( ( pgmflags[adrs] & ( PF_LABEL | PF_SYMBOL ) ) == ( PF_LABEL | PF_SYMBOL ) )
			cptr = NULL;
		else
			cptr = find_entry( pgmmem[adrs], symbol_count, sym_val_index );

		if ( cptr )
			kcnt += fprintf( fp, "%s", cptr );
		else {
			if ( !( pgmflags[adrs] & PF_ASCII ) )
				puthex( pgmmem[adrs] & 0xff );
			else					// user defined this as ascii text
			{					// even though it's not; let's try
				chr = pgmmem[adrs];		// to give him what he wants.

				if ( chr & 0x80 )		// if flagged binary byte because
				{				// high bit is set...
					chr &= 0x7f;

					if ( chr >= ' ' && chr <= 'z' )		// would it be ascii
					{					// without bit 7 set?
						kcnt += fprintf( fp, "'%c'+80h", chr );	// yes
//						bcnt += 3;
					} else					// else do as binary and remove
					{					// ascii flag
						puthex( pgmmem[adrs] & 0xff );
						pgmflags[adrs] &= ~PF_ASCII;
					}
				} else						// high bit not set, so is
				{						// really binary, not ascii
					puthex( pgmmem[adrs] & 0xff );
					pgmflags[adrs] &= ~PF_ASCII;
				}
			}
		}

		bcnt++;
		k = ( kcnt + 8 ) & 0x78;		// where we will be if we tab
		k = 80 - k;				// how many spaces left
		k -= ( bcnt + 11 );			// minus what it takes for ascii output

		if ( pgmflags[adrs + 1] & PF_NAME )	// see if the next byte has a name entry
			cptr = find_entry( adrs + 1, name_count, name_val_index );
		else if ( ( pgmflags[adrs + 1] & ( PF_LABEL | PF_SYMBOL ) ) == ( PF_LABEL | PF_SYMBOL ) )
			cptr = NULL;
		else
			cptr = find_entry( pgmmem[adrs + 1], symbol_count, sym_val_index );

		if ( cptr )				// if next byte has a name entry...
			k -= strlen( cptr );		// subtract its length from available space

		if ( bcnt >= BYTELINE || pgmflags[adrs] & PF_ICMT || k < 1 ) {	// if max line length...
			bcnt = 0;

			if ( hexflag ) {		// do ascii dump of previous bytes
				do {
					putc( '\t', fp );
					kcnt = ( kcnt + 8 ) & 0x78;
				} while ( kcnt < XSTOP );

				fprintf( fp, "; %04X ", padrs );

				for ( k = 0; k <= off; k++ )
					putc( ascii( pgmmem[padrs + k] ), fp );

				padrs += k;
				off = 0;
			}
		} else
			off++;

		if ( pgmflags[adrs] & PF_ICMT )
			output_icomment( adrs );

		--byte_cnt;
		adrs++;
	}

	if ( off && hexflag ) {				// generate comment line
		do {
			putc( '\t', fp );
			kcnt = ( kcnt + 8 ) & 0x78;
		} while ( kcnt < XSTOP );

		fprintf( fp, "; %04X ", padrs );	// show address and ascii for data

		for ( k = 0; k < off; k++ )
			putc( ascii( pgmmem[padrs + k] ), fp );
	}

	dump = 1;
}

// ---- cycle counting routines
// attempts to read in the file pointed by *c, as called from commandline parser
void readcyclefile( char *filename )
{
	char	*text;
	char	c;
	int	i, opcode, b, pos;

	fp = fopen( filename, "r" );

	if ( fp == NULL ) {
		printf( "\n* Can't open cycle count file %s  *\n\n", filename );
		exit( FILE_ERROR );
	} else
		printf( "Reading cycle count file %s... \n", filename );

	for ( opcode = 0; opcode < 256; opcode++ ) {
		cycles[opcode] = 0;			// first clear the default counts
		cycles2[opcode] = 0;
	}

	while ( !feof( fp ) ) {				// until end of file...
		*linebuffer = '\0';			// clear previous line
		text = fgets( linebuffer, MAX_LINE - 1, fp );	// read one line
		i = 0;

		if ( !text )
			break;

		text = &linebuffer[0];
		pos = 0;

		do {
			while ( 1 ) {
				c = *text++;

				if ( c != ' ' && c != '\t' )	// skip whitespace
					break;
			}

			c = toupper( c );

			switch ( c ) {
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				c = c - 'A' + '9' + 1;
				// note that we fall over to the following (no break here!)

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				b = c - '0';
				c = toupper( *text );

				if ( ( c >= 'A' ) && ( c <= 'F' ) )
					c = c - 'A' + '9' + 1;

				if ( ( c >= '0' ) && ( c <= ( '9' + 6 ) ) ) {
					b = b * 16 + ( c - '0' );
					text++;
				}

				switch ( pos ) {
				case 0:
					opcode = b;
					pos++;
					break;

				case 2:
					cycles[opcode] = b;
					cycles2[opcode] = b;
					pos++;
					break;

				case 4:
					cycles2[opcode] = b;
					pos++;
					break;

				default:
					c = 'Q';		// we deliberately replace c with an invalid
					// character to cause error, when hexadecimal number
					// encountered in an unexpected field
				}

				if ( c != 'Q' ) break;		// for the error case fall over, so that
				// it falls over to the error handling (bruhaha)

			case '-':				// opcode and first cycle count separator
				if ( pos == 1 ) {
					pos++;
					break;
				} else
					c = 'Q';		// explanation for the invalid character see above

			case '/':				// first and second cycle count separator
				if ( pos == 3 ) {
					pos++;
					break;
				} else
					c = 'Q';		// explanation for the invalid character see above

			case '#':
			case ';':
			case '\n':
			case '\r':
			case '\0':				// remark or end of line -> exit for the next line
				// exit only if blank line or one or both values were given
				if ( ( pos == 0 ) || ( pos == 3 ) || ( pos == 5 ) ) {
					if ( c == '#' )
						printf( "%s", text );	// remark to be written to output file

					pos = -1;
					break;
				} else
					c = 'Q';		// explanation for the invalid character see above

			default:
				printf( "\n* Can't reasonably interpret the following line: *\n%s", linebuffer );
				exit( FILE_ERROR );
			}
		} while ( pos >= 0 );
	}
}

// this is just a "nested" function for the following cycle counting function
// - returns the current cycle_range for address num
CYCLE_RANGE_PTR cycle_in_in( int num )
{
	CYCLE_RANGE_PTR	cr, cx;
	bool	found;

	cx = NULL;
	cr = cycle_r;

	do {
		found = FALSE;

		while ( ( !found ) && ( cr != NULL ) ) {
			if ( ( num >= cr->min ) && ( num <= cr->max ) ) {
				cx = cr;
				found = TRUE;
			} else
				cr = cr->next;
		}

		if ( found )
			cr = cx -> child;

	} while ( found );

	return( cx );
}

// checks if cycle counting is on for line number num
//  - at the same time adds the cycle count cnt to the appropriate "counter"
//  - prints the needed text into comment
//  - sets also the cycle_current pointer to the currently counted cycle range
// (to be used in cycle_end)
//
void cycle_in( int num, int next_num, int cnt, int cnt2 )
{
	CYCLE_RANGE_PTR	cx, cy;

	cx = cycle_in_in( num );
	cycle_current = cx;

	if ( cx == NULL )
		fprintf( fp, "     " );
	else {
		if ( cx->mul == 0 )
			cycle_exclude = TRUE;		// setting this flag enables to write "-"
		// next to cycle counts in the excluded range, even for the
		// children (which might have nonzero multiplier to know the
		// excluded code length)

		if ( cx->cnt2 == 2 )
			cycle_alwaystake = TRUE;	// "worst case" analysis (jumps always taken
		// etc) for this range including subranges

		if ( cx->val != 0 ) {
			cnt = cx->val;
			fprintf( fp, ";=%2d", cnt );	// this is if the range has explicit
			// one-off value, as specified by "=" in the control file
		} else if	(
		        ( cx->min == cx->max ) &&	// for autorepeated in Z80 - have to
		        //  specify cycle count explicitly for this single line!
		        ( cnt2 != cnt ) && 		// and the two tables have to contain
		        // different count values (and, implicitly in this
		        // branch of if, the explicit val == 0)
		        ( cx -> mul > 1 )
		) {
			fprintf( fp, ";%3d = %d * %dx + %d",
			         cnt2 * ( cx->mul - 1 ) + cnt, cnt2, cx->mul - 1, cnt );
			cnt = cnt2 * ( cx->mul - 1 ) + cnt;
			cx->cnt2 = 1;			// indicate autorepeated instruction, so that summary
			// printing in cycle-end is suppressed
		} else {
			cy = cycle_in_in( next_num );

			if ( ( ( cy == NULL ) ||	// if there is no counting specified
			                // for the following instruction
			                ( ( cy->min == next_num ) && ( cy->mul == 0 ) ) ||
			                // or the cycle counting status just changed to inactive
			                ( cycle_alwaystake )	// or this range explicitly requires
			                // "worst case" (= "jump always taken") analysis
			     ) &&
			                ( cnt != cnt2 )		// and if this _is_ an instruction
			                // with alternative length
			   ) {				// then this is the case for "taken jump" and
				// the value from second table should be taken
				cnt = cnt2;
				fprintf( fp, ";^%2d", cnt );		// indicate taken jump
			} else
				fprintf( fp, "; %2d", cnt );		// normal instruction
		}

		cx->cnt += cnt;

		if ( cycle_exclude )
			fprintf( fp, "-" );		// indicate excluded range
		else
			fprintf( fp, " " );
	}

	return;
}

// cycle counting - check end range for address num and emit code appropriately
void cycle_end( int num )
{
	CYCLE_RANGE_PTR	cr, cx;
	int	cycles;

	cr = cycle_current;

	while ( cr != NULL ) {
		if ( num > cr->max ) {
			if ( cr->val == 0 ) {				// if nonzero explicit value of cycle count,
				// don't write summary
				if (
				        ( cr->min == cr->max ) &&		// for autorepeated in Z80 - have to
				        // specify cycle count explicitly for this single line!
				        ( cr->cnt2 == 1 ) && 		// and the two tables have to contain
				        // different count values
				        ( cr->val == 0 ) && 		// and there is no explicitly entered
				        // value for this range in the control file via "="
				        ( cr->mul > 1 ) ) {
					cycles = cr->cnt;		// autorepeated instruction - this was
					// already printed in-line, so print nothing here...
				} else {
					cycles = cr->cnt;

					if ( !newline )
						fprintf( fp, "\n;" );

					fprintf( fp, "\n; --- Cycle count %04X-%04X = %d",
					         cr->min, cr->max, cycles );

					if ( cr->mul > 1 ) {
						cycles *= cr->mul;
						fprintf( fp, " * %dx = %d", cr->mul, cycles );
					}

					if ( cr->mul == 0 ) {		// as a consequence of "-" suffix
						fprintf( fp, " - not included to the parent cycle count." );
						cycles = 0;
					}

					fprintf( fp, "\n;" );
					newline = TRUE;
				}
			} else
				cycles = cr->val;			// this is the explicitly set cycles count

			if ( cr->mul == 0 )				// reset the "cycle_exclude" flag when
				cycle_exclude = FALSE;			// leaving an excluded range

			if ( cr->cnt2 == 2 )				// reset the "worst case" analysis flag
				cycle_alwaystake = FALSE;		// when leaving a range with this flag

			cx = cr->parent;

			if ( cx != NULL )
				cx->cnt += cycles;			//cr->cnt * cr->mul;

			cr->cnt = 0;
			cr = cx;
		} else
			cr = NULL;
	}
}

//
// Read control file, if it exists, and flag areas as code, text,
// or whatever. Also handle labels, symbols, etc.
//
// Some strange things happen here with oring and anding on the
// flag bits; this is so the user can define something as code,
// data, or whatever, and assign a label for the same location.
// We should handle the result intelligently regardless of the
// order in which such things are found in the control file.
//

void pass0( void )
{
	int i;
	char * text, func, c, * ltext;
	int start, stop, code, temp;
	FILE * fpc;
	SYM_PTR sym;
	CYCLE_RANGE_PTR cr, cx, cy, cn;
	int found;

	if ( upperflag ) {
		makeupper( defbstr );
		makeupper( defwstr );
		makeupper( ascistr );
		makeupper( orgstr );
		makeupper( equstr );
	}

	fpc = fopen( ctl, "r" );

	if ( !fpc )
		return;

	if ( fpc != NULL ) {						// if control file exists...
		printf( "\nReading control file   " );

		while ( !feof( fpc ) ) {				// until end of file...
			start = stop = 0;
			*linebuffer = '\0';				// clear previous line
			text = fgets( linebuffer, MAX_LINE - 1, fpc );	// read one line
			i = 0;

			if ( !text )
				break;

			while ( linebuffer[i] && linebuffer[i] != '\n' )
				i++;

			linebuffer[i] = '\0';
			text = &linebuffer[1];

			while ( isgraph( *text ) )			// skip remaining chars in first word
				text++;

			text = get_adrs( text, &start );

			while ( 1 ) {
				c = *text++;
				if ( c != ' ' && c != '\t' )		// skip whitespace
					break;
			}

			if ( c == '\n' || c == ';' )			// if only one numeric...
				--text;					// back up to newline

			func = c;					// save operator
			ltext = text;
			--ltext;
			text = get_adrs( text, &stop );

			if ( func == '+' )				// check for valid operator
				stop += ( start - 1 );
			else if ( func == '-' && !stop )
				stop = start;

			switch ( toupper( linebuffer[0] ) ) {
			case 'A':					// address
				if ( start < PMEMSIZE && stop < PMEMSIZE ) {
					do {				// get address to reference
						code = pgmmem[start] & 0xff;
						temp = ( pgmmem[start + 1] & 0xff ) << 8;
						code |= temp;

						if ( code < PMEMSIZE ) {
							pgmflags[code] |= PF_REF;		// flag referenced address
							pgmflags[code] &= ~PF_SPLIT;
							pgmflags[start] |= PF_ADRS;		// set flags to adrs code
							pgmflags[start++] &=
							        ~( PF_NOINIT | PF_WORD | PF_BYTE | PF_ASCII | PF_SPLIT );
							pgmflags[start] |= PF_ADRS;
							pgmflags[start++] &=
							        ~( PF_NOINIT | PF_WORD | PF_BYTE | PF_ASCII | PF_SPLIT );
						} else {
							pgmflags[start++] |= PF_WORD;
							pgmflags[start++] |= PF_WORD;
						}
					} while ( start < stop );
				} else
					printf( "\rinvalid address specified: %x, %x\n",
					        start, stop );
				break;

			case 'B':					// byte binary
				if ( start < PMEMSIZE && stop < PMEMSIZE ) {
					do {
						pgmflags[start] |= PF_BYTE;
						pgmflags[start++] &=
						        ~( PF_NOINIT | PF_ADRS | PF_WORD | PF_ASCII | PF_SPLIT );
					} while ( start <= stop );
				} else
					printf( "\rinvalid address specified: %X, %X\n",
					        start, stop );
				break;

			case 'C':					// code
				if ( start < PMEMSIZE && stop < PMEMSIZE ) {
					do {
						pgmflags[start] &=
						        ~( PF_NOINIT | PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII | PF_SPLIT );
						pgmflags[start++] |= PF_FORCE;
					} while ( start <= stop );
				} else
					printf( "\rinvalid address specified: %X, %X\n",
					        start, stop );
				break;

			case 'D':							// define data type
				switch ( toupper( *ltext ) ) {
				case '0':
				case 'L':
					pgmflags[start] |= PF_LABEL;			// 0 or (l)abel = search label table only
					break;

				case '1':
				case 'S':
					pgmflags[start] |= PF_SYMBOL;			// 1 or (s)ymbol = search symbol table only
					break;

				case '2':
				case 'N':
					pgmflags[start] |= ( PF_LABEL | PF_SYMBOL );	// 2 or (n)one = don't search either table
					break;

				default:
					printf( "\nInvalid data type flag: %s\n", linebuffer );
					break;
				}

				break;
#ifdef	EXTENDED_MEM
			case 'E':					// extended memory specification
									// incomplete
				break;
#endif
			case 'I':					// ignore
				if ( start < PMEMSIZE && stop < PMEMSIZE ) {
					do {
						pgmflags[start++] = PF_NOINIT;
					} while ( start <= stop );
				} else
					printf( "\rinvalid address specified: %X, %X\n",
					        start, stop );
				break;

			case 'L':					// label
				if ( start < PMEMSIZE ) {
					pgmflags[start] |= PF_REF;	// flag reference
					pgmflags[start] &= ~PF_SPLIT;

					if ( isgraph( *ltext ) )
						add_entry( start, ltext, LABEL_TYPE );
				} else
					printf( "\rinvalid address specified: %X, %X\n",
					        start, stop );
				break;
			case 'N':
				pgmflags[start] |= PF_NOLABEL;
				break;

			case 'O':					// hex offset for program
				if ( !offset )
					offset = start;
				break;

			case 'P':					// patch code (add inline code)
				pgmflags[start] |= PF_PATCH;		// flag address
				text = get_adrs( ( char * ) & linebuffer[1], &start );
				add_patch( start, text );
				break;
			case 'S':					// symbol
				add_entry( start, ltext, SYMBOL_TYPE );
				break;

			case 'T':					// ascii text
				do {
					pgmflags[start] |= PF_ASCII;
					pgmflags[start++] &=
					        ~( PF_NOINIT | PF_ADRS | PF_WORD | PF_BYTE | PF_SPLIT );
				} while ( start <= stop );
				break;
#ifdef	EXTENDED_MEM
			case 'U':					// use extended memory translation
				// incomplete
				break;
#endif
			case 'W':					// word binary
				if ( start < PMEMSIZE && stop < PMEMSIZE ) {
					do {
						pgmflags[start] |= PF_WORD;
						pgmflags[start++] &=
						        ~( PF_NOINIT | PF_ADRS | PF_BYTE | PF_ASCII | PF_SPLIT );
					} while ( start <= stop );
				} else
					printf( "\rinvalid address specified: %X, %X\n",
					        start, stop );
				break;

			case 'X':					// operand name
				pgmflags[start] |= PF_NAME;
				sym = add_entry( start, ltext, NAME_TYPE );
				sym->used = FALSE;
				break;

			case 'Y':					// operand name, no equ generation
				pgmflags[start] |= PF_NAME;
				sym = add_entry( start, ltext, NAME_TYPE );
				sym->used = -1;
				break;

			case 'Z':					// cycle counting
				if ( ( cn = ( CYCLE_RANGE_PTR ) malloc( sizeof( struct cycle_range ) ) ) == NULL )
					exit( 1 );			// error handling !!!

				cn->cnt = 0;				// fill up with default
				cn->cnt2 = 0;
				cn->min = start;
				cn->max = stop;
				cn->mul = 1;
				cn->val = 0;				// the explicit one-off value, see '=' below
				cn->child = NULL;
				cn->parent = NULL;
				cn->next = NULL;
				// patch the single-num-range--and-third-parameter cases
				// (we are unable to parse the xxxx- case, that would need to
				// rewrite a portion of parsing above which we don't want for
				// backwards compatibility reasons - let's document it, making
				// a "feature" from the bug... :-)

				if ( func == '*' ) {
					cn->mul = stop;
					cn->max = stop = start;
				} else if ( func == '=' ) {
					cn->val = stop;
					cn->max = stop = start;
				}
				// get the third numeral - I've simply stolen Jeff's code from above... -- JW
				while ( 1 ) {
					c = *text++;

					if ( c != ' ' && c != '\t' )	// skip whitespace
						break;
				}

				if ( c == '\n' || c == ';' )		// if no more arguments
					--text;				// back up to newline

				if ( c == '*' )
					text = get_adrs( text, &cn->mul );
				else if ( c == '=' )
					text = get_adrs( text, &cn->val );
				else if ( c == '^' )			// indicate maximum overdrive,
					cn->cnt2 = 2;			// sorry, "worst case" ("jump always taken")
				else if ( c == '-' )
					cn->mul = 0;			// zero multiplier to indicate "exclude from
									// overall count" - for the "not taken" branch

				if ( stop > PMEMSIZE )
					stop = PMEMSIZE;		// sanity

				if ( stop < start )			// sanity
					exit( 1 ); //!!! error handling !!!

				cx = NULL;				// now let's find, where could this range fit
				cy = NULL;
				cr = cycle_r;

				do {
					found = 0;

					while ( ( !found ) && ( cr != NULL ) ) {
						if ( ( start >= cr->min ) && ( stop <= cr->max ) ) {
							// our new range would fit into this one
							cx = cr;
							found = 1;
						} else if ( ( start <= cr->min ) && ( stop >= cr->max ) ) {
							//our new range eats this one
							if ( cy == NULL )		// init conditon - only one range is there yet
								cycle_r = cn;
							else if ( cy->child == cr ) {
								cy->child = cn;
								cn->parent = cx;
							} else {
								cy->next = cn;		// first swap in the new for the old
								cn->parent = cy->parent;
							}

							cn->child = cr;			// the old is now child of the new
							cr->parent = cn;

							while ( ( cx = cr->next ) != NULL ) {
								// determine, siblings of old will be siblings or children of new
								if ( ( start <= cx->min ) && ( stop >= cx->max ) ) {
									// remains a child - but child of cn!
									cx->parent = cr->parent;
								} else if ( ( ( cx->min < start ) &&
									    ( cx->max > start ) &&
									    ( cx->max < stop ) ) ||
									   ( ( cx->min > start ) &&
									     ( cx->min < stop ) &&
									     ( cx->max > stop ) ) ) {
  									// pathologic partial overlap
									exit( 1 ); //!!! error handling !!!
								} else	// goes outside cn range so it's sibling - the
									// 4th option is impossible from context
								{
									cr->next = cx->next;	// detach from old chain
									cx->next = cn->next;
									cx->parent = cn->parent;
									cn->next = cx;		// insert into new chain
								}

								cr = cx;			// and go for the next
							}

							found = 2;	// when finished, get out of the whole new
									// range handling the shortest way
						} else if ( ( ( cr->min < start ) && ( cr->max > start ) &&
						                ( cr->max < stop ) ) ||
								( ( cr->min > start ) &&
						                  ( cr->min < stop ) && ( cr->max > stop ) ) ) {					// pathologic partial overlap
							exit( 1 );	// !!! error handling !!!
						} else {
							cy = cr;
							cr = cy->next;	// none of those - ignore - but let cy
									// always point to the currently probed one
						}
					}

					if ( found == 1 ) {
						cy = cx;
						cr = cx -> child;
					}
				} while ( found == 1 );

				if ( found == 0 )			// this means the new range fits into an old ...
				{					// cx contains the would-be parent
					if ( cy == NULL )		// special case: first record of the day... :-)
						cycle_r = cn;
					else if ( cx == NULL )		// no matching record in the topmost level
						cy->next = cn;
					else {
						cn->next = cx->child;	// insert as a new child of cx
						cn->parent = cx;
						cx->child = cn;
					}
				}

				cycleflag = TRUE;			// oh yes, we almost forgot to flip on the
									// main cycle counting switch...
				break;

			case 0x00:					// ignore empty lines
			case '\n':
			case '\r':
			case '\t':
			case ' ':
			case ';':					// ignore commented out lines
				break;

			case '#':					// comment string
				pgmflags[start] |= PF_CMT;		// flag address
				text = get_adrs( ( char * ) & linebuffer[1], &start );
				add_comment( start, text );
				break;

			case '!':					// inline comment string
				pgmflags[start] |= PF_ICMT;
				text = get_adrs( ( char * ) & linebuffer[1], &start );
				add_icomment( start, text );
				break;

			default:					// somebody didn't read the docs...
				printf( "\nUnknown control code: 0x%02X in '%s'\n",
				        linebuffer[0], linebuffer );
				break;
			}
		}

		if ( label_count || symbol_count || name_count ) {	// set up tail node for sort
			tail_ptr = ( SYM_PTR ) malloc( sizeof( SYM ) );

			if ( tail_ptr == NULL ) {
				printf( "\nNo memory for symbol pointers!\n" );
				exit( MEM_ERROR );
			}

			tail_ptr->next = tail_ptr;
			tail_ptr->name = malloc( 4 );
			tail_ptr->name[0] = 0xfe;			// set max values for sort
			tail_ptr->name[1] = 0;
			tail_ptr->val = 0xfffff;			// set an invalid value to mark end of list
		}

		if ( label_count ) {					// if labels encountered...
			lab_tab_last->next = tail_ptr;			// set up pointer array for sort
			lab_val_index = malloc( sizeof( SYM_PTR ) * label_count );

			if ( lab_val_index == NULL ) {
				printf( "\nNo memory for label pointers" );
				exit( MEM_ERROR );
			}

			lab_tab = sort( lab_tab, lab_val_index, label_count );

			if ( !lab_tab )
				exit( USER_ERROR );
		}

		if ( symbol_count ) {					// if symbols encountered...
			sym_tab_last->next = tail_ptr;
			sym_val_index = malloc( sizeof( SYM_PTR ) * symbol_count );

			if ( sym_val_index == NULL ) {
				printf( "\nNo memory for symbol pointers" );
				exit( MEM_ERROR );
			}

			sym_tab = sort( sym_tab, sym_val_index, symbol_count );

			if ( !sym_tab )
				exit( USER_ERROR );
		}

		if ( name_count ) {					// if operand symbols encountered...
			name_tab_last->next = tail_ptr;
			name_val_index = malloc( sizeof( SYM_PTR ) * name_count );

			if ( name_val_index == NULL ) {
				printf( "\nNo memory for operand name pointers!\n" );
				exit( MEM_ERROR );
			}

			name_tab = sort( name_tab, name_val_index, name_count );

			if ( !name_tab )
				exit( USER_ERROR );
		}

		fclose( fpc );
	} else
		printf( "No control file found\n\n" );
}

/*
	MISC_EQU_TAG is used in pass 3 to determine whether a miscellaneous
	equate statement should be generated. The equate statement will be
	written to the disassembly file if the location is referenced by some
	instruction and either it is a split reference or the location is
	uninitialized and the location does not have an entry in the label
	table. If the above requirements are met except that the location has
	an entry in the label table, then an equate statement will be generated
	as a label equate.
*/

#define	MISC_EQU_TAG ((pflag & PF_REF) && ((pflag & PF_SPLIT) | (pflag & PF_NOINIT)))


//
// Global variables
//
/* none */

// Return TRUE if string is a simple expression based on a defined
// symbol, label, or operand name, eg: label+10 or symbol*4

bool isnumexpression( char *str )
{
	int	i, len;
	bool	arith = FALSE, valid = FALSE;
	char	name[MAX_LINE];

	len = strlen( str );

	for ( i = 0; i < len && !arith; i++ ) {
		switch ( str[i] ) {
		case '+':
		case '-':
		case '*':
		case '/':
			arith = TRUE;
			name[i] = '\0';
			break;

		default:
			name[i] = str[i];
			break;
		}
	}

	name[i] = '\0';

	if ( arith ) {	// appears to be an arithmetic expression
		if ( find_name( name, label_count, lab_val_index ) )			// is it a valid label?
			valid = TRUE;
		else if ( find_name( name, symbol_count, sym_val_index ) )	// is it a valid symbol?
			valid = TRUE;
		else if ( find_name( name, name_count, name_val_index ) )		// is it a valid operand name?
			valid = TRUE;
	}

	if ( arith && valid )	// arithmetic expression based on a defined label, symbol, or name
		return TRUE;

	return FALSE;
}

// Return TRUE if string is purely numeric (eg: "0800h" or "1024" or "10110b")

bool isnumeric( char *str )
{
	bool	result = TRUE;
	bool	hex = FALSE;
	bool	binary = FALSE;
	int	i, len;

	len = strlen( str );

	if ( toupper( str[len - 1] ) == 'H' ) {
		hex = TRUE;
		--len;
	} else if ( toupper( str[len - 1] ) == 'B' ) {
		binary = TRUE;
		--len;
	}

	i = 0;

	if ( !hex && !binary ) {		// if decimal number
		if ( str[0] == '-' && len > 1 )		// allow negative
			i = 1;
	}

	for ( ; i < len; i++ ) {
		if ( !str[i] || str[i] == '\n' )
			break;

		if ( hex ) {				// hexadecimal number
			if ( !isxdigit( str[i] ) ) {
				result = FALSE;
				break;
			}
		} else if ( binary ) {	// binary number
			if ( str[i] != '0' && str[i] != '1' ) {
				result = FALSE;
				break;
			}
		} else {					// decimal number
			if ( !isdigit( str[i] ) ) {
				result = isnumexpression( str );	// allow expressions in decimal data, eg: symbol+100
				break;
			}
		}
	}

	return result;
}

//
// Pass three of disassembly
//
// Search for references to un-initialized data or split references
// and, if found, generate EQU statements for them.
//

void pass3( void )
{
	int i, j, k, index, val, adrs, next_adrs, next_val, ok;
	SYM_PTR ptr = NULL;
	char * cptr;
	int pflag;
	bool isnum = FALSE;

	printf( "\nPass 3 0000" );

	// Sort tables by name for checking numeric entries

	if ( label_count ) {
		lab_tab = sort_by_name( lab_tab );
		ptr = lab_tab;

		for ( i = 0; i < label_count; i++ ) {
			lab_val_index[i] = ptr;
			ptr = ptr->next;
		}
	}

	if ( symbol_count ) {
		sym_tab = sort_by_name( sym_tab );
		ptr = sym_tab;

		for ( i = 0; i < symbol_count; i++ ) {
			sym_val_index[i] = ptr;
			ptr = ptr->next;
		}
	}

	if ( name_count ) {
		name_tab = sort_by_name( name_tab );
		ptr = name_tab;

		for ( i = 0; i < name_count; i++ ) {
			name_val_index[i] = ptr;
			ptr = ptr->next;
		}
	}

// search label table for labels referenced but not generated

	j = TRUE;

	for ( index = 0; index < label_count; index++ ) {
		ptr = lab_val_index[index];
		isnum = isnumeric( ptr->name );			// ignore if numeric expression

		if ( ptr->used && !isnum ) {
			val = ptr->val;
			val = pgmflags[val];
			val &= ( PF_NOINIT | PF_SPLIT | PF_REF );

			if ( val == ( PF_REF | PF_SPLIT ) || val == ( PF_REF | PF_NOINIT ) ) {
				if ( j ) {			// do header if first one
					j = FALSE;

					if ( !newline || dump )
						fprintf( fp, "\n;" );

					fprintf( fp, "\n;\tLabel equates\n;\n;"
					         "  These are labels in the control file that reference\n;"
					         "  the middle of a multibyte instruction or reference\n;"
					         "  an address outside the initialized space\n;" );
				}

				fprintf( fp, "\n%s\t%s\t", ptr->name, equstr );
				puthex( ptr->val );
				newline = FALSE;
			}
		}
	}

// now do equates for symbol table

	j = TRUE;

	for ( index = 0; index < symbol_count; index++ ) {
		ptr = sym_val_index[index];
		isnum = isnumeric( ptr->name );			// ignore if numeric expression

		if ( ptr->used && !isnum ) {
			if ( j ) {				// do header if first one
				j = FALSE;

				if ( !newline || dump )
					fprintf( fp, "\n;" );

				fprintf( fp, "\n;\tSymbol equates\n;\n;"
				         "  These are symbols from the control\n;"
				         "  file that are referenced in the code\n;" );
			}

			fprintf( fp, "\n%s\t%s\t", ptr->name, equstr );
			puthex( ptr->val );
			newline = FALSE;
		}
	}

// now do equates for operand name table

	j = TRUE;
	ptr = name_tab;

	for ( index = 0; index < name_count; index++ ) {
		ok = FALSE;
		isnum = isnumeric( ptr->name );			// ignore if numeric expression

		if ( ( ptr->used != 255 ) && !isnum ) {
			if ( !strcasecmp( ptr->name, ptr->next->name ) ) {
				adrs = ptr->val;
				val = pgmmem[adrs];

				if ( pgmflags[adrs] & PF_2BYTE )
					val |= ( pgmmem[adrs + 1] << 8 );

				next_adrs = ptr->next->val;
				next_val = pgmmem[next_adrs];

				if ( pgmflags[next_adrs] & PF_2BYTE )
					next_val |= ( pgmmem[next_adrs + 1] << 8 );

				if ( val != next_val )
					ok = TRUE;
			} else
				ok = TRUE;

			if ( ok ) {
				if ( j ) {			// do header if first one
					j = FALSE;

					if ( !newline || dump )
						fprintf( fp, "\n;" );

					fprintf( fp, "\n;\tOperand symbol equates\n;\n;"
					         "  These are operand symbols from the control\n;"
					         "  file that are referenced in the code\n;" );
				}

				adrs = ptr->val;
				val = pgmmem[adrs] & 0xff;

				if ( pgmflags[adrs] & PF_2BYTE )
					val |= ( pgmmem[adrs + 1] << 8 );

				fprintf( fp, "\n%s\t%s\t", ptr->name, equstr );
				puthex( val );
				newline = FALSE;
			}
		}

		ptr = ptr->next;
	}

	// to do miscellaneous equates, we need to resort labels by value

	if ( label_count )
		lab_tab = sort( lab_tab, lab_val_index, label_count );

	j = TRUE;

	for ( i = 0; ; ) {
		k = i & 0xfff;
		pflag = pgmflags[i];

// if location is referenced and un-initialized or is a split ref

		if ( MISC_EQU_TAG ) {
			cptr = find_entry( i, label_count, lab_val_index );

			if ( ( cptr == NULL ) && !( pflag & PF_LABGEN ) ) {	// if not in label list
				if ( j ) {					// do header if first one
					j = FALSE;

					if ( !newline || dump )
						fprintf( fp, "\n;" );

					fprintf( fp, "\n;\tMiscellaneous equates\n;\n;"
					         "  These are addresses referenced in the code but\n;"
					         "  which are in the middle of a multibyte instruction\n;"
					         "  or are addresses outside the initialized space\n;" );
					newline = FALSE;
				}
				// do EQU statement
				if ( !upperflag )
					fprintf( fp, "\nX%04x\t%s\t", i, equstr );
				else
					fprintf( fp, "\nX%04X\t%s\t", i, equstr );

				puthex( i );
			}
		}

		i++;

		if ( !( i & WORD_MASK ) )
			break;

		if ( ( i & 0xfff ) < k )
			printf( "\rPass 3 %04X", i );
	}

	printf( "\rPass 3 - Equate generation complete" );

	if ( !newline || dump )
		fprintf( fp, "\n;" );

	if ( upperflag )
		fprintf( fp, "\n\tEND\n;\n\n" );
	else
		fprintf( fp, "\n\tend\n;\n\n" );

	fflush( fp );
	fclose( fp );
}	//  End of Pass 3


// end of mdz80.c

