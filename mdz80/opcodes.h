/***************************************************************************
 *   Copyright (C) 2008 by Piergiorgio Betti   *
 *   pbetti@lpconsul.net   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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


/*  DZ80.TBL - Copyright 1990 by J. L. Post
 *  01/20/96
 *  Mnemonic Table for Z80 Disassembler
 */

struct mnementry mnemtbl[] = {
    {"NOP"},        {"LD BC,"},     {"LD (BC),A"},  {"INC BC"}, /* 00 - 03 */
    {"INC B"},      {"DEC B"},      {"LD B,"},      {"RLCA"},   /* 04 - 07 */
    {"EX AF,AF'"},  {"ADD HL,BC"},  {"LD A,(BC)"},  {"DEC BC"}, /* 08 - 0B */
    {"INC C"},      {"DEC C"},      {"LD C,"},      {"RRCA"},   /* 0C - 0F */
    {"DJNZ "},      {"LD DE,"},     {"LD (DE),A"},  {"INC DE"}, /* 10 - 13 */
    {"INC D"},      {"DEC D"},      {"LD D,"},      {"RLA"},    /* 14 - 17 */
    {"JR "},        {"ADD HL,DE"},  {"LD A,(DE)"},  {"DEC DE"}, /* 18 - 1B */
    {"INC E"},      {"DEC E"},      {"LD E,"},      {"RRA"},    /* 1C - 1F */
    {"JR NZ,"},     {"LD HL,"},     {"LD "},        {"INC HL"}, /* 20 - 23 */
    {"INC H"},      {"DEC H"},      {"LD H,"},      {"DAA"},    /* 24 - 27 */
    {"JR Z,"},      {"ADD HL,HL"},  {"LD HL,"},     {"DEC HL"}, /* 28 - 2B */
    {"INC L"},      {"DEC L"},      {"LD L,"},      {"CPL"},    /* 2C - 2F */
    {"JR NC,"},     {"LD SP,"},     {"LD "},        {"INC SP"}, /* 30 - 33 */
    {"INC (HL)"},   {"DEC (HL)"},   {"LD (HL),"},   {"SCF"},    /* 34 - 37 */
    {"JR C,"},      {"ADD HL,SP"},  {"LD A,"},      {"DEC SP"}, /* 38 - 3B */
    {"INC A"},      {"DEC A"},      {"LD A,"},      {"CCF"},    /* 3C - 3F */
    {"LD B,B"},     {"LD B,C"},     {"LD B,D"},     {"LD B,E"}, /* 40 - 43 */
    {"LD B,H"},     {"LD B,L"},     {"LD B,(HL)"},  {"LD B,A"}, /* 44 - 47 */
    {"LD C,B"},     {"LD C,C"},     {"LD C,D"},     {"LD C,E"}, /* 48 - 4B */
    {"LD C,H"},     {"LD C,L"},     {"LD C,(HL)"},  {"LD C,A"}, /* 4C - 4F */
    {"LD D,B"},     {"LD D,C"},     {"LD D,D"},     {"LD D,E"}, /* 50 - 53 */
    {"LD D,H"},     {"LD D,L"},     {"LD D,(HL)"},  {"LD D,A"}, /* 54 - 57 */
    {"LD E,B"},     {"LD E,C"},     {"LD E,D"},     {"LD E,E"}, /* 58 - 5B */
    {"LD E,H"},     {"LD E,L"},     {"LD E,(HL)"},  {"LD E,A"}, /* 5C - 5F */
    {"LD H,B"},     {"LD H,C"},     {"LD H,D"},     {"LD H,E"}, /* 60 - 63 */
    {"LD H,H"},     {"LD H,L"},     {"LD H,(HL)"},  {"LD H,A"}, /* 64 - 67 */
    {"LD L,B"},     {"LD L,C"},     {"LD L,D"},     {"LD L,E"}, /* 68 - 6B */
    {"LD L,H"},     {"LD L,L"},     {"LD L,(HL)"},  {"LD L,A"}, /* 6C - 6F */
    {"LD (HL),B"},  {"LD (HL),C"},  {"LD (HL),D"},  {"LD (HL),E"}, /* 70 - 73 */
    {"LD (HL),H"},  {"LD (HL),L"},  {"HALT"},       {"LD (HL),A"}, /* 74 - 77 */
    {"LD A,B"},     {"LD A,C"},     {"LD A,D"},     {"LD A,E"}, /* 78 - 7B */
    {"LD A,H"},     {"LD A,L"},     {"LD A,(HL)"},  {"LD A,A"}, /* 7C - 7F */
    {"ADD A,B"},    {"ADD A,C"},    {"ADD A,D"},    {"ADD A,E"},/* 80 - 83 */
    {"ADD A,H"},    {"ADD A,L"},    {"ADD A,(HL)"}, {"ADD A,A"},/* 84 - 87 */
    {"ADC A,B"},    {"ADC A,C"},    {"ADC A,D"},    {"ADC A,E"},/* 88 - 8B */
    {"ADC A,H"},    {"ADC A,L"},    {"ADC A,(HL)"}, {"ADC A,A"},/* 8C - 8F */
    {"SUB B"},      {"SUB C"},      {"SUB D"},      {"SUB E"},  /* 90 - 93 */
    {"SUB H"},      {"SUB L"},      {"SUB (HL)"},   {"SUB A"},  /* 94 - 97 */
    {"SBC A,B"},    {"SBC A,C"},    {"SBC A,D"},    {"SBC A,E"},/* 98 - 9B */
    {"SBC A,H"},    {"SBC A,L"},    {"SBC A,(HL)"}, {"SBC A,A"},/* 9C - 9F */
    {"AND B"},      {"AND C"},      {"AND D"},      {"AND E"},  /* A0 - A3 */
    {"AND H"},      {"AND L"},      {"AND (HL)"},   {"AND A"},  /* A4 - A7 */
    {"XOR B"},      {"XOR C"},      {"XOR D"},      {"XOR E"},  /* A8 - AB */
    {"XOR H"},      {"XOR L"},      {"XOR (HL)"},   {"XOR A"},  /* AC - AF */
    {"OR B"},       {"OR C"},       {"OR D"},       {"OR E"},   /* A0 - A3 */
    {"OR H"},       {"OR L"},       {"OR (HL)"},    {"OR A"},   /* A4 - A7 */
    {"CP B"},       {"CP C"},       {"CP D"},       {"CP E"},   /* A8 - AB */
    {"CP H"},       {"CP L"},       {"CP (HL)"},    {"CP A"},   /* AC - AF */
    {"RET NZ"},     {"POP BC"},     {"JP NZ,"},     {"JP "},    /* C0 - C3 */
    {"CALL NZ,"},   {"PUSH BC"},    {"ADD A,"},     {"RST 0"},  /* C4 - C7 */
    {"RET Z"},      {"RET"},        {"JP Z,"},      {""},       /* C8 - CB */
    {"CALL Z,"},    {"CALL "},      {"ADC A,"},     {"RST 8"},  /* CC - CF */
    {"RET NC"},     {"POP DE"},     {"JP NC,"},     {"OUT "},   /* D0 - D3 */
    {"CALL NC,"},   {"PUSH DE"},    {"SUB "},       {"RST 10H"},/* D4 - D7 */
    {"RET C"},      {"EXX"},        {"JP C,"},      {"IN A,"},  /* D8 - DB */
    {"CALL C,"},    {""},           {"SBC A,"},     {"RST 18H"},/* DC - DF */
    {"RET PO"},     {"POP HL"},     {"JP PO,"},     {"EX (SP),HL"},/* E0 - E3 */
    {"CALL PO,"},   {"PUSH HL"},    {"AND "},       {"RST 20H"},/* E4 - E7 */
    {"RET PE"},     {"JP (HL)"},    {"JP PE,"},     {"EX DE,HL"},/* E8 - EB */
    {"CALL PE,"},   {""},           {"XOR "},       {"RST 28H"},/* EC - EF */
    {"RET P"},      {"POP AF"},     {"JP P,"},      {"DI"},     /* F0 - F3 */
    {"CALL P,"},    {"PUSH AF"},    {"OR "},        {"RST 30H"},/* F4 - F7 */
    {"RET M"},      {"LD SP,HL"},   {"JP M,"},      {"EI"},     /* F8 - FB */
    {"CALL M,"},    {""},           {"CP "},        {"RST 38H"} /* FC - FF */
} ;

