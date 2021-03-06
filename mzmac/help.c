// Detailed help routines

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>


static int tt = 0;
static int termwidth = 80;		/* default terminal width */
static char f_color = 0;

void print ( char *str );
void tprint ( char *str );
void hlink ( char *, char );
void tlink ( char *, char );

// ANSI colours table.
// ---------------------------------------
// Black       0;30     Dark Gray     1;30
// Blue        0;34     Light Blue    1;34
// Green       0;32     Light Green   1;32
// Cyan        0;36     Light Cyan    1;36
// Red         0;31     Light Red     1;31
// Purple      0;35     Light Purple  1;35
// Brown       0;33     Yellow        1;33
// Light Gray  0;37     White         1;37
// 1: Bold
// 4: Underscore
// 5: Blink
// 7: Inverse
// 8: Concealed
// 44 for Blue background,
// 41 for a Red background, etc
// ---------------------------------------

// Assign one of the above to colorize output and end it with "m"
static char *c_strong = "1m";
static char *c_head = "1;31m";
static char *c_name = "0;36m";
static char *c_link = "0;32m";
// Don't touch this
static char *c_pfx = "\033[";
static char *c_rst = "\033[0m";



static char *documentation =
        ".Overview of zmac\n"
        "zmac is a Z-80 macro cross-assembler. It has all the features you'd\n"
        "expect. It assembles the specified input file (with a '.z' extension\n"
        "if there is no pre-existing extension and the file as given doesn't\n"
        "exist) and produces program output in many different formats@@format.\n"
        "It also produces a nicely-formatted\n"
        "listing of the machine code and cycle counts alongside the source\n"
        "in a \".lst\" file.\n"
        "\n"
        "To reduce clutter and command line options, all zmac output is put\n"
        "into an (auto-created) _zout_ subdirectory.  For _file.z_ the listing\n"
        "will be in _zout/file.lst_, the TRS-80 executable format in _zout/file.cmd_\n"
        "and so on.\n"
        "\n"
        "Undocumented@undoc Z-80 instructions are supported as well as 8080 code.\n"
        "\n"
        "zmac strives to be a powerful assembler with expressions familiar to C\n"
        "programmers while providing good backward compatibility with original\n"
        "assemblers such as Edtasm, MRAS and Macro-80.\n"
        "---\n"
        ".Usage\n"
        "zmac\n"
        "[ --help ]\n"
        "[ --version ]\n"
        "[ --dep ]\n"
        "[ --mras ]\n"
        "[ --rel ]\n"
        "[ --doc ]\n"
        "[ --html ]\n"
        "[ -8bcefghijJlLmnopstz ]\n"
        "[ filename[.z] ]\n"
        "\n"
        ".Options\n"
        "\n"
        " --help\n"
        "  Display a list of options and a terse description of what the options do.\n"
        " \n"
        " --version\n"
        "  Print zmac version name.\n"
        " \n"
        " --mras\n"
        "  MRAS compatibility mode.  Any _?_ in a label will be expanded to the\n"
        "  current module identifier as set by _*mod_.\n"
        " \n"
        " --rel\n"
        "  Output \".rel\" (relocatable object file) format only.\n"
        " \n"
        " --zmac\n"
        "  zmac compatibility mode.  _defl_ labels are undefined after each pass.\n"
        "  Quotes and double quotes are stripped from macro arguments before expansion.\n"
        "  _$_ is ignored in identifiers allowing _foo$bar_ to construct identifiers\n"
        "  in macro expansions.  Use _`_ (backquote) instead in normal mode.  Labels\n"
        "  starting with _\".\"_ are temporary and are reset whenever a non-temporary\n"
        "  label is defined (thus they may be reused).  Labels starting with _\"__\"_\n"
        "  are local to their file thus avoid multiple definition when brought in\n"
        "  with _include_.\n"
        " \n"
        " --dep\n"
        "  Print all files read by _include_ and _incbin_.\n"
        " \n"
        " --doc\n"
        "  Print this documentation in HTML format to standard output.\n"
        " \n"
        " -8\n"
        "  Accept 8080 mnemonics preferentially.  Equivalent to _.8080_ pseudo-op.\n"
        " \n"
        " -b\n"
        "  Don't generate any machine code output at all.\n"
        " \n"
        " -c\n"
        "  Make the listing continuous, i.e., don't generate any page breaks or\n"
        "  page headers. Can make things less confusing if you're going to\n"
        "  consult the listing online rather than printing it.  This is the default.\n"
        " \n"
        " -e\n"
        "  Omit the \"error report\" section in the listing.\n"
        " \n"
        " -f\n"
        "  List instructions not assembled due to \"_if_\" expressions being\n"
        "  false. (Normally these are not shown in the listing.)\n"
        " \n"
        " -g\n"
        "  List only the first line of equivalent hex for a source line.\n"
        " \n"
        " -h\n"
        "  Display a list of options and a terse description of what the options do.\n"
        "  (same as --help)\n"
        " \n"
        " -i\n"
        "  Don't list files included with _include_ or _read_.\n"
        " \n"
        " -I dir\n"
        "  Add _dir_ to the end of the include file search path.\n"
        " \n"
        " -j\n"
        "  Promote relative jumps and _DJNZ_ to absolute equivalents as needed.\n"
        " \n"
        " -J\n"
        "  Error if an absolute jump could be replaced with a relative jump.\n"
        " \n"
        " -l\n"
        "  Don't generate a listing at all.\n"
        " \n"
        " -L\n"
        "  Generate listing no matter what. Overrides any conflicting options.\n"
        " \n"
        " -m\n"
        "  List macro expansions.\n"
        " \n"
        " -n\n"
        "  Omit line numbers from listing.\n"
        " \n"
        " -o\n"
        "  List to standard output.\n"
        " \n"
        " -p\n"
        "  Use a few linefeeds for page break in listing rather than ^L.\n"
        " \n"
        " -P\n"
        "  Output listing for a printer with headers, multiple symbols per column, etc.\n"
        " \n"
        " -s\n"
        "  Omit the symbol table from the listing.\n"
        " \n"
        " -t\n"
        "  Only output number of errors instead list of each one.\n"
        " \n"
        " -z\n"
        "  Accept Z-80 mnemonics preferentially.  Equivalent to _.z80_ pseudo-op.\n"
        "---\n"
        ".Input Format\n"
        "\n"
        "zmac uses the standard Zilog mnemonics, and the pseudo-ops are also\n"
        "largely as you'd expect.\n"
        "\n"
        "A \"_._\" may optionally preceeed any psuedo-op.\n"
        "For example, \"_.org_\" and \"_org_\" are treated as equivalent.\n"
        "\n"
        "Input can be upper or lowercase.\n"
        "\n"
        "Comments start with _;_ and carry on to the end of the line.\n"
        "\n"
        "Number constants can take a trailing h or a leading $ or 0x for hex,\n"
        "a trailing b for binary, a trailing o or q for octal, or a trailing\n"
        "d for decimal.\n"
        "\n"
        "_'LH'_ (any length 2 string) can be treated as a number whose value\n"
        "is _'H'_ * 256 + _'L'_.\n"
        "\n"
        "Labels are declared with _label:_ or just _label_ - indentation is unimportant.\n"
        "Labels can be up to 40 chars long.  They can start with and contain \n"
        "letters, digits, _$_, _._, _?_, _@_ and __.  Ambiguous identifiers like\n"
        "_$FCB_ will be treated as hex constants unless defined as a label.  Labels\n"
        "declared with two colons (_label::_) make the label public.\n"
        "\n"
        "Here is how other things work.  Numbers are used as examples, but a full\n"
        "expression@expr can be used in their place.\n"
        "\n"
        "..Data\n"
        "\n"
        "_defb 42_\n"
        "|A byte.  _ascii_, _byte_, _db_, _defm_ and _text_ are synonyms.\n"
        "\n"
        "_defb 'foobar'_\n"
        "|An ASCII character string (not NUL-terminated).\n"
        "|Double quotes can also be used.\n"
        "\n"
        "_defb 'Who needs anything more than CP/M?',13,10,'$'_\n"
        "|Strings and bytes can mix together.\n"
        "\n"
        "_defw 2112_\n"
        "\n"
        "_defw $123,0x456_\n"
        "|A word (16 bits).  _word_ and _dw_ are synonyms.\n"
        "\n"
        "_defd $12345678_\n"
        "|A double word (32 bits). _dword_ is a synonym.\n"
        "\n"
        "_defs 500_\n"
        "|Skip output ahead 500 bytes.  This will insert 500 zeros in the \".ams\"\n"
        "|and \".cim\" output files or if inside a \".phase\" section.\n"
        "|_block_, _ds_ and _rmem_ are synonyms.\n"
        "\n"
        "_dc 'string'_\n"
        "|Like _ascii_ but accepts only a single string and the high bit of the\n"
        "|last character will be set.\n"
        "\n"
        "_dc count,value_\n"
        "|Repeat the byte _value_ a total of _count_ times.  Similar to _defs_\n"
        "|except that memory is always filled with _value_.\n"
        "\n"
        "_incbin file_\n"
        "|Inserts the raw contents of the file into the assembly.  Simpler for\n"
        "|large amounts of data.\n"
        "\n"
        "..Symbols\n"
        "\n"
        "_label equ 100_\n"
        "|Define a symbol to have a fixed value.  The symbol can be used before it\n"
        "|is defined.  A symbol defined with _equ_ or as a label can be defined only\n"
        "|once, except that a symbol defined with _equ_ may be redefined to the\n"
        "|same value.\n"
        "\n"
        "_label defl 200_\n"
        "|Define a symbol to have a changeable value.  The symbol cannot be used\n"
        "|before it is defined, and it can be redefined to a different value later\n"
        "|with another _defl_. _aset_ is a synonym.\n"
        "\n"
        "_min_\n"
        "\n"
        "_max_\n"
        "|Same as _defl_ except that the symbol is defined as the\n"
        "|smaller or bigger of two comma-separated expressions.\n"
        "\n"
        "_*mod_\n"
        "|Increment the internal module name string.  The first time this results\n"
        "|in \"a\".  Then \"b\", \"c\", ... \"z\".  Then \"aa\", \"ab\", \"ac\", etc. all the way\n"
        "|up to \"zzzz\".  The module name string is used in _--mras_ mode where \"?\" in\n"
        "|label names is replaced with the current module name.\n"
        "\n"
        "_extern lab1,lab2,..._\n"
        "|The listed labels are defined in an external module for later linking.\n"
        "|No effect unless zmac is producing \".rel\" output.\n"
        "|_ext_ and _extrn_ are synonyms.\n"
        "\n"
        "_public lab1,lab2,..._\n"
        "|The given labels will be visible to external modules when linking.\n"
        "|No effect unless zmac is producing \".rel\" output.\n"
        "|_global_ and _entry_ are synonyms.\n"
        "\n"
        "..Location Control\n"
        "\n"
        "_org 9000h_\n"
        "|Set the address to assemble to 0x9000.\n"
        "\n"
        "_phase address_\n"
        "|Continue to produce code and data for loading at the current address\n"
        "|but assemble instructions and define labels as if they originated at\n"
        "|the given address.  Useful when producing code that will be copied to\n"
        "|a different location before being executed (e.g., an overlay).\n"
        "\n"
        "_dephase_\n"
        "|End _phase_ mode assembly.\n"
        "\n"
        "_aseg_\n"
        "_cseg_\n"
        "_dseg_\n"
        "|Switch to the absolute, code and data segments respectively.\n"
        "|No effect unless zmac is producing \".rel\" output.\n"
        "\n"
        "..Input Control\n"
        "\n"
        "_end_\n"
        "|Ends the input.  Any lines after an _end_ are silently ignored.\n"
        "|If an arg is given, it declares the entry address for the program.\n"
        "|This has no effect in \".cim\" output. In \".hex\" output\n"
        "|it generates an S-record directing 0 bytes of data to be loaded\n"
        "|at the given address.\n"
        "\n"
        "_if_ ... [ _else_ ... ] _endif_\n"
        "|For conditional assembly. If you do _if foo_ and _foo_ evaluates to\n"
        "|zero, all the lines up until the next corresponding _else_ or _endif_\n"
        "|are completely ignored.  Conversely, if _foo_ evaluates to non-zero, any\n"
        "|lines from a corresponding _else_ to the _endif_ are ignored.  Ifs can\n"
        "|be nested.  _cond_/_endc_ are synonyms for _if_/_endif_.\n"
        "\n"
        "_include file_\n"
        "|Include a file. Like C's (well, cpp's) #include and follows the same\n"
        "|include path search rules, but the filename arg\n"
        "|lacks the angle brackets or quotes (though single or double quotes may be used).\n"
        "|_read_ is a synonym.\n"
        "\n"
        "_comment X_\n"
        "|Suspend assembly until the next occurence of character _X_ on a line.\n"
        "|The rest of the line will be ignored.  A multi-line comment.\n"
        "\n"
        "_assert expr_\n"
        "|Stop assembly if _expr_ is non-zero.\n"
        "\n"
        "..Cycle Counting\n"
        "\n"
        "_sett expr_\n"
        "|Set the current T-state count to _expr_. _tstate_ is a synonym.\n"
        "\n"
        "_setocf expr_\n"
        "|Set the current opcode fetch count to _expr_.\n"
        "\n"
        "..Code Generation\n"
        "\n"
        "_8080_\n"
        "|Make cycle counting operators return 8080 cycle counts and\n"
        "|interpret any ambiguous assembly statements as Intel 8080 mnemonics.\n"
        "|_CP_ will be interpreted as \"call on positive\" and _JP_ as \"jump on positive\".\n"
        "\n"
        "_z80_\n"
        "|Make cycle counting operators return Z-80 cycle counts and\n"
        "|interpret any ambiguous assembly statements as Zilog Z-80 mnemonics.\n"
        "|_CP_ will be interpreted as \"compare accumulator\" and _JP_ as \"jump unconditionally\".\n"
        "\n"
        "_jperror enable_\n"
        "|If _enable_ is non-zero, turn on errors when _JR_ instructions could be used\n"
        "|instead of _JP_, off otherwise.  Used to check existing code for situations\n"
        "|where shorter code could be generated.  Same as _-J_ option.\n"
        "|No effect if in 8080 mode.\n"
        "\n"
        "_jrpromote enable_\n"
        "|If _enable_ is non-zero, _JR_ and _DJNZ_ instructions will be promoted to\n"
        "|equivalent _JP_ and _DEC##B_, _JP##NZ_ instructions if the relative branch\n"
        "|offset is out of range.  If _enable_ is zero, promotion is disabled.\n"
        "|Same as the _-j_ option.\n"
        "|No effect if in 8080 mode.\n"
        "\n"
        "..Undocumented Instructions\\undoc\n"
        "\n"
        "Most Z-80 chips support a number of undocumented instructions that were part of\n"
        "the original design but not made an offical part of the Zilog specification.\n"
        "These instructions may not be supported by all Z-80 chips, especially\n"
        "licensed variants, but are fairly widely available nonetheless.\n"
        "\n"
        "_sl1 r_\n"
        "|Same as _sla r_ but shifts a 1 into the lower bit of _r_ rather than a 0.\n"
        "\n"
        "_in (c)_\n"
        "|Inputs a byte from port _c_ but does not store the value.  Flags are still\n"
        "|set as with the normal _in r,(c)_ instruction.\n"
        "\n"
        "_out (c),0_\n"
        "|Outputs a zero to port _c_.\n"
        "\n"
        "_bit/set/res n,(ix+d),r_\n"
        "\n"
        "_rlc/rrc/rl/rr/sla/sl1/sra/srl (iy+d),r_\n"
        "|Same as the corresponding operation on just _(ix+d)_ or _(iy+d)_ but with\n"
        "|the result being stored both into _(ix+d)_ and register _r_.  Except for _bit_\n"
        "|which has no effect on _r_. zmac supports the syntax to allow those\n"
        "|instruction patterns to be generated.\n"
        "\n"
        "The upper and lower bytes of the _ix_ and _iy_ can be used in a number of\n"
        "instructions much in the same way as _d_ and _e_ correspond to the upper and\n"
        "lower bytes of _de_.  zmac names these _ixh_, _ixl_, _iyh_ and _iyl_ and\n"
        "are referred to generically as _ixylh_ here.\n"
        "\n"
        "_inc/dec/add/adc/sub/sbc/and/xor/or/cp ixylh_\n"
        "|Arithmetic or logical operation on _ix_ or _iy_ high or low byte.\n"
        "\n"
        "_ld a/b/c/d/e,ixylh_\n"
        "|Load register with _ix_ or _iy_ high or low byte.\n"
        "\n"
        "_ld ixylh,a/b/c/d/e_\n"
        "|Load _ix_ or _iy_ high or low byte with register.\n"
        "\n"
        "_pfix_\n"
        "\n"
        "_pfiy_\n"
        "|Output $DD and $FD prefix bytes.  The Z-80 allows multiple prefix bytes\n"
        "|for IX and IY instructions.  This allows you to specify them abstractly.\n"
        "|There is little purpose except for delaying an interrupt or confusing\n"
        "|disassemblers.\n"
        "\n"
        "..Miscellaneous\n"
        "\n"
        "_name str_\n"
        "|Set the name of the output model to _str_.  For compatibility reasons\n"
        "|_str_ may be parenthesized (e.g., \"_name ('foo')_\").  Not all output\n"
        "|formats support an internal name and many have severe length limits.\n"
        "\n"
        "_rsym_ and _wsym_\n"
        "|Read/write a symbol file. These simply load/save the currently defined\n"
        "|symbols from/to the file specified (in a non-portable format). _rsym_\n"
        "|takes place at the point it is encountered in the file (on the first\n"
        "|pass); _wsym_ is delayed until assembly has finished.\n"
        "\n"
        "..Listing Pseudo-ops\n"
        "\n"
        "There are several pseudo-ops for controlling the listing. None of\n"
        "these ops appear in the listing themselves:\n"
        "\n"
        "_eject_\n"
        "|Start a new listing page.\n"
        "\n"
        "_nolist_\n"
        "|Do nothing. This can be used to have a comment in the source but not\n"
        "|the listing, I suppose.\n"
        "\n"
        "_elist_, _flist_, _glist_, _mlist_\n"
        "|These have the same effect as the similarly-named command-line\n"
        "|options, though possibly with the sense reversed depending on the\n"
        "|default. Use an arg >0 (or no arg) to enable, and an arg <0 to\n"
        "|disable.\n"
        "\n"
        "_list_\n"
        "|Sets whether to list or not. You can use this to avoid listing certain\n"
        "|parts of the source. Takes same optional arg as 'elist', etc..\n"
        "\n"
        "_title_\n"
        "|Set title (used in listing and symbol file).\n"
        "\n"
        "_space_\n"
        "|Output arg blank lines in the listing, or one line if no arg is given.\n"
        "\n"
        "---\n"
        "\n"
        ".Expressions\\expr\n"
        "\n"
        "Expressions feature a full set of C operators with the same precedence\n"
        "rules and some common assembler extensions and names.\n"
        "Here is the complete list of operators, highest-precedence first.\n"
        "Operators separated only by a space are synonyms; for example, _~_\n"
        "is the same as _not_.\n"
        "\n"
        "_!_ (logical), _~ not_ (bitwise), _+_ (unary), _-_ (unary), _low_, _high_, _t_, _tilo_, _tihi_, _ocf_\n"
        "\n"
        "_*_, _/_, _% mod_\n"
        "\n"
        "_+_, _-_\n"
        "\n"
        "_<< shl_, _>> shr_\n"
        "\n"
        "_< lt_, _> gt_, _<= le_, _>= ge_\n"
        "\n"
        "_== = eq_, _!= <> ne_\n"
        "\n"
        "_& and_ (bitwise)\n"
        "\n"
        "_^ xor_ (bitwise)\n"
        "\n"
        "_| or_ (bitwise)\n"
        "\n"
        "_&&_\n"
        "\n"
        "_||_\n"
        "\n"
        "_? :_  (ternary choice operator)\n"
        "\n"
        "You can use normal parentheses or square brackets to override\n"
        "the precedence rules. Square brackets can be used where parentheses would\n"
        "conflict with Z-80 mnemonic syntax, but this is not necessary in any\n"
        "practical case.\n"
        "\n"
        "The _?_ may need spaces around it to distinguish it from a label that\n"
        "has _?_ in it.\n"
        "\n"
        "The unary operators not familiar to C programmers:\n"
        "\n"
        " low expr\n"
        "  Returns low 8 bits of _expr_\n"
        " \n"
        " high expr\n"
        "  Returns high 8 bits of _expr_\n"
        " \n"
        " t expr\n"
        "  Current count of T-states up to memory location _expr_\n"
        " \n"
        " tilo expr\n"
        "  Low count of T-states used by instruction at memory location _expr_\n"
        " \n"
        " tihi expr\n"
        "  High count of T-states used by instruction at memory location _expr_\n"
        " \n"
        " ocf expr\n"
        "  Current count of opcode fetches up to memory location _expr_\n"
        "---\n"
        ".Macros\n"
        "The following defines a macro named m with zero or more formal parameters\n"
        "_p1_, _p2_, ..., _pn_, zero or more local symbols _?s1_, _?s2_, ..., _?sm_,\n"
        "and body _b1_, _b2_, ...:\n"
        "\n"
        " _m macro p1, p2, ..., pn, ?s1, ?s2, ..., ?sm_\n"
        "  \n"
        " _####b1_\n"
        "  \n"
        " _####b2_\n"
        "  \n"
        " _####..._\n"
        "  \n"
        " _####endm_\n"
        "\n"
        "The macro is called by writing:\n"
        "|_m v1, v2, ..., vn_\n"
        "\n"
        "A macro call expands to the text of the macro's body, with each \n"
        "occurrence of a formal parameter _pk_ replaced by the corresponding \n"
        "value _vk_, and with each local symbol _?sk_ replaced by a new, unique \n"
        "symbol invented for this call.  Invented symbols begin with _?_,\n"
        "so you should avoid using such symbols elsewhere in your program.\n"
        "\n"
        "zmac currently does not check that you have provided the right number \n"
        "of parameters when calling a macro.  If you provide too few, unmatched \n"
        "formals are replaced with the empty string.  If you provide too \n"
        "many, the additional values begin to replace local symbols as if \n"
        "they were ordinary parameters.  (This could be considered a feature.)  \n"
        "After the local symbols are all replaced, additional parameters \n"
        "are silently ignored.\n"
        "\n"
        "For compatibility with Macro-80, the first line of a macro definition can\n"
        "list other labels that will be treated locally:\n"
        "\n"
        "|_####local lab1,lab2,..._\n"
        "\n"
        "Each time the macro is expanded the local labels are replaced with unique\n"
        "names thus avoiding multiple definition problems.\n"
        "\n"
        "For compatability with MRAS, macro arguments may be preceeded by _##_\n"
        "in their definition and use.\n"
        "\n"
        "Any _`_ (backquote) in a macro is ignored thus allowing a macro to\n"
        "construct identifiers.  For example:\n"
        "\n"
        " _move macro dir_\n"
        "  \n"
        " _######ld`dir`r_\n"
        "  \n"
        " _######endm_\n"
        "\n"
        "Invoking _move i_ will construct a _ldir_ block move instruction.\n"
        "---\n"
        ".Compatibility\n"
        "\n"
        "zmac is broadly compatible with many original Z-80 and 8080 assemblers\n"
        "because it accepts many different names for common operations and has\n"
        "liberal identifier and numeric formats.  It also accepts most simple\n"
        "usage of macros.\n"
        "\n"
        "When assembling old code keep these portability problems in mind.\n"
        "\n"
        "Expression order of evaluation may be different.  zmac uses C semantics\n"
        "more order of evaluation but assemblers often used simple left to right\n"
        "ordering.  zmac will evaluate _2+2*3_ as _8_ where other assemblers will\n"
        "yield _12_.\n"
        "\n"
        "zmac has no support operating on strings in macros.  Assemblers like Macro-80\n"
        "could perform conditional tests on strings.\n"
        "\n"
        "Advanced macros are unlikely to work.  zmac hasn't advanced to the state where\n"
        "all the possible ways of substituting parameters are supported.  Nor does it\n"
        "have simple repetition, a way to exit a macro expansion early and so on.\n"
        "\n"
        "Consult the original assembler manual.  zmac error messages won't help you\n"
        "figure out what an unknown assembler command is supposed to do.\n"
        "\n"
        "Compare against original output.  The very safest thing to do when porting\n"
        "assembly code is to compare the binary output of zmac against that produced\n"
        "by the original assembler.  This way you can ensure everything has been\n"
        "interpreted correctly.  Only once that has been achieved should you modify\n"
        "the code.\n"
        "---\n"
        ".Errors and Warnings\n"
        "\n"
        "Any errors or warnings encountered during assembly are reported to standard\n"
        "error and in the listing file.  The errors output immediately give the source\n"
        "file and line number containing the error.  In listings the error letter\n"
        "and message appear just after the line containing the error.\n"
        "\n"
        " B\n"
        "  Balance error\n"
        " ##\n"
        "  A string is missing an closing quote or an _if_ is missing an _endif_\n"
        " E\n"
        "  Expression error\n"
        " ##\n"
        "  An expression did not parse or attempts a divide or modulus by 0.\n"
        " F\n"
        "  Syntax error\n"
        " ##\n"
        "  General problem with the syntax on a line.  Sometimes extra explanation\n"
        "  will be printed on standard output.\n"
        " I\n"
        "  Digit error\n"
        " ##\n"
        "  A numeric constant has too many digits to be represented as a 32 bit number.\n"
        " M\n"
        "  Mult. def. error\n"
        " ##\n"
        "  A symbol has been defined more than once and those values differ.\n"
        " P\n"
        "  Phase error\n"
        " ##\n"
        "  On the second or subsequent assembly passes the assembly has changed\n"
        "  significantly.  Most commonly it means an _if_ has changed conditions\n"
        "  but can also happen when labels or equated values do not converge to\n"
        "  a fixed value.\n"
        " U\n"
        "  Undeclared error\n"
        " ##\n"
        "  An undeclared symbol was used in an expression or _public_ statement.\n"
        " V\n"
        "  Value error\n"
        " ##\n"
        "  An invalid value was given to a statement.  Often this means using less\n"
        "  than -128 or greater then 255 in a _defb_ or less than -32768 or greater\n"
        "  than 65535 in a _defw_.  Or similar invalid values used Z-80/8080 opcodes\n"
        "  requiring an 8 or 16 bit value (and other restrictions like 0 to 7 for _BIT_).\n"
        "  Also if a relative jump is out of range or if a negative value is given\n"
        "  in _defs_ or _dc_.\n"
        " O\n"
        "  Phase/Dephase error\n"
        " ##\n"
        "  _phase_ was used within another _phase_ or _dephase_ without _phase_.\n"
        "  Or if _org_ is used within _phase_.\n"
        " A\n"
        "  Assertion failure error\n"
        " ##\n"
        "  An assert statement evaluated to zero.\n"
        " J\n"
        "  Use JR error\n"
        " ##\n"
        "  An absolute jump instruction was used where relative jump was in range\n"
        "  of the destination address.  Only generated if _-j_ or _jrpromote_ is\n"
        "  in effect.\n"
        " R\n"
        "  Not relocatable error\n"
        " ##\n"
        "  An expression was used that must be generated at link time but cannot\n"
        "  be handled by the \".rel\" format.  For instance, an _org_ to a symbol in\n"
        "  the data segment when in the code segment.  Or a relative jump to a\n"
        "  different segment.  The \".rel\" format can evaluate expressions at link\n"
        "  time using the _high_, _low_, _not_, _-_, _+_, _*_, _/_ and _%_ operators.\n"
        "  zmac is clever enough to use _high_ or _low_ in place of _& $ff00_ and\n"
        "  _& 255_.  But it won't replace a _shl_ with a multiply.\n"
        " G\n"
        "  Register usage error\n"
        " ##\n"
        "  A invalid register was given to an instruction.  For example, _LD B,(DE)_\n"
        "  or _ADD HL,IX_.\n"
        " Z\n"
        "  Z-80 instruction in 8080 mode error\n"
        " ##\n"
        "  An instruction was assembled that is only valid on the Z-80 but _.8080_\n"
        "  (or _-8_) mode is in effect.  However, use use of Z-80 mnemonics that\n"
        "  output valid 8080 instructions is always OK.\n"
        " H\n"
        "  $hex constant interpreted as symbol warning\n"
        " ##\n"
        "  A symbol such as _$FCB_ has been defined even though it could appear to\n"
        "  be a hexadecimal constant.  zmac will treat _$FCB_ as symbol for the entire\n"
        "  assembly which could be rather surprising if that were not the intent.\n"
        " N\n"
        "  Not implemented warning\n"
        " ##\n"
        "  For statements that have been added as parse rules but have no effect.\n"
        "  The only current example is _subttl_ which sets the sub title of a listing\n"
        "  in certain assemblers.\n"
        "\n"
        "---\n"
        ".Output Formats\\format\n"
        "\n"
        "Except for \".rel\", zmac writes every known output when assembling.  This\n"
        "is no burden on modern computers and saves having to come up with options\n"
        "to select a particular output format.\n"
        "\n"
        "\".rel\" is a special case since that format is intended for linking and\n"
        "can have undefined external symbols which would be errors in the other\n"
        "formats which do not support them.\n"
        "\n"
        " .ams\n"
        "  AMSDOS executable format for Amstrad computers.\n"
        " .bds\n"
        "  Experimental format with the potential for source-level debugging.\n"
        " .cas\n"
        "  TRS-80 high-speed (1500 baud) cassette SYSTEM file.  The internal name of the\n"
        "  file is the source file name shortened to 6 characters with suffixes\n"
        "  removed.\n"
        " .cim\n"
        "  Core In-Memory image.  A raw binary format with the first byte corresponding\n"
        "  to the lowest generated code or data and proceeding contiguously until the\n"
        "  highest address generated.  Any gaps are filled with zeros.  Typically used\n"
        "  for CP/M where all executables start at address 256 or for ROM images.\n"
        " .cmd\n"
        "  TRS-80 DOS executable file format as used by all major DOSes on the TRS-80\n"
        "  (TRS-DOS, LDOS, MULTIDOS, NEWDOS, etc.)\n"
        " .hex\n"
        "  Intel hex record format.\n"
        " .lcas\n"
        "  TRS-80 low-speed (500 baud) cassette SYSTEM file.  The internal name of the\n"
        "  file is the source file name shortened to 6 characters with suffixes removed.\n"
        " .rel\n"
        "  Relocatable object module format as produced by MACRO-80 and other assemblers.\n"
        "\n"
        "---\n"
        ".Miscellaneous\n"
        "In the symbol table listing, the _=_ prefix is given for those symbols\n"
        "defined by _equ_ or _defl_.\n"
        "---\n"
        ".Exit Status\n"
        "\n"
        " 0\n"
        "  No errors.\n"
        " 1\n"
        "  One or more errors were found during assembly, or zmac exited with a\n"
        "  fatal error.\n"
        "\n"
        "--\n"
        ".Credits\n"
        "Bruce Norskog original wrote zmac in 1978.\n"
        "\n"
        "Updates and bugfixes over the years by John Providenza, Colin Kelley,\n"
        "and more recently by Russell Marks, Mark RISON, Chris Smith,\n"
        "Matthew Phillips and Tim Mann.\n"
        "\n"
        "Extensive modifications for cycle counting, multiple output formats,\n"
        "\".rel\" output, 8080 mode and older assembler compatibilty were written\n"
        "by George Phillips.\n"
        "\n"
        "This document was based on Russell Marks zmac man page which had\n"
        "tweaks by Mark RISON and Tim Mann.  George Phillips converted it to HTML\n"
        "and documented the new features and some older ones (e.g., _phase_/_dephase_).\n"
        ;



