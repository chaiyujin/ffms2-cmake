#pragma once
#include <ffms.h>
#include <videosource.h>
#include <audiosource.h>
#include <string>
#include <vector>

struct VideoTrack {
    int track_no;
    FFMS_VideoSource * source;
    const FFMS_VideoProperties * props;
};

struct AudioTrack {
    int track_no;
    FFMS_AudioSource * source;
    const FFMS_AudioProperties * props;
};

class VideoReader
{
private:

    static bool s_is_ffms_inited;
    static bool _Initialize();

    // error message handling
    char               errmsg_[1024];
    FFMS_ErrorInfo     errinfo_;
    // flag
    bool               is_open_;
    // video source
    std::vector<VideoTrack> video_tracks_;
    std::vector<AudioTrack> audio_tracks_;

    void _handle_error();
    bool _open_video_track(std::string _fpath, int _trackno, FFMS_Index * _index);
    bool _open_audio_track(std::string _fpath, int _trackno, FFMS_Index * _index);
    void _close_video_tracks();
    void _close_audio_tracks();

public:
    VideoReader();
    virtual ~VideoReader();

    bool open(std::string _fpath);
    void close();
    bool is_open() const { return is_open_; }
    const FFMS_Frame * get_frame(int _number, int _i_video_track=0);
    std::unique_ptr<std::vector<uint8_t>> get_audio(int64_t _start, int64_t _n_samples=2048, int _i_audio_track=0);

    VideoTrack & video_track(size_t _i) { return video_tracks_[_i]; }
    AudioTrack & audio_track(size_t _i) { return audio_tracks_[_i]; }
};