/*  Indexed codes (dd, fd ..)   */

struct snementry cbtbl[] = {
    {"RLC "},   {"RRC "},   {"RL "},    {"RR "},        /* 00 - 1F */
    {"SLA "},   {"SRA "},   {""},       {"SRL "},       /* 20 - 3F */
    {"BIT 0,"}, {"BIT 1,"}, {"BIT 2,"}, {"BIT 3,"},     /* 40 - 5F */
    {"BIT 4,"}, {"BIT 5,"}, {"BIT 6,"}, {"BIT 7,"},     /* 60 - 7F */
    {"RES 0,"}, {"RES 1,"}, {"RES 2,"}, {"RES 3,"},     /* 80 - 9F */
    {"RES 4,"}, {"RES 5,"}, {"RES 6,"}, {"RES 7,"},     /* A0 - BF */
    {"SET 0,"}, {"SET 1,"}, {"SET 2,"}, {"SET 3,"},     /* C0 - DF */
    {"SET 4,"}, {"SET 5,"}, {"SET 6,"}, {"SET 7,"}      /* E0 - FF */
};

struct snementry regtbl[] = {
    {"B"},      {"C"},      {"D"},      {"E"},
    {"H"},      {"L"},      {"(HL)"},   {"A"},
};

struct snementry ddcbtbl[] = {
    {"RLC "},   {"RRC "},   {"RL "},    {"RR "},
    {"SLA "},   {"SRA "},   {"??? "},   {"SRL "},
    {"BIT "},   {"RES "},   {"SET "}
};