void longhelp ( void ) {
	char line[1024], *p;
	int ch;
	int table = 0, tcol = 0;
	int blockquote = 0;
	struct winsize win;
	char *dp = documentation;

	/* Where is my output ? */
	if (isatty(STDOUT_FILENO)) {
		if ((p = getenv("COLUMNS")) != NULL)
			termwidth = atoi(p);
		else if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == 0 &&
			win.ws_col > 0)
			termwidth = win.ws_col;

		f_color = 1;
	}


	for ( ;; ) {
		p = line;

		for ( ;; ) {
			ch = *dp++;

			if ( ch <= 0 || ch == '\n' )
				break;

			*p++ = ch;
		}

		if ( ch <= 0 )
			break;

		*p = '\0';

		// An initial '|' means part of a block quote
		if ( *line == '|' ) {
			blockquote = 1;
			tprint("    ");
			tprint ( line + 1 );
			tprint ( "\n" );
			continue;
		}

		if ( blockquote ) {
			blockquote = 0;
		}

		// Start with space means you're a table element.
		// Number of spaces indicate which column.  If not, then we drop
		// out of table mode.  Nothing after the spaces is a special case
		// to remain in table mode.
		if ( *line == ' ' ) {
			int depth = 0;
			tcol = 0;

			for ( p = line; *p == ' '; p++ )
				depth++;

			if ( *p == '\0' && depth == 1 )
				continue;

			if ( !table ) {
				table = 1;
			}

			while ( tcol < depth ) {
				tprint ( "\t" );
				tcol++;
			}

			tprint ( p );
			tprint ( "\n" );
			continue;
		}
		else if ( table ) {
			table = 0;
		}

		// Line starting with '-' generates a full horizontal rule
		if ( *line == '-' ) {
			int r = 0;
			while (r < termwidth) {
				tprint ( "-" );
				++r;
			}
			tprint("\n");
			continue;
		}

		// Empty line denotes paragraph separation
		if ( *line == '\0' ) {
			tprint ( "\n" );
			continue;
		}

		// Initial . for a heading.  More . for a lesser heading.
		if ( *line == '.' ) {
			for ( p = line; *p == '.'; p++ )
				;
			if (f_color) {
				tprint(c_pfx);
				tprint(c_head);
			}
			tprint ( p );
			if (f_color)
				tprint(c_rst);
			tprint ( "\n" );
			continue;
		}

		tprint ( line );
		tprint ( "\n" );
	}

}


