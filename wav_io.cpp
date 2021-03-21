#include "wav_io.hpp"
#include <stdint.h>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace wav_io {

static const uint16_t kFormatNone      = (uint16_t)0;
static const uint16_t kFormatPcm       = (uint16_t)1;
static const uint16_t kFormatAdPcm     = (uint16_t)2;
static const uint16_t kFormatPcmFloat  = (uint16_t)3;
static const uint16_t kFormatALaw      = (uint16_t)6;
static const uint16_t kFormatMuLaw     = (uint16_t)7;
static const uint16_t kFormatExtension = (uint16_t)65534;

static const GUID     kSubFormatPcm      ("0x0100000000001000800000aa00389b71");
static const GUID     kSubFormatAdPcm    ("0x0200000000001000800000aa00389b71");
static const GUID     kSubFormatPcmFloat ("0x0300000000001000800000aa00389b71");
static const GUID     kSubFormatALaw     ("0x0600000000001000800000aa00389b71");
static const GUID     kSubFormatMuLaw    ("0x0700000000001000800000aa00389b71");
static const GUID     kSubFormatMpeg     ("0x5000000000001000800000aa00389b71");

static const int16_t MuLawDecodeTable[256] =
{
    -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956,
    -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764,
    -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412,
    -11900, -11388, -10876, -10364,  -9852,  -9340,  -8828,  -8316,
     -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,
     -5884,  -5628,  -5372,  -5116,  -4860,  -4604,  -4348,  -4092,
     -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
     -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,
     -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
     -1372,  -1308,  -1244,  -1180,  -1116,  -1052,   -988,   -924,
      -876,   -844,   -812,   -780,   -748,   -716,   -684,   -652,
      -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,
      -372,   -356,   -340,   -324,   -308,   -292,   -276,   -260,
      -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
      -120,   -112,   -104,    -96,    -88,    -80,    -72,    -64,
       -56,    -48,    -40,    -32,    -24,    -16,     -8,      0,
     32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,
     23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
     15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,
     11900,  11388,  10876,  10364,   9852,   9340,   8828,   8316,
      7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
      5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,
      3900,   3772,   3644,   3516,   3388,   3260,   3132,   3004,
      2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,
      1884,   1820,   1756,   1692,   1628,   1564,   1500,   1436,
      1372,   1308,   1244,   1180,   1116,   1052,    988,    924,
       876,    844,    812,    780,    748,    716,    684,    652,
       620,    588,    556,    524,    492,    460,    428,    396,
       372,    356,    340,    324,    308,    292,    276,    260,
       244,    228,    212,    196,    180,    164,    148,    132,
       120,    112,    104,     96,     88,     80,     72,     64,
	    56,     48,     40,     32,     24,     16,      8,      0
};

static const int16_t ALawDecodeTable[256] = 
{
    -5504, -5248, -6016, -5760, -4480, -4224,
    -4992, -4736, -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784,
    -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368, -3776, -3648,
    -4032, -3904, -3264, -3136, -3520, -3392, -22016, -20992, -24064,
    -23040, -17920, -16896, -19968, -18944, -30208, -29184, -32256,
    -31232, -26112, -25088, -28160, -27136, -11008, -10496, -12032,
    -11520, -8960, -8448, -9984, -9472, -15104, -14592, -16128, -15616,
    -13056, -12544, -14080, -13568, -344, -328, -376, -360, -280, -264,
    -312, -296, -472, -456, -504, -488, -408, -392, -440, -424, -88, -72,
    -120, -104, -24, -8, -56, -40, -216, -200, -248, -232, -152, -136,
    -184, -168, -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184,
    -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696, -688, -656,
    -752, -720, -560, -528, -624, -592, -944, -912, -1008, -976, -816,
    -784, -880, -848, 5504, 5248, 6016, 5760, 4480, 4224, 4992, 4736,
    7552, 7296, 8064, 7808, 6528, 6272, 7040, 6784, 2752, 2624, 3008,
    2880, 2240, 2112, 2496, 2368, 3776, 3648, 4032, 3904, 3264, 3136,
    3520, 3392, 22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944,
    30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136, 11008, 10496,
    12032, 11520, 8960, 8448, 9984, 9472, 15104, 14592, 16128, 15616,
    13056, 12544, 14080, 13568, 344, 328, 376, 360, 280, 264, 312, 296,
    472, 456, 504, 488, 408, 392, 440, 424, 88, 72, 120, 104, 24, 8, 56,
    40, 216, 200, 248, 232, 152, 136, 184, 168, 1376, 1312, 1504, 1440,
    1120, 1056, 1248, 1184, 1888, 1824, 2016, 1952, 1632, 1568, 1760,
    1696, 688, 656, 752, 720, 560, 528, 624, 592, 944, 912, 1008, 976,
    816, 784, 880, 848
};

GUID::GUID(std::string hexString)
{
    // assert
    // log::assertion(hexString.length() == 34
    //               && hexString[0] == '0'
    //               && (hexString[1] == 'x' || hexString[1] == 'X'),
    //               "[wav] wrong GUID string {}", hexString);
    for (size_t i = 2; i < hexString.length(); i+=2)
    {
        auto sub = hexString.substr(i, 2);
        guid[i/2-1] = (char)std::stoi(sub, nullptr, 16);
    }
    // log::assertion(hexString == HexString(), "fuck you!");
}

std::string GUID::HexString() const
{
    std::stringstream ss;
    ss << "0x";
    for (int i = 0; i < 16; ++i)
    {
        ss << std::setfill('0') << std::setw(2)
           << std::hex << (guid[i] & 0xff);
    }
    std::string ret;
    ss >> ret;
    return ret;
}

bool GUID::operator==(const GUID &b) const
{
    for (int i = 0; i < 16; ++i)
        if (guid[i] != b.guid[i]) return false;
    return true;
}

uint32_t    _GetId(const char id[4]) { return *(uint32_t*)(id); }
std::string _GetIdString(const char *id, size_t size)
{
    char *buf = new char[size+1];
    memcpy(buf, id, size);
    buf[size] = 0;
    std::string ret(buf);
    delete[] buf;
    return ret;
}

bool _ReadSamplePcm(const FormatChunk &formatChunk, std::ifstream &fin, int16_t &value)
{
    // write data according to bits
    switch (formatChunk.bitsPerSample)
    {
    case 8: {
        uint8_t val;
        fin.read((char *)&val, 1);
        value = ((int16_t)val - (int16_t)128) << 8;
        return true;
    }
    case 16: {
        int16_t val;
        fin.read((char *)&val, 2);
        value = val;
        return true;
    }
    case 32: {
        int32_t val;
        fin.read((char *)&val, 4);
        value = (int16_t)(val >> 16);
        return true;
    }
    default:
        // log::fatal("[wav] unsupport bits {:d} for PCM Integer",
        //            formatChunk.bitsPerSample);
        fin.close();
        return false;
    }
}

bool _ReadSamplePcmFloat(const FormatChunk &formatChunk, std::ifstream &fin, int16_t &value)
{
    // write data according to bits
    switch (formatChunk.bitsPerSample)
    {
    case 32: {
        float val;
        fin.read((char *)&val, sizeof(float));
        value = (int16_t)(std::round((val + 1.f) * 32767.5f) - 32768.f);
        return true;
    }
    case 64: {
        double val;
        fin.read((char *)&val, sizeof(double));
        value = (int16_t)(std::round((val + 1.0) * 32767.5) - 32768.0);
        return true;
    }
    default:
        // log::fatal("[wav] unsupport bits {:d} for PCM Float.",
        //              formatChunk.bitsPerSample);
        fin.close();
        return false;
    }
}

bool _ReadSampleALaw(const FormatChunk &formatChunk, std::ifstream &fin, int16_t &value)
{
    if (formatChunk.bitsPerSample == 8)
    {
        uint8_t val;
        fin.read((char *)&val, 1);
        value = ALawDecodeTable[val];
        return true;
    }
    else
    {
        // log::fatal("[wav] unsupport bits {:d} for ALaw.",
        //              formatChunk.bitsPerSample);
        fin.close();
        return false;
    }
}

