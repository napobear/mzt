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
 * 8080/8085 Disassembler
 * Copyright (C) 1990-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d80table.c - Mnemonic Tables
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

#include "mdz80.h"

// blank entries are defb invalid opcodes

struct mnementry mnemtbl80[] = {
    {"NOP"},       {"LXI B,"},     {"STAX B"},    {"INX B"},  // 00 - 03
    {"INR B"},     {"DCR B"},      {"MVI B,"},    {"RLC"},    // 04 - 07
    {""},          {"DAD B"},      {"LDAX B"},    {"DCX B"},  // 08 - 0B
    {"INR C"},     {"DCR C"},      {"MVI C,"},    {"RRC"},    // 0C - 0F
    {""},          {"LXI D,"},     {"STAX D"},    {"INX D"},  // 10 - 13
    {"INR D"},     {"DCR D"},      {"MVI D,"},    {"RAL"},    // 14 - 17
    {""},          {"DAD D"},      {"LDAX D"},    {"DCX D"},  // 18 - 1B
    {"INR E"},     {"DCR E"},      {"MVI E,"},    {"RAR"},    // 1C - 1F
    {"RIM"},       {"LXI H,"},     {"SHLD "},     {"INX H"},  // 20 - 23
    {"INR H"},     {"DCR H"},      {"MVI H,"},    {"DAA"},    // 24 - 27
    {""},          {"DAD H"},      {"LHLD "},     {"DCX H"},  // 28 - 2B
    {"INR L"},     {"DCR L"},      {"MVI L,"},    {"CMA"},    // 2C - 2F
    {"SIM"},       {"LXI SP,"},    {"STA "},      {"INX SP"}, // 30 - 33
    {"INR M"},     {"DCR M"},      {"MVI M,"},    {"STC"},    // 34 - 37
    {""},          {"DAD SP"},     {"LDA "},      {"DCX SP"}, // 38 - 3B
    {"INR A"},     {"DCR A"},      {"MVI A,"},    {"CMC"},    // 3C - 3F
    {"MOV B,B"},   {"MOV B,C"},    {"MOV B,D"},   {"MOV B,E"},// 40 - 43
    {"MOV B,H"},   {"MOV B,L"},    {"MOV B,M"},   {"MOV B,A"},// 44 - 47
    {"MOV C,B"},   {"MOV C,C"},    {"MOV C,D"},   {"MOV C,E"},// 48 - 4B
    {"MOV C,H"},   {"MOV C,L"},    {"MOV C,M"},   {"MOV C,A"},// 4C - 4F
    {"MOV D,B"},   {"MOV D,C"},    {"MOV D,D"},   {"MOV D,E"},// 50 - 53
    {"MOV D,H"},   {"MOV D,L"},    {"MOV D,M"},   {"MOV D,A"},// 54 - 57
    {"MOV E,B"},   {"MOV E,C"},    {"MOV E,D"},   {"MOV E,E"},// 58 - 5B
    {"MOV E,H"},   {"MOV E,L"},    {"MOV E,M"},   {"MOV E,A"},// 5C - 5F
    {"MOV H,B"},   {"MOV H,C"},    {"MOV H,D"},   {"MOV H,E"},// 60 - 63
    {"MOV H,H"},   {"MOV H,L"},    {"MOV H,M"},   {"MOV H,A"},// 64 - 67
    {"MOV L,B"},   {"MOV L,C"},    {"MOV L,D"},   {"MOV L,E"},// 68 - 6B
    {"MOV L,H"},   {"MOV L,L"},    {"MOV L,M"},   {"MOV L,A"},// 6C - 6F
    {"MOV M,B"},   {"MOV M,C"},    {"MOV M,D"},   {"MOV M,E"},// 70 - 73
    {"MOV M,H"},   {"MOV M,L"},    {"HLT"},       {"MOV M,A"},// 74 - 77
    {"MOV A,B"},   {"MOV A,C"},    {"MOV A,D"},   {"MOV A,E"},// 78 - 7B
    {"MOV A,H"},   {"MOV A,L"},    {"MOV A,M"},   {"MOV A,A"},// 7C - 7F
    {"ADD B"},     {"ADD C"},      {"ADD D"},     {"ADD E"},  // 80 - 83
    {"ADD H"},     {"ADD L"},      {"ADD M"},     {"ADD A"},  // 84 - 87
    {"ADC B"},     {"ADC C"},      {"ADC D"},     {"ADC E"},  // 88 - 8B
    {"ADC H"},     {"ADC L"},      {"ADC M"},     {"ADC A"},  // 8C - 8F
    {"SUB B"},     {"SUB C"},      {"SUB D"},     {"SUB E"},  // 90 - 93
    {"SUB H"},     {"SUB L"},      {"SUB M"},     {"SUB A"},  // 94 - 97
    {"SBB B"},     {"SBB C"},      {"SBB D"},     {"SBB E"},  // 98 - 9B
    {"SBB H"},     {"SBB L"},      {"SBB M"},     {"SBB A"},  // 9C - 9F
    {"ANA B"},     {"ANA C"},      {"ANA D"},     {"ANA E"},  // A0 - A3
    {"ANA H"},     {"ANA L"},      {"ANA M"},     {"ANA A"},  // A4 - A7
    {"XRA B"},     {"XRA C"},      {"XRA D"},     {"XRA E"},  // A8 - AB
    {"XRA H"},     {"XRA L"},      {"XRA M"},     {"XRA A"},  // AC - AF
    {"ORA B"},     {"ORA C"},      {"ORA D"},     {"ORA E"},  // A0 - A3
    {"ORA H"},     {"ORA L"},      {"ORA M"},     {"ORA A"},  // A4 - A7
    {"CMP B"},     {"CMP C"},      {"CMP D"},     {"CMP E"},  // A8 - AB
    {"CMP H"},     {"CMP L"},      {"CMP M"},     {"CMP A"},  // AC - AF
    {"RNZ"},       {"POP B"},      {"JNZ "},      {"JMP "},   // C0 - C3
    {"CNZ "},      {"PUSH B"},     {"ADI "},      {"RST 0"},  // C4 - C7
    {"RZ"},        {"RET"},        {"JZ "},       {""},       // C8 - CB
    {"CZ "},       {"CALL "},      {"ACI "},      {"RST 1"},  // CC - CF
    {"RNC"},       {"POP D"},      {"JNC "},      {"OUT "},   // D0 - D3
    {"CNC "},      {"PUSH D"},     {"SUI "},      {"RST 2"},  // D4 - D7
    {"RC"},        {""},           {"JC "},       {"IN "},    // D8 - DB
    {"CC "},       {""},           {"SBI "},      {"RST 3"},  // DC - DF
    {"RPO"},       {"POP H"},      {"JPO "},      {"XTHL"},   // E0 - E3
    {"CPO "},      {"PUSH H"},     {"ANI "},      {"RST 4"},  // E4 - E7
    {"RPE"},       {"PCHL"},       {"JPE "},      {"XCHG"},   // E8 - EB
    {"CPE "},      {""},           {"XRI "},      {"RST 5"},  // EC - EF
    {"RP"},        {"POP PSW"},    {"JP "},       {"DI"},     // F0 - F3
    {"CP "},       {"PUSH PSW"},   {"ORI "},      {"RST 6"},  // F4 - F7
    {"RM"},        {"SPHL"},       {"JM "},       {"EI"},     // F8 - FB
    {"CM "},       {""},           {"CPI "},      {"RST 7"}   // FC - FF
} ;

