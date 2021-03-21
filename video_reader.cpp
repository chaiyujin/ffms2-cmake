#include "video_reader.hpp"

bool VideoReader::s_is_ffms_inited = false;

bool VideoReader::_Initialize()
{
    if (!s_is_ffms_inited) {
        FFMS_Init(0, 0);
    }
    s_is_ffms_inited = true;
    return s_is_ffms_inited;
}

VideoReader::VideoReader()
    : is_open_(false)
    , video_sources_(0)
    , audio_sources_(0)
{
    VideoReader::_Initialize();
    // init error info handler
    errinfo_.Buffer     = errmsg_;
    errinfo_.BufferSize = sizeof(errmsg_);
    errinfo_.ErrorType  = FFMS_ERROR_SUCCESS;
    errinfo_.SubType    = FFMS_ERROR_SUCCESS;
}
VideoReader::~VideoReader()
{
    this->close();
}

void VideoReader::_handle_error()
{
    printf("%s\n", errinfo_.Buffer);
    exit(1);
}

bool VideoReader::_open_video_source(std::string _fpath, int _trackno, FFMS_Index * _index)
{
    printf("Try to open video track: %d\n", _trackno);
    /* We now have enough information to create the video source object */
    auto * video_source = FFMS_CreateVideoSource(
        _fpath.c_str(), _trackno, _index, 1, FFMS_SEEK_NORMAL, &errinfo_
    );
    if (video_source == nullptr) { this->_handle_error(); return false;}

    /* Get the first frame for examination so we know what we're getting. This is required
    because resolution and colorspace is a per frame property and NOT global for the video. */
    const FFMS_Frame *propframe = FFMS_GetFrame(video_source, 0, &errinfo_);
    if (propframe == nullptr) {
        FFMS_DestroyVideoSource(video_source);
        video_source = nullptr;
        this->_handle_error();
        return false;
    }

    /* Now you may want to do something with the info; particularly interesting values are:
    propframe->EncodedWidth; (frame width in pixels)
    propframe->EncodedHeight; (frame height in pixels)
    propframe->EncodedPixelFormat; (actual frame colorspace)
    */

    /* If you want to change the output colorspace or resize the output frame size,
    now is the time to do it. IMPORTANT: This step is also required to prevent
    resolution and colorspace changes midstream. You can you can always tell a frame's
    original properties by examining the Encoded* properties in FFMS_Frame. */
    /* See libavutil/pixfmt.h for the list of pixel formats/colorspaces.
    To get the name of a given pixel format, strip the leading PIX_FMT_
    and convert to lowercase. For example, PIX_FMT_YUV420P becomes "yuv420p". */

    /* A -1 terminated list of the acceptable output formats. */
    int pixfmts[2];
    pixfmts[0] = FFMS_GetPixFmt("rgba");
    pixfmts[1] = -1;

    if (FFMS_SetOutputFormatV2(video_source, pixfmts,
                               propframe->EncodedWidth,
                               propframe->EncodedHeight,
                               FFMS_RESIZER_BICUBIC, &errinfo_))
    {
        FFMS_DestroyVideoSource(video_source);
        video_source = nullptr;
        this->_handle_error();
        return false;
    }

    video_sources_.push_back({_trackno, video_source});
    return true;
}
bool VideoReader::_open_audio_source(std::string _fpath, int _trackno, FFMS_Index * _index)
{
    printf("Try to open audio track: %d\n", _trackno);
    /* We now have enough information to create the video source object */
    auto * audio_source = FFMS_CreateAudioSource(
        _fpath.c_str(), _trackno, _index, FFMS_DELAY_NO_SHIFT, &errinfo_
    );
    if (audio_source == nullptr) { this->_handle_error(); return false;}

    audio_sources_.push_back({_trackno, audio_source});
    return true;
}

void VideoReader::_close_video_sources()
{
    for (auto & p : video_sources_) {
        if (p.second) FFMS_DestroyVideoSource(p.second);
    }
    video_sources_.clear();
}
void VideoReader::_close_audio_sources()
{
    for (auto & p : audio_sources_) {
        if (p.second) FFMS_DestroyAudioSource(p.second);
    }
    audio_sources_.clear();
}

bool VideoReader::open(std::string _fpath)
{
    // close existing file first
    this->close();

    // create an empty indexer
    FFMS_Indexer *indexer = FFMS_CreateIndexer(_fpath.c_str(), &errinfo_);
    if (indexer == NULL) { this->_handle_error(); return false; }

    // do index
    FFMS_Index *index = FFMS_DoIndexing2(indexer, FFMS_IEH_ABORT, &errinfo_);
    if (index == NULL) { this->_handle_error(); return false; }

    int n_tracks = FFMS_GetNumTracks(index);
    printf("Toally %d tracks\n", n_tracks);

    bool success = true;
    int trackno = -1;

    /* Retrieve the track number of the first video track */
    trackno = FFMS_GetFirstTrackOfType(index, FFMS_TYPE_VIDEO, &errinfo_);
    if (trackno >= 0) {
        success &= _open_video_source(_fpath, trackno, index);
    }

    /* Retrieve the track number of the first audio track */
    trackno = FFMS_GetFirstTrackOfType(index, FFMS_TYPE_AUDIO, &errinfo_);
    if (trackno >= 0) {
        success &= _open_audio_source(_fpath, trackno, index);
    }

    /* Since the index is copied into the video source object upon its creation,
    we can and should now destroy the index object. */
    FFMS_DestroyIndex(index);
    is_open_ = true;

    if (success) {
        // now we're ready to actually retrieve the video frames
        return true;
    } else {
        this->close();
        return false;
    }
}

void VideoReader::close()
{
    // reset sources
    this->_close_video_sources();
    this->_close_audio_sources();
    // set as closed
    is_open_ = false;
}

const FFMS_Frame * VideoReader::get_frame(int _number)
{
    // if (video_source_ == nullptr) { return nullptr; }
    const FFMS_Frame *curframe = FFMS_GetFrame(video_sources_[0].second, _number, &errinfo_);
    if (curframe == nullptr) {
        this->_handle_error();
        return nullptr;
    }
    return curframe;
}
