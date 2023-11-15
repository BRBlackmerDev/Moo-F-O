
struct FILE_DATA {
    uint8 * Contents;
    uint32  Size;
};

typedef FILE_DATA ( READ_FILE )( MEMORY * Memory, char * SaveDir, char * FileName, char * extension );
typedef boo32     ( WRITE_FILE )( char * SaveDir, char * FileName, char * extension, void * data, uint32 size );
typedef boo32     ( DOES_FILE_EXIST )( char * SaveDir, char * FileName, char * extension );
typedef boo32     ( DELETE_FILE )( char * SaveDir, char * FileName, char * extension );