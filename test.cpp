#include <ffms.h>
#include <stdlib.h>
#include <stdio.h>
#include "video_reader.hpp"
#include "wav_io.hpp"

void write_ppm(const uint8_t * _data, int _width, int _height, int _linesize) {
    int i, j;
    FILE *fp = fopen("frame.ppm", "wb"); /* b - binary mode */
    (void) fprintf(fp, "P6\n%d %d\n255\n", _width, _height);
    for (j = 0; j < _height; ++j) {
        for (i = 0; i < _width; ++i) {
            const uint8_t * color = &_data[j * _linesize + i * 4];
            (void) fwrite(color, 1, 3, fp);
        }
    }
    (void) fclose(fp);
}

int main (...) {
    VideoReader video_reader;
    video_reader.open("../assets/test.mp4");
    const FFMS_Frame *curframe = video_reader.get_frame(100);
    if (curframe == NULL) {
        exit(1);
    }
    write_ppm(curframe->Data[0], curframe->EncodedWidth, curframe->EncodedHeight, curframe->Linesize[0]);

    auto sr = video_reader.audio_track(0).props->SampleRate;
    auto ch = video_reader.audio_track(0).props->Channels;
    auto data = video_reader.get_audio(sr*2, sr*3, 0);
    wav_io::save(
        "test.wav", sr, data->size() / video_reader.audio_track(0).source->BytesPerSample, ch,
        (const int16_t *)data->data()
    );
    return 0;
}
