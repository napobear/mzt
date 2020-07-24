#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ld80.h"

static struct zsymbol *zsymbol_table;
static int next_zsymbol, zsymbol_table_size;

struct zsymbol *find_zsymbol ( char *name ) {
	struct zsymbol *sym;
	ENTRY e, *ep;

	sym = get_zsymbol ( name );

	if ( sym ) return sym;

	if ( next_zsymbol >= zsymbol_table_size ) die ( E_RESOURCE,
		                "ld80: Symbol table exceeds %d entries. "
		                "Use -S option.\n", zsymbol_table_size );

	sym = zsymbol_table + next_zsymbol++;
	strcpy ( sym->name, name );
	sym->value = UNDEFINED;

	e.key = sym->name;
	e.data = ( char * ) sym;
	ep = hsearch ( e, ENTER );

	if ( ep == NULL ) die ( E_RESOURCE, "ld80: Not enough memory\n" );

	return sym;
}

int add_zsymbol ( char *name, int offset, struct section *section ) {
	struct zsymbol *sym;

	sym = find_zsymbol ( name );

	if ( sym->at.section ) {
		fprintf ( stderr, "ld80: Symbol %s is redefined\n", name );
		fatalerror = 1;
	}

	sym->at.offset = offset;
	sym->at.section = section;
	sym->value = 0;

	return 1;
}

struct zsymbol *get_zsymbol ( char *name ) {
	ENTRY e, *ep;

	e.key = name;
	ep = hsearch ( e, FIND );
	return ( struct zsymbol * ) ( ep ? ep->data : NULL );
}

void set_zsymbols ( void ) {
	int i;
	struct zsymbol *s;

	for ( i = 0; i < zsymbol_table_size; i++ ) {
		s = zsymbol_table + i;

		if ( s->at.section == NULL ) break;

		s->value = s->at.section->base + s->at.offset;
	}
}

int init_zsymbol ( int n ) {
	zsymbol_table = calloc ( n, sizeof ( struct zsymbol ) );

	if ( zsymbol_table == NULL || hcreate ( n ) == 0 )
		die ( E_RESOURCE, "ld80: Not enough memory for %d zsymbols\n", n );

	next_zsymbol = 0;
	zsymbol_table_size = n;
	return n;
}

void clear_zsymbol ( void ) {
	free ( zsymbol_table );
	hdestroy();
}

#ifdef	DEBUG
void dump_zsymbols ( void ) {
	int i;

	for ( i = 0; i < next_zsymbol; i++ ) {
		printf ( "name=%-8s section=%p offset=%.4hx value=%.4hx\n",
		         zsymbol_table[i].name, zsymbol_table[i].at.section,
		         zsymbol_table[i].at.offset, zsymbol_table[i].value );
	}
}
#endif

#define	SYM(x)	(*((struct zsymbol **)(x)))

static
int by_value ( const void *a, const void *b ) {
	return SYM ( a )->value - SYM ( b )->value;
}

static
int by_name ( const void *a, const void *b ) {
	return strcmp ( SYM ( a )->name, SYM ( b )->name );
}

void print_zsymbol_table ( FILE *f ) {
	int i;
	struct zsymbol *s, **slist, **sp;

	slist = calloc_or_die ( next_zsymbol, sizeof ( *slist ) );

	for ( i = 0; i < next_zsymbol; i++ ) slist[i] = zsymbol_table + i;

	qsort ( ( void* ) slist, next_zsymbol, sizeof ( *slist ), by_name );

	fprintf ( f,
	          "Symbol   Value Module   File\n"
	          "======================================================================\n" );

	for ( i = 0, sp = slist; i < next_zsymbol; i++, sp++ ) {
		s = *sp;

		if ( s->at.section ) fprintf ( f, "%-8s %.4hx  %-8s %s\n",
			                               s->name, s->value, s->at.section->module_name,
			                               s->at.section->filename );
		else fprintf ( f, "%-8s *** UNDEFINED ***\n", s->name );
	}
	
}