void html ( void ) {
	char line[1024], *p;
	int ch;
	int table = 0, tcol = 0;
	int blockquote = 0;
	char *dp = documentation;

	for ( ;; ) {
		p = line;

		for ( ;; ) {
			ch = *dp++;

			if ( ch <= 0 || ch == '\n' )
				break;

			*p++ = ch;
		}

		if ( ch <= 0 )
			break;

		*p = '\0';

		// An initial '|' means part of a block quote
		if ( *line == '|' ) {
			if ( !blockquote )
				printf ( "<BLOCKQUOTE>" );

			blockquote = 1;
			print ( line + 1 );
			printf ( "\n" );
			continue;
		}

		if ( blockquote ) {
			printf ( "</BLOCKQUOTE>\n" );
			blockquote = 0;
		}

		// Start with space means you're a table element.
		// Number of spaces indicate which column.  If not, then we drop
		// out of table mode.  Nothing after the spaces is a special case
		// to remain in table mode.
		if ( *line == ' ' ) {
			int depth = 0;

			for ( p = line; *p == ' '; p++ )
				depth++;

			if ( *p == '\0' && depth == 1 )
				continue;

			if ( !table ) {
				printf ( "<TABLE>\n" );
				table = 1;
				tcol = 0;
			}

			if ( depth < tcol ) {
				printf ( "</TD></TR>\n" );
				tcol = 0;
			}

			if ( tcol == 0 ) {
				printf ( "<TR><TD VALIGN=\"TOP\">" );
				printf ( "<PRE>" ); // Column 1 is fixed width font for us
				tcol = 1;
			}

			while ( tcol < depth ) {
				if ( tcol == 1 )
					printf ( "</PRE>" ); // Column 1 is fixed width font for us

				printf ( "</TD><TD>" );
				tcol++;
			}

			print ( p );
			printf ( " " );
			continue;
		}
		else if ( table ) {
			if ( tcol )
				printf ( "</TD></TR>\n" );

			printf ( "</TABLE>\n" );
			table = 0;
		}

		// Line starting with '-' generates a full horizontal rule
		if ( *line == '-' ) {
			printf ( "<HR>\n" );
			continue;
		}

		// Empty line denotes paragraph separation
		if ( *line == '\0' ) {
			printf ( "<P>\n" );
			continue;
		}

		// Initial . for a heading.  More . for a lesser heading.
		if ( *line == '.' ) {
			for ( p = line; *p == '.'; p++ )
				;

			printf ( "<H%d>", 3 + ( int ) ( p - line - 1 ) );
			print ( p );
			printf ( "</H%d>\n", 3 + ( int ) ( p - line - 1 ) );
			continue;
		}

		print ( line );
		printf ( "\n" );
	}

	if ( tt )
		printf ( "</TT>\n" );

	printf ( "<!--\n" );
	printf ( "  If you ran \"zmac --html\" you may want to send the output\n" );
	printf ( "  to a file using \"zmac --html >zmac.html\" and then open\n" );
	printf ( "  zmac.html in your web browser.\n" );
	printf ( "-->\n" );

}

