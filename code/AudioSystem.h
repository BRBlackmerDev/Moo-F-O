
typedef HRESULT WINAPI DIRECT_SOUND_CREATE( LPCGUID lpcGuidDevice, LPDIRECTSOUND * ppDS, LPUNKNOWN lpUnkOuter );

#define AUDIO__SAMPLES_PER_SECOND  ( 48000 )
#define AUDIO__BYTES_PER_SAMPLE    ( 2 )
#define AUDIO__CHANNEL_COUNT       ( 2 )
#define AUDIO__BUFFER_SECONDS      ( 2 )
#define AUDIO__WRITE_FRAMES        ( 6 )

struct AUDIO_DATA {
    uint8 * Audio;
    int32  nSamples;
};

struct AUDIO_ENTRY {
    boo32    CanAdvance;
    AUDIO_ID AudioID;
    
    uint8 * Audio;
    int32  nSamples;
    flo32  AtSample;
    
    flo32 bVolume;
    flo32  Volume;
    flo32  Rate;
    boo32  IsLooping;
    
    flo32 TargetVolume;
    flo32 TargetVolume_dMax;
    flo32 TargetVolume_Delay;
    
    AUDIO_ID AudioIDForNextScript;
};

struct ASSET_SYSTEM;
#define AUDIO__MAX_ENTRY_COUNT  ( 256 )
struct AUDIO_SYSTEM {
    ASSET_SYSTEM * AssetSystem;
    
    flo32   MainVolume;
    
    int32      nAudio;
    AUDIO_ENTRY Audio[ AUDIO__MAX_ENTRY_COUNT ];
    
    uint8 * MixBuffer;
    int32   MixBuffer_Size;
    
    uint8 * DebugBuffer;
    int32   DebugBuffer_Size;
};