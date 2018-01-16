#ifndef h_oscompat_h
#define h_oscompat_h

#include <ctype.h>
#include "endianutils.h"
#include <PACEInterfaceLib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
#  define _Palm_va_list char *
// Fixed size data types
typedef signed char		Int8;
typedef signed short		Int16;	
typedef signed long		Int32;

typedef unsigned char	UInt8;
typedef unsigned short  UInt16;
typedef unsigned long   UInt32;

// Logical data types
typedef unsigned char	Boolean;

typedef char				Char;
typedef UInt16				WChar;		// 'wide' int'l character type.

typedef UInt16				Err;

typedef UInt32				LocalID;		// local (card relative) chunk ID

typedef Int16 				Coord;		// screen/window coordinate


typedef void *				MemPtr;		// global pointer
typedef struct _opaque *MemHandle;	// global handle

typedef void * FileRef;
*/
void *MemGluePtrNewL ( UInt32 foo );

extern int errno;

#define VFSPATH "/PALM/Programs/t1lib"

#define alloca(a) MemPtrNew(a)
//#define malloc(a) MemPtrNew(a)
void *malloc ( unsigned long s );
//#define free(a) MemPtrFree(a)
void free ( void *p );
//#define realloc(p,s) MemPtrResize(p,s)
void *realloc ( unsigned char *p, UInt32 s );
//void *memcpy ( void *d, const void *s, unsigned int size );
void *calloc ( unsigned long s, unsigned long n );
void *memset(void *s, int c, unsigned long n);
void *memcpy(void *t, const void *f, unsigned long s);
//void *memset ( void *b, int c, unsigned int len );
#define memmove(t,f,s) MemMove(t,f,s)
#define memcmp(a,b,s) MemCmp(a,b,s)
//#define htonl(a) a
//#define ntohl(a) a
#define puts(a) menu_info(a)
//#define printf Sys_PrintF


extern Int16 StrPrintF( Char* s, const Char* formatStr, ... );
extern Int16 StrVPrintF( Char* s, const Char* formatStr, _Palm_va_list arg );


void ClearScreen(void);

void Sys_PrintF( char *s, ...);
void Sys_PrintF2( char *s, ...);

int sprintf(char *text, const char *s, ...);

Char *strrchr(Char *string, Char chr);
Char *strdup(Char *string);
Char *strtok(Char *string, Char *token);
int strcspn(const Char *str1, const Char *str2);
void XBell(int *, int);
void *XtMalloc(int size);
void XtFree(void *ptr);
void qsort(void *list, int count, int size, int (*sortfunc)(const void *, const void *));
int scanf(const char *format, ...);
int sscanf(char *buff, const char *format, ...);

//__ctype_mapC
//__lower_mapC
//__upper_mapC

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

void abort ( void );
int abs ( int x );

void debuglog ( char *poo );


#ifndef int8
typedef  signed char    int8;
#endif
#ifndef int16
typedef  signed short   int16;
#endif
#ifndef int32
typedef  signed long     int32;
#endif
#ifndef uint8
typedef  unsigned char  uint8;
#endif
#ifndef uint16
typedef  unsigned short uint16;
#endif
#ifndef uint32
typedef  unsigned long   uint32;
#endif

#ifdef __cplusplus
  }
#endif


#endif