struct mnementry dd1tbl[] = {
    {"ADD "},   {"ADD "},   {"INC "},   {"ADD "},
    {"DEC "},   {"ADD "},   {"POP "},   {"EX (SP),"},
    {"PUSH "},  {"JP ("},   {"LD SP,"}
};

struct mnementry dd2tbl[] = {
    {"ADD A,(I"},   {"ADC A,(I"},   {"SUB (I"},
    {"SBC A,(I"},   {"AND (I"},     {"XOR (I"},
    {"OR (I"},      {"CP (I"}
};

/*  Miscellaneous codes (ed ..)     */

struct mnementry edtbl[] = {
    {"IN B,(C)"},   {"OUT (C),B"},  {"SBC HL,BC"},  {"LD "},        /*40-43*/
    {"NEG"},        {"RETN"},       {"IM 0"},       {"LD I,A"},     /*44-47*/
    {"IN C,(C)"},   {"OUT (C),C"},  {"ADC HL,BC"},  {"LD BC,"},     /*48-4B*/
    {"DEFB "},      {"RETI"},       {"DEFB "},      {"LD R,A"},     /*4C-4F*/
    {"IN D,(C)"},   {"OUT (C),D"},  {"SBC HL,DE"},  {"LD "},        /*50-53*/
    {"DEFB "},      {"DEFB "},      {"IM 1"},       {"LD A,I"},     /*54-57*/
    {"IN E,(C)"},   {"OUT (C),E"},  {"ADC HL,DE"},  {"LD DE,"},     /*58-5B*/
    {"DEFB "},      {"DEFB "},      {"IM 2"},       {"LD A,R"},     /*5C-5F*/
    {"IN H,(C)"},   {"OUT (C),H"},  {"SBC HL,HL"},  {"DEFB "},      /*60-63*/
    {"DEFB "},      {"DEFB "},      {"DEFB "},      {"RRD"},        /*64-67*/
    {"IN L,(C)"},   {"OUT (C),L"},  {"ADC HL,HL"},  {"DEFB "},      /*68-6B*/
    {"DEFB "},      {"DEFB "},      {"DEFB "},      {"RLD"},        /*6C-6F*/
    {"DEFB "},      {"DEFB "},      {"SBC HL,SP"},  {"LD "},        /*70-73*/
    {"DEFB "},      {"DEFB "},      {"DEFB "},      {"DEFB "},      /*74-77*/
    {"IN A,(C)"},   {"OUT (C),A"},  {"ADC HL,SP"},  {"LD SP,"},     /*78-7B*/

    {"LDI"},        {"CPI"},        {"INI"},        {"OUTI"},       /*A0-A3*/
    {"DEFB "},      {"DEFB "},      {"DEFB "},      {"DEFB "},      /*A4-A7*/
    {"LDD"},        {"CPD"},        {"IND"},        {"OUTD"},       /*A8-AB*/
    {"DEFB "},      {"DEFB "},      {"DEFB "},      {"DEFB "},      /*AC-AF*/
    {"LDIR"},       {"CPIR"},       {"INIR"},       {"OTIR"},       /*B0-B3*/
    {"DEFB "},      {"DEFB "},      {"DEFB "},      {"DEFB "},      /*B4-B7*/
    {"LDDR"},       {"CPDR"},       {"INDR"},       {"OTDR"}        /*B8-BB*/
};

/* OPTTBL (option table) entries:

   bit 7 = unconditional transfer instruction
   bit 6 = special processing (cb, dd, ed, fd opcodes)
   bit 5 = relative addressing (jr & djnz instructions)
   bit 4 = direct reference (jp or call)
   bit 3 = parenthesized address
   bit 2 = immediate data
   bit 1 = 3 byte instruction
   bit 0 = 2 byte instruction
      if entry = 0, instruction is single byte, no options
*/