bool _ReadSampleMuLaw(const FormatChunk &formatChunk, std::ifstream &fin, int16_t &value)
{
    static double invQuan = 1.0 / 255.0;
    if (formatChunk.bitsPerSample == 8)
    {
        uint8_t val;
        fin.read((char *)&val, 1);
        value = MuLawDecodeTable[val];
        return true;
    }
    else
    {
        // log::fatal("[wav] unsupport bits {:d} for MuLaw.",
        //              formatChunk.bitsPerSample);
        fin.close();
        return false;
    }
}

void _DumpFormat(const FileHeader &fileHeader, const FactChunk &factChunk, const FormatChunk &formatChunk)
{
    // log::info(
    //     "WAV file\n"
    //     "  RIFF Chunk Id:            {}\n"
    //     "  RIFF Chunk Body Size:     {}\n"
    //     "  Format Code:              {}\n"
    //     "  - Fact Chunk Id:          {}\n"
    //     "  - Fact Chunk Body Size:   {}\n"
    //     "    - Num sample frames:    {}\n"
    //     "  - Format Chunk Id:        {}\n"
    //     "  - Format Chunk Body Size: {}\n"
    //     "    - Audio format:         {}\n"
    //     "    - Num channels:         {}\n"
    //     "    - Sample rate:          {}\n"
    //     "    - Bytes per second:     {}\n"
    //     "    - Bytes per frame:      {}\n"
    //     "    - Bits per sample:      {}\n"
    //     "    - Extension size:       {}\n"
    //     "    - Valid bps:            {}\n"
    //     "    - Channel mask:         {}\n"
    //     "    - GUID:                 {}\n",

    //     // file header
    //     _GetIdString(fileHeader.chunkId, 4),
    //     fileHeader.chunkSize,
    //     _GetIdString(fileHeader.format, 4),

    //     // fact chunk
    //     _GetIdString(factChunk.header.chunkId, 4),
    //     factChunk.header.chunkSize,
    //     factChunk.sampleFrames,

    //     // fmt chunk
    //     _GetIdString(formatChunk.header.chunkId, 4),
    //     formatChunk.header.chunkSize,
    //     formatChunk.audioFormat,
    //     formatChunk.numChannels,
    //     formatChunk.sampleRate,
    //     formatChunk.byteRate,
    //     formatChunk.blockAlign,
    //     formatChunk.bitsPerSample,
    //     formatChunk.extensionSize,
    //     formatChunk.validBitsPerSample,
    //     formatChunk.channelMask,
    //     formatChunk.guid.HexString()
    // );
}


bool save(std::string _filename, int _sample_rate, int _n_total_samples, int _n_channels, const int16_t * _data)
{
    wav_io::FileHeader fileHeader;
    *(uint32_t*)fileHeader.chunkId = wav_io::kIdRIFF;
    *(uint32_t*)fileHeader.format  = wav_io::kIdWAVE;
    fileHeader.chunkSize = (uint32_t)(
        sizeof(wav_io::FileHeader) + sizeof(wav_io::FormatChunk) +
        _n_total_samples * 2 * _n_channels
    );

    wav_io::FormatChunk formatChunk;
    *(uint32_t*)formatChunk.header.chunkId = wav_io::kIdfmt;
    formatChunk.header.chunkSize = 16;
    formatChunk.audioFormat = 1;
    formatChunk.numChannels = (uint16_t)_n_channels;
    formatChunk.sampleRate = _sample_rate;
    formatChunk.byteRate = (uint32_t)(_sample_rate * 2 * _n_channels);
    formatChunk.blockAlign = 16 * _n_channels / 8;
    formatChunk.bitsPerSample = 16;

    // data chunk
    wav_io::ChunkHeader chunk;
    *(uint32_t*)chunk.chunkId = wav_io::kIddata;
    chunk.chunkSize = (uint32_t)(_n_total_samples * 2 * _n_channels);

    std::ofstream fout(_filename, std::ios::binary);
    if (fout.is_open())
    {
        fout.write((char *)(&fileHeader),  sizeof(wav_io::FileHeader));
        fout.write((char *)(&formatChunk), 24);
        fout.write((char *)(&chunk),       sizeof(wav_io::ChunkHeader));
        for (int i = 0; i < _n_total_samples * _n_channels; ++i) {
            fout.write((char*)(_data+i), sizeof(int16_t));
        }
        fout.close();
        return true;
    }
    else
    {
        return false;
    }
}

}
