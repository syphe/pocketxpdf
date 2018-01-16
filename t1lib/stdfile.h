#ifndef h_stdfile_h
#define h_stdfile_h

#ifdef __cplusplus
extern "C" {
#endif



/* define if you want a alert to show each function as entered
 */
//#define DEBUGFILE 1
#define TAILMAX 8192

#define SF_LITTLEENDIAN 1 /* define if compiling in pnolet */

/* simulate stdio FILE using VFS; why not?
 */
typedef struct {
  char filename [ 128 ]; // for fun
  UInt32 size; // get file size (for read types)
  FileRef ref;
  UInt8 bufferp; // 1 for buffered input, 2 for output, 0 no buffering
  char buffer [ TAILMAX ];
  UInt16 tail;
} FILE;
//#define FILE FileRef

#define stderr ((FILE*) 37)
//#define stdin ((FILE*) 36)
#define stdout ((FILE*) 35)

#define SEEK_SET vfsOriginBeginning
#define SEEK_CUR vfsOriginCurrent
#define SEEK_END vfsOriginEnd

#define O_TEXT			0x0			/* 960827: Added this for Visual C++ compatibility. */
#define O_RDWR			0x1			/* open the file in read/write mode */	/*- mm 980420 -*/
#define O_RDONLY		0x2			/* open the file in read only mode */	/*- mm 980420 -*/
#define O_WRONLY		0x4			/* open the file in write only mode */	/*- mm 980420 -*/
#define O_APPEND		0x0100		/* open the file in append mode */
#define O_CREAT			0x0200		/* create the file if it doesn't exist */
#define O_EXCL			0x0400		/* if the file already exists don't create it again */
#define O_TRUNC			0x0800		/* truncate the file after opening it */
#define O_NRESOLVE		0x1000		/* Don't resolve any aliases */
#define O_ALIAS			0x2000		/* Open alias file (if the file is an alias) */
#define O_RSRC 			0x4000		/* Open the resource fork */
#define O_BINARY		0x8000		/* open the file in binary mode (default is text mode) */
#define F_DUPFD			0x0			/* return a duplicate file descriptor */

UInt16 palm_vfs_vol ( char *path );

int fchdir ( char *path ); // my hack; not related to std-c

FILE *fopen ( const char *filename, char *mode );
Int8 fclose ( FILE *file );
Int8 fseek ( FILE *file, UInt32 rel, UInt16 source );
UInt32 fread ( void *buf, UInt32 size, UInt32 num, FILE *file );
UInt32 fwrite ( const void *buf, UInt32 size, UInt32 num, FILE *file );
int fprintf ( FILE *file, char *fmt, ... );
int fgetc ( FILE *file );
int fputs ( const char *str, FILE *stream );
char *fgets(char *buf,int size,FILE *file);
int fputc ( UInt8 c, FILE *file );
UInt32 ftell ( FILE *file );
#define ferror(f) 0
#define feof(x) 0
long int fscanf_get_d ( FILE *f );
char fscanf_get_c ( FILE *f );
Int8 remove ( const char *filename );
UInt32 fgetsize(FILE *f);

/* special extensions
 */
#define STDTEMPFILE "StdTemp"

//MemHandle pdb_temp_open ( char *filename ); // copy to RAM and open
//void pdb_temp_close ( char *filename );     // back to VFS and delete temp

UInt8 file_exists ( char *filename );
UInt8 file_delete ( char *filename );
Int8 unlink( const char *filename );

Int8 fgetdate( const char *filename,UInt32 *date);

Int16 ARM_InitFileList(int *num_entries, char ***listchoice, char *lastrom, char *pExtension);
void ExitFileList(int num_entries, char **listchoice);

#ifdef __cplusplus
}
#endif


#endif
