#pragma once
#include <ffms.h>
#include <videosource.h>
#include <audiosource.h>
#include <string>
#include <vector>

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
    std::vector<std::pair<int, FFMS_VideoSource *>> video_sources_;
    std::vector<std::pair<int, FFMS_AudioSource *>> audio_sources_;

    void _handle_error();
    bool _open_video_source(std::string _fpath, int _trackno, FFMS_Index * _index);
    bool _open_audio_source(std::string _fpath, int _trackno, FFMS_Index * _index);
    void _close_video_sources();
    void _close_audio_sources();

public:
    VideoReader();
    virtual ~VideoReader();

    bool open(std::string _fpath);
    void close();
    bool is_open() const { return is_open_; }
    const FFMS_Frame * get_frame(int _number);
};