void hlink ( char *str, char how ) {
	char *id = strchr ( str, how );
	char *text = id, save, *id_end;
	int len = 0, spccnt;

	while ( id[len] == how )
		len++;

	for ( id_end = id + len; *id_end >= 'a' && *id_end <= 'z'; id_end++ )
		;

	spccnt = len;

	do {
		if ( text > str )
			text--;

		while ( text > str && *text != ' ' )
			text--;

		spccnt--;
	}
	while ( spccnt > 0 );

	if ( *text == ' ' )
		text++;

	save = *text;
	*text = '\0';
	print ( str );
	*text = save;

	save = *id_end;
	*id_end = '\0';
	printf ( "<A " );

	if ( how == '@' )
		printf ( "HREF=\"#" );
	else
		printf ( "NAME=\"" );

	printf ( "%s\">", id + len );
	*id_end = save;

	save = *id;
	*id = '\0';
	print ( text );
	*id = save;

	printf ( "</A>" );

	print ( id_end );
}

// Output string with HTML escaping and certain internal metacharacter handling.

void print ( char *str ) {
	// Check for @id (link-to) and \id (link-target) specifiers.
	char *to = strchr ( str, '@' );
	char *target = strchr ( str, '\\' );

	if ( to && ! ( ( to[1] >= 'a' && to[1] <= 'z' ) || to[1] == '@' ) )
		to = 0;

	if ( target && ! ( ( target[1] >= 'a' && target[1] <= 'z' ) || target[1] == '@' ) )
		target = 0;

	if ( to && target && target < to )
		to = 0;

	if ( to && target && to < target )
		target = 0;

	if ( to ) {
		hlink ( str, '@' );
		return;
	}
	else if ( target ) {
		hlink ( str, '\\' );
		return;
	}

	while ( *str ) {
		// Two or more ## mean N-1 non-breaking spaces.
		if ( str[0] == '#' && str[1] == '#' ) {
			int n = 0;

			for ( ; *str == '#'; str++ )
				n++;

			while ( n-- > 1 )
				printf ( "&nbsp;" );

			continue;
		}

		if ( *str == '_' ) {
			if ( str[1] == '_' ) {
				printf ( "_" );
				str++;
			}
			else {
				printf ( "<%sTT>", tt ? "/" : "" );
				tt = !tt;
			}
		}
		else if ( *str == '<' )
			printf ( "&lt;" );
		else if ( *str == '>' )
			printf ( "&gt;" );
		else if ( *str == '&' )
			printf ( "&amp;" );
		else
			printf ( "%c", *str );

		str++;
	}
}