/* OPTTBL (option table) entries:

   bit 7 = unconditional transfer instruction
   bit 6 = unused
   bit 5 = unused
   bit 4 = direct reference (jmp, call, etc)
   bit 3 = invalid opcode (defb)
   bit 2 = immediate data
   bit 1 = 3 byte instruction
   bit 0 = 2 byte instruction
      if entry = 0, instruction is single byte, no options
*/

unsigned char opttbl80[] = {
   0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    // 00 - 07
   0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    // 08 - 0f
   0x08, 0x16, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    // 10 - 17
   0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    // 18 - 1f
   0x00, 0x16, 0x16, 0x00, 0x00, 0x00, 0x05, 0x00,    // 20 - 27
   0x08, 0x00, 0x16, 0x00, 0x00, 0x00, 0x05, 0x00,    // 28 - 2f
   0x00, 0x16, 0x16, 0x00, 0x00, 0x00, 0x05, 0x00,    // 30 - 37
   0x08, 0x00, 0x16, 0x00, 0x00, 0x00, 0x05, 0x00,    // 38 - 3f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 40 - 47
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 48 - 4f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 50 - 57
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 58 - 5f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 60 - 67
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 68 - 6f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,    // 70 - 77
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 78 - 7f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 80 - 87
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 88 - 8f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 90 - 97
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 98 - 9f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // a0 - a7
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // a8 - af
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // b0 - b7
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // b8 - bf
   0x00, 0x00, 0x16, 0x96, 0x16, 0x00, 0x05, 0x00,    // c0 - c7
   0x00, 0x80, 0x16, 0x08, 0x16, 0x16, 0x05, 0x00,    // c8 - cf
   0x00, 0x00, 0x16, 0x05, 0x16, 0x00, 0x05, 0x00,    // d0 - d7
   0x00, 0x08, 0x16, 0x05, 0x16, 0x08, 0x05, 0x00,    // d8 - df
   0x00, 0x00, 0x16, 0x00, 0x16, 0x00, 0x05, 0x00,    // e0 - e7
   0x00, 0x80, 0x16, 0x00, 0x16, 0x08, 0x05, 0x00,    // e8 - ef
   0x00, 0x00, 0x16, 0x00, 0x16, 0x00, 0x05, 0x00,    // f0 - f7
   0x00, 0x00, 0x16, 0x00, 0x16, 0x08, 0x05, 0x00     // f8 - ff
} ;

// end of d80table.c