unsigned char opttbl[] = {
   0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    /* 00 - 07 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    /* 08 - 0f */
   0x21, 0x16, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    /* 10 - 17 */
   0xa1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    /* 18 - 1f */
   0x21, 0x16, 0x1e, 0x00, 0x00, 0x00, 0x05, 0x00,    /* 20 - 27 */
   0x21, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x05, 0x00,    /* 28 - 2f */
   0x21, 0x16, 0x1e, 0x00, 0x00, 0x00, 0x05, 0x00,    /* 30 - 37 */
   0x21, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x05, 0x00,    /* 38 - 3f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 40 - 47 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 48 - 4f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 50 - 57 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 58 - 5f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 60 - 67 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 68 - 6f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,    /* 70 - 77 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 78 - 7f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 80 - 87 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 88 - 8f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 90 - 97 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 98 - 9f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* a0 - a7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* a8 - af */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* b0 - b7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* b8 - bf */
   0x00, 0x00, 0x16, 0x96, 0x16, 0x00, 0x05, 0x00,    /* c0 - c7 */
   0x00, 0x80, 0x16, 0x41, 0x16, 0x16, 0x05, 0x00,    /* c8 - cf */
   0x00, 0x00, 0x16, 0x09, 0x16, 0x00, 0x05, 0x00,    /* d0 - d7 */
   0x00, 0x00, 0x16, 0x09, 0x16, 0x40, 0x05, 0x00,    /* d8 - df */
   0x00, 0x00, 0x16, 0x00, 0x16, 0x00, 0x05, 0x00,    /* e0 - e7 */
   0x00, 0x80, 0x16, 0x00, 0x16, 0x40, 0x05, 0x00,    /* e8 - ef */
   0x00, 0x00, 0x16, 0x00, 0x16, 0x00, 0x05, 0x00,    /* f0 - f7 */
   0x00, 0x00, 0x16, 0x00, 0x16, 0x40, 0x05, 0x00     /* f8 - ff */
} ;

/*  Flags for ED codes:
    00 - invalid code
    01 - 2 byte, entry in edtbl
    03 - xx,(nn) where xx = bc, de, or sp
    13 - (nn),bc
    23 - (nn),de
    33 - (nn),sp
    81 - retn or reti
*/

unsigned char edcode[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 00 - 07 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 08 - 0f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 10 - 17 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 18 - 1f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 20 - 27 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 28 - 2f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 30 - 37 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 38 - 3f */
   0x01, 0x01, 0x01, 0x13, 0x01, 0x81, 0x01, 0x01,    /* 40 - 47 */
   0x01, 0x01, 0x01, 0x03, 0x00, 0x81, 0x00, 0x01,    /* 48 - 4f */
   0x01, 0x01, 0x01, 0x23, 0x00, 0x00, 0x01, 0x01,    /* 50 - 57 */
   0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x01, 0x01,    /* 58 - 5f */
   0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,    /* 60 - 67 */
   0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,    /* 68 - 6f */
   0x00, 0x00, 0x01, 0x33, 0x00, 0x00, 0x00, 0x00,    /* 70 - 77 */
   0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00,    /* 78 - 7f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 80 - 87 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 88 - 8f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 90 - 97 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 98 - 9f */
   0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,    /* a0 - a7 */
   0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,    /* a8 - af */
   0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,    /* b0 - b7 */
   0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,    /* b8 - bf */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* c0 - c7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* c8 - cf */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* d0 - d7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* d8 - df */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* e0 - e7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* e8 - ef */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* f0 - f7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     /* f8 - ff */
} ;

/*  Flags for DD and FD codes:
    00 - invalid code
    x1 - 2 byte entry in dd1tbl
    x2 - 3 byte load codes
    x3 - 4 byte direct address codes
    x6 - 3 byte arithmetic codes
    x7 - 4 byte dd/fd cb codes
*/
unsigned char ddcode[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 00 - 07 */
   0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 08 - 0f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 10 - 17 */
   0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 18 - 1f */
   0x00, 0x03, 0x13, 0x21, 0x00, 0x00, 0x00, 0x00,    /* 20 - 27 */
   0x00, 0x31, 0x23, 0x41, 0x00, 0x00, 0x00, 0x00,    /* 28 - 2f */
   0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x33, 0x00,    /* 30 - 37 */
   0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 38 - 3f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00,    /* 40 - 47 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00,    /* 48 - 4f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00,    /* 50 - 57 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x00,    /* 58 - 5f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x00,    /* 60 - 67 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x00,    /* 68 - 6f */
   0x82, 0x92, 0xa2, 0xb2, 0xc2, 0xd2, 0x00, 0xe2,    /* 70 - 77 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x00,    /* 78 - 7f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,    /* 80 - 87 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00,    /* 88 - 8f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x00,    /* 90 - 97 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00,    /* 98 - 9f */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00,    /* a0 - a7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x00,    /* a8 - af */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00,    /* b0 - b7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00,    /* b8 - bf */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* c0 - c7 */
   0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00,    /* c8 - cf */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* d0 - d7 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* d8 - df */
   0x00, 0x61, 0x00, 0x71, 0x00, 0x81, 0x00, 0x00,    /* e0 - e7 */
   0x00, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* e8 - ef */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* f0 - f7 */
   0x00, 0xa1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     /* f8 - ff */
} ;

/*  End of DZ80.TBL  */