void tprint ( char *str ) {
	// Check for @id (link-to) and \id (link-target) specifiers.
	char *to = strchr ( str, '@' );
	char *target = strchr ( str, '\\' );

	if ( to && ! ( ( to[1] >= 'a' && to[1] <= 'z' ) || to[1] == '@' ) )
		to = 0;

	if ( target && ! ( ( target[1] >= 'a' && target[1] <= 'z' ) || target[1] == '@' ) )
		target = 0;

	if ( to && target && target < to )
		to = 0;

	if ( to && target && to < target )
		target = 0;

	if ( to ) {
		tlink ( str, '@' );
		return;
	}
	else if ( target ) {
		tlink ( str, '\\' );
		return;
	}

	while ( *str ) {
		// Two or more ## mean N-1 non-breaking spaces.
		if ( str[0] == '#' && str[1] == '#' ) {
			int n = 0;

			for ( ; *str == '#'; str++ )
				n++;

			while ( n-- > 1 )
				printf ( " " );

			continue;
		}

		if ( *str == '_' ) {
			if ( str[1] == '_' ) {
				printf ( "_" );
				str++;
			}
			else {
				if (f_color) {
					if (!tt) {
						printf(c_pfx);
						printf(c_strong);
					}
					else
						printf(c_rst);
				}
				tt = !tt;
			}
		}
		else
			printf ( "%c", *str );

		str++;
	}
}

