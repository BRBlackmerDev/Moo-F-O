
#pragma pack( push, 1 )
struct WAV_HEADER {
	uint32 ckID;
	uint32 ckSize;
	uint32 waveID;
};

struct BLOCK_HEADER {
	uint32 marker;
	uint32 size;
};

struct FMT_BLOCK {
	uint16 format;
	uint16 channels;
	uint32 sampleRate;
	uint32 avgDataRate;
	uint16 blockAlign;
	uint16 bitsPerSample;
	uint16 extensionSize;
};
#pragma pack( pop )

internal AUDIO_DATA
WAV_ReadFile( READ_FILE * ReadFile, MEMORY * PermMemory, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
	AUDIO_DATA Result = {};
	FILE_DATA File = ReadFile( TempMemory, SaveDir, FileName, "wav" );
	if( File.Contents ) {
		uint32 riffTag = *( ( uint32 * )&"RIFF" );
		uint32 waveTag = *( ( uint32 * )&"WAVE" );
		uint32  fmtTag = *( ( uint32 * )&"fmt " );
		uint32 dataTag = *( ( uint32 * )&"data" );
		
		uint8 * Start = ( uint8 * )File.Contents;
		uint8 * at = Start;
		
		WAV_HEADER * header = _addr( at, WAV_HEADER );
		Assert( header->ckID == riffTag );
		Assert( header->waveID == waveTag );
		
		BLOCK_HEADER * blockA = _addr( at, BLOCK_HEADER );
		Assert( blockA->marker == fmtTag );
		
		FMT_BLOCK * fmt = _addr( at, FMT_BLOCK );
		Assert( fmt->format        == 1 ); // PCM format
		Assert( fmt->channels      == 2 );
		Assert( fmt->sampleRate    == 48000 );
		Assert( fmt->bitsPerSample == 16 );
		
		BLOCK_HEADER * blockB = _addr( at, BLOCK_HEADER );
		Assert( blockB->marker == dataTag );
		
		uint8 * src = at;
		uint8 * dest = ( uint8 * )_PushSize( PermMemory, blockB->size );
		memcpy( dest, src, blockB->size );
		
		Assert( blockB->size % ( ( fmt->bitsPerSample / 8 ) * fmt->channels ) == 0 );
		uint32 samples = blockB->size / ( ( fmt->bitsPerSample / 8 ) * fmt->channels );
        Assert( ( samples % 100 ) == 0 );
        
#if 0
        int16 * Mod = ( int16 * )dest;
        int32 nSamplesToMod = ( AUDIO__SAMPLES_PER_SECOND / 60 ) * 4;
        int32 Sample_Start = 0;
        int32 Sample_End   = samples - 1;
        while( Mod[ Sample_Start * AUDIO__CHANNEL_COUNT ] == 0 ) {
            Sample_Start += 1;
        }
        while( Mod[ Sample_End   * AUDIO__CHANNEL_COUNT ] == 0 ) {
            Sample_End   -= 1;
        }
        for( int32 iSample = 0; iSample < nSamplesToMod; iSample++ ) {
            flo32 tSample = Clamp01( ( flo32 )iSample / nSamplesToMod );
            
            int16 * Samp0 = Mod + ( ( Sample_Start + iSample ) * AUDIO__CHANNEL_COUNT );
            Samp0[ 0 ] = ( int16 )( ( flo32 )Samp0[ 0 ] * tSample );
            Samp0[ 1 ] = ( int16 )( ( flo32 )Samp0[ 1 ] * tSample );
            
            int16 * Samp1 = Mod + ( ( Sample_End - iSample ) * AUDIO__CHANNEL_COUNT );
            Samp1[ 0 ] = ( int16 )( ( flo32 )Samp1[ 0 ] * tSample );
            Samp1[ 1 ] = ( int16 )( ( flo32 )Samp1[ 1 ] * tSample );
        }
#endif
		
        Result.Audio    = dest;
        Result.nSamples = samples;
        
        _PopSize( TempMemory, File.Size );
	}
	return Result;
}