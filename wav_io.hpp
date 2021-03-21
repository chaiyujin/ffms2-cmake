#pragma once
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdint.h>

namespace wav_io {

struct GUID
{
    char guid[16];
    GUID() { memset(guid, 0, 16); }
    GUID(std::string hexString);
    std::string HexString() const;
    bool operator==(const GUID &b) const;
};

struct FileHeader
{
    char        chunkId[4];
    uint32_t    chunkSize;
    char        format[4];
};
struct ChunkHeader
{
    char        chunkId[4];
    uint32_t    chunkSize;
};
struct FormatChunk
{
    ChunkHeader header;
    uint16_t    audioFormat;
    uint16_t    numChannels;
    uint32_t    sampleRate;
    uint32_t    byteRate;
    uint16_t    blockAlign;
    uint16_t    bitsPerSample;
    // possible
    uint16_t    extensionSize;
    uint16_t    validBitsPerSample;
    uint32_t    channelMask;
    GUID        guid;
};
struct FactChunk
{
    ChunkHeader header;
    uint32_t    sampleFrames;
};

static const uint32_t kIddata = 0x61746164;
static const uint32_t kIdfmt  = 0x20746D66;
static const uint32_t kIdRIFF = 0x46464952;
static const uint32_t kIdWAVE = 0x45564157;
static const uint32_t kIdfact = 0x74636166;

bool save(std::string _filename, int _sample_rate, int _n_total_samples, int _n_channels, const int16_t * _data);
}