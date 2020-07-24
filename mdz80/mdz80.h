
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
 * dz80.h - Z80 disassembler definitions
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

#ifndef	_MDZ80_H_
#define	_MDZ80_H_

#include "defs.h"
#include <stdio.h>

//
//	Defined Constants
//

#ifdef EXTENDED_MEM
#define EXT_PGM_SEGS	8	/* maximum number of extended segments: 8 * 64kb = 512kb */
#endif

// opttbl bits:

#define	OPT_XFER	0x80	// unconditional transfer
#define	OPT_SPEC	0x40	// special codes (cd, dd, ed, fd)
#define	OPT_REL		0x20	// relative addressing
#define	OPT_DIR		0x10	// direct addressing (jp, call)
#define	OPT_PAR		0x08	// parenthesized address
#define	OPT_IMM		0x04	// immediate data
#define	OPT_3		0x02	// 3 byte instruction
#define	OPT_2		0x01	// 2 byte instruction
#define	OPT_NONE	0x00	// single byte, no options

#define	OPT_SIZE	(OPT_2 | OPT_3)
#define	OPT_IMM2	(OPT_IMM | OPT_2)
#define	OPT_PAR2	(OPT_PAR | OPT_2)
#define	OPT_DIR_IMM3	(OPT_DIR | OPT_IMM | OPT_3)
#define	OPT_PAR_IMM3	(OPT_PAR | OPT_DIR_IMM3)
#define	OPT_REL2	(OPT_REL | OPT_2)
#define	OPT_SPEC2	(OPT_SPEC | OPT_2)

// ED option bits:

#define	OPT_ED_2	0x01	// 2 byte, entry in edtbl
#define	OPT_ED_STORE	0x03	// write 16 bit register to memory
#define	OPT_ED_LD_BC	0x13	// load bc from memory
#define	OPT_ED_LD_DE	0x23	// load de from memory
#define	OPT_ED_LD_SP	0x33	// load sp from memory
#define	OPT_ED_RET	0x81	// retn or reti

// DD and FD option bits:

#define	OPT_DD_2	0x01	// 2 byte, entry in dd1tbl
#define	OPT_DD_LOAD	0x02	// 3 byte loads
#define	OPT_DD_DIR	0x03	// 4 byte direct addressing
#define	OPT_DD_ARTH	0x06	// 3 byte arithmetic codes
#define	OPT_DD_CB	0x07	// 4 byte dd/fd cb codes

struct snementry {
	char mcode[8];
};

struct entry {
	char mnem[16];
};

//
//	Global Variables
//

extern char	licenseText[];
extern int	d8080;				// 8080 mnemonic flag

extern char	src[FN_LEN], dst[FN_LEN];	// file name buffers
extern char	baseFileName[FN_LEN];		// source file name without extension
extern char	ctl[FN_LEN];			// control file name
extern char	linebuffer[MAX_LINE];		// input line buffer
extern FILE	* fp;				// input file struct
extern int	hexflag;			// append hex flag
extern int	fileflag;			// file type flag
extern int	upperflag;			// upper case output flag
extern int	baseflag;
extern int	traceflag;			// trace and analyze code
extern int	prsmcode;			// presume code flag
extern int	kcnt;				// output char counter
extern int	pc;				// current program counter
extern int	himark;				// highest data adrs
extern int	offset;				// program counter offset
extern byte	*pgmmem;			// program data pointer
extern int	*pgmflags;			// pointer to program flags

#ifdef	EXTENDED_MEM
extern byte	*epgmmem[EXT_PGM_SEGS];		// extended program memory pointers
extern int	*epgmflags[EXT_PGM_SEGS];	// extended program flags pointers
#endif

extern char	string[ASCLIMIT];		// ascii data for defb
extern int	asc_cnt;			// count for string data
extern byte	byte_data[BYTELIMIT];		// binary data for defb
extern int	byte_cnt;			// count for binary data
extern int	word_data[WORDLIMIT];		// binary data for defw
extern int	word_cnt;			// count for word data
extern byte	dump;				// dump just done flag
extern byte	ascii_flag;			// use ascii string flag
extern char	defbstr[8];			// string for defined bytes
extern char	defwstr[8];			// string for defined words
extern char	ascistr[8];			// string for defined ascii
extern char	orgstr[8];			// org pseudo-op string
extern char	equstr[8];			// equ pseudo-op string

extern SYM_PTR	sym_tab;			// symbol table pointer
extern SYM_PTR	lab_tab;			// label table pointer
extern SYM_PTR	name_tab;			// operand names pointer
extern SYM_PTR	sym_tab_last;			// last symbol table pointer
extern SYM_PTR	lab_tab_last;			// lastlabel table pointer
extern SYM_PTR	name_tab_last;			// last name table pointer
extern int	symbol_count;			// number of symbols
extern int	label_count;			// number of labels
extern int	name_count;			// number of operand names

extern SYM_PTR	* sym_val_index;		// array of pointers
extern SYM_PTR	* lab_val_index;		//  for binary search
extern SYM_PTR	* name_val_index;
extern SYM_PTR tail_ptr, head_ptr;		// sort pointers

extern COMMENT_PTR comment_list;		// header comment list
extern COMMENT_PTR icomment_list;		// inline comment list
extern COMMENT_PTR patch_list;			// patch list

extern int	newline;			// just output newline flag
extern struct tm * date_time;			// disassembly time

extern bool	cycleflag;			// cycle counting global on/off switch
extern bool	cycle_exclude;			// cycle counting suspended flag
extern bool 	cycle_alwaystake;		// cycle counting "always take" (worst case) flag
extern CYCLE_RANGE_PTR	cycle_r;		// cycle counting root range
extern CYCLE_RANGE_PTR	cycle_current;		// cycle counting current range

//
// Prototypes
//

extern bool	init_memory(void);

#ifdef	EXTENDED_MEM
extern byte	* get_extended_mem(int width);
#endif

extern char	* makeupper(char *str);
extern int	readfile(char *filename);
extern void	getCTLoffset(void);
extern void	getcode(char *from, byte *loc);
extern char	* get_adrs(char *text, int *val);
extern void	error(char *str1, char *str2);		// fatal error trap
extern SYM_PTR	sort(SYM_PTR list, SYM_PTR * array, int count);
extern SYM_PTR	sort_by_name(SYM_PTR list);
extern SYM_PTR	sort_by_value(SYM_PTR list);
extern SYM_PTR	merge_by_name(SYM_PTR a, SYM_PTR b);
extern SYM_PTR	merge_by_value(SYM_PTR a, SYM_PTR b);
extern void	chk_dup_name(SYM_PTR list, int count);
extern void	chk_dup_op_name(SYM_PTR list, int count);
extern void	chk_dup_value(SYM_PTR list, int count);
extern void	chk_dup_op_value(SYM_PTR list, int count);
extern char	* find_entry(int val, int count, SYM_PTR *table);
extern SYM_PTR	find_name(char *name, int count, SYM_PTR *table);
extern SYM_PTR	get_smem(int type, int req_size);
extern SYM_PTR	add_entry(int val, char *symbol, int type);
extern void	output_comment(int adrs);
extern void	output_icomment(int adrs);
extern void	output_patch(int adrs);
extern void	add_comment(int adrs, char *str);
extern void	add_icomment(int adrs, char *str);
extern void	add_patch(int adrs, char *str);
extern void	puthex(int j);
extern int	ascii(int i);
extern int	is_ascii(byte data);
extern int	atox(char *str);
extern void	splitcheck(int i);
extern void	chk_ref(int i);
extern void	chk_label(int i);
extern void	doopcode(char *mnem);
extern void	dump_ascii(int adrs);
extern void	dump_bytes(int adrs);
extern void	readcyclefile(char *c);
extern void	cycle_in(int num, int next_num, int cnt, int cnt2);
extern void	cycle_end(int num);
extern void	usage( void );
extern char	* strip_ext( char * fname );
extern FILE	* ext_open( char * fname, char * hex_e, char * bin_e );
extern bool	isnumeric( char *str );

// --- although I know this is not properly declared here,
// I know no better way how to make it usable in common.c ---
//     (haven't I told that I am not really into C? :-) )  -- JW
// so it is assumed that all 3 ports (d52, d48, dz80) do have
// these tables implemented (in XXXtables.c)
extern byte	cycles[256];
extern byte	cycles2[256];

// some prototype to keep gcc happy
void pass0( void );
void pass3( void );


#endif	// _MDZ80_H_
