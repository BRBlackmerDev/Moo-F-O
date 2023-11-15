
#ifdef	STD_INCLUDE_DECL

struct MEMORY {
	void * Base;
	uint64 Size;
	uint64 Used;
    
#define MEMORY__SNAP_COUNT  ( 8 )
    uint32 snap_index;
    uint64 snapUsed[ MEMORY__SNAP_COUNT ];
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// Memory functions
//----------

internal MEMORY
Memory( void * base, uint64 size ) {
	MEMORY Result = { base, size };
	return Result;
}

internal void
ResetMemory( MEMORY * Memory ) {
	memset( Memory->Base, 0, Memory->Size );
	Memory->Used = 0;
}

#define _PushType( Memory, Type ) ( Type * )pushSize_( Memory, sizeof( Type ) )
#define _PushArray( Memory, Type, count ) ( Type * )pushSize_( Memory, sizeof( Type ) * ( count ) )
#define _PushSize( Memory, size ) pushSize_( Memory, size )

internal void *
pushSize_( MEMORY * Memory, uint64 size ) {
	Assert( Memory->Used + size <= Memory->Size );
	void * Result = ( uint8 * )Memory->Base + Memory->Used;
	Memory->Used += size;
	return Result;
}

#define _PushType_Clear( Memory, Type ) ( Type * )pushSize_Clear( Memory, sizeof( Type ) )
#define _PushArray_Clear( Memory, Type, count ) ( Type * )pushSize_Clear( Memory, sizeof( Type ) * ( count ) )
#define _PushSize_Clear( Memory, size ) pushSize_Clear( Memory, size )

internal void *
pushSize_Clear( MEMORY * Memory, uint64 size ) {
	void * Result = pushSize_( Memory, size );
	memset( Result, 0, size );
	return Result;
}

#define _PopType(  Memory, Type ) 			popSize_( Memory, sizeof( Type ) )
#define _PopArray( Memory, Type, count ) 	popSize_( Memory, sizeof( Type ) * ( count ) )
#define _PopSize(  Memory, size ) 			popSize_( Memory, size )

internal void
popSize_( MEMORY * Memory, uint64 size ) {
	Assert( Memory->Used - size >= 0 );
	Memory->Used -= size;
}

inline void
pushValue( MEMORY * Memory, uint8 value ) {
	Assert( Memory->Used + sizeof( uint8 ) <= Memory->Size );
	uint8 * slot = ( uint8 * )Memory->Base + Memory->Used;
	*slot = value;
	Memory->Used += sizeof( uint8 );
}

internal void *
copyBlock( MEMORY * Memory, void * data, uint64 size ) {
    void * Result = _PushSize( Memory, size );
    memcpy( Result, data, size );
    return Result;
}

inline uint8 *
getUsed( MEMORY * Memory ) {
    uint8 * Result = ( uint8 * )Memory->Base + Memory->Used;
    return Result;
}

internal MEMORY
SubArena( MEMORY * parentArena, uint64 size ) {
	MEMORY Result = {};
	Result.Base = _PushSize_Clear( parentArena, size );
	Result.Size = size;
	return Result;
}

internal MEMORY
SubArena( MEMORY * parentArena ) {
    MEMORY Result = SubArena( parentArena, parentArena->Size - parentArena->Used );
    return Result;
}

internal MEMORY *
SnapMemory( MEMORY * Memory ) {
    Assert( Memory->snap_index < MEMORY__SNAP_COUNT );
    Memory->snapUsed[ Memory->snap_index++ ] = Memory->Used;
    
    MEMORY * Result = Memory;
    return Result;
}

internal uint8 *
getSnapBase( MEMORY * Memory ) {
    Assert( Memory->snap_index > 0 );
    uint8 * Result = ( uint8 * )Memory->Base + Memory->snapUsed[ Memory->snap_index - 1 ];
    return Result;
}

internal uint32
getSnapUsed( MEMORY * Memory ) {
    Assert( Memory->snap_index > 0 );
    uint32 Result = ( uint32 )( Memory->Used - Memory->snapUsed[ Memory->snap_index - 1 ] );
    return Result;
}

internal void
popToSnap( MEMORY * Memory ) {
    Assert( Memory->snap_index > 0 );
    Memory->Used = Memory->snapUsed[ --Memory->snap_index ];
}

#endif	// STD_INCLUDE_FUNC