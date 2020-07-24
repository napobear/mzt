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
/* prototypes for mio.h */

extern FILE *mfopen(char *filename,char *mode);
extern int mfclose(FILE *f);
extern int mfputc(unsigned int c,FILE *f);
extern int mfgetc(FILE *f);
extern int mfseek(FILE *f,long loc,int origin);
extern int mfread(char *ptr, unsigned int size, unsigned int nitems, FILE *f);
extern int mfwrite(char *ptr, int size, int nitems, FILE *f);