void tlink ( char *str, char how ) {
	char *id = strchr ( str, how );
	char *text = id, save, *id_end;
	int len = 0, spccnt;

	while ( id[len] == how )
		len++;

	for ( id_end = id + len; *id_end >= 'a' && *id_end <= 'z'; id_end++ )
		;

	spccnt = len;

	do {
		if ( text > str )
			text--;

		while ( text > str && *text != ' ' )
			text--;

		spccnt--;
	}
	while ( spccnt > 0 );

	if ( *text == ' ' )
		text++;

	save = *text;
	*text = '\0';
	printf ( str );
	*text = save;

	save = *id_end;
	*id_end = '\0';
// 	printf ( "<A " );
//
// 	if ( how == '@' )
// 		printf ( "HREF=\"#" );
// 	else
// 		printf ( "NAME=\"" );
//
// 	printf ( "%s\">", id + len );
	*id_end = save;

	save = *id;
	*id = '\0';
// 	print ( text );
	if ( how == '@' && f_color) {
		printf(c_pfx);
		printf(c_link);
	}
	else if (f_color) {
		printf(c_pfx);
		printf(c_name);
	}

	printf ( text );

	if (f_color) {
		printf(c_rst);
	}

	*id = save;

// 	printf ( "</A>" );

	printf ( id_end );
}
