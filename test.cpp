#include <ffms.h>
#include <stdlib.h>
#include <stdio.h>

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
  /* Initialize the library. */
  FFMS_Init(0, 0);

  /* Index the source file. Note that this example does not index any audio tracks. */
  char errmsg[1024];
  FFMS_ErrorInfo errinfo;
  errinfo.Buffer      = errmsg;
  errinfo.BufferSize  = sizeof(errmsg);
  errinfo.ErrorType   = FFMS_ERROR_SUCCESS;
  errinfo.SubType     = FFMS_ERROR_SUCCESS;
  const char *sourcefile = "../assets/test.mp4";

  FFMS_Indexer *indexer = FFMS_CreateIndexer(sourcefile, &errinfo);
  if (indexer == NULL) {
    /* handle error (print errinfo.Buffer somewhere) */
    printf("%s\n", errinfo.Buffer);
    exit(1);
  }

  FFMS_Index *index = FFMS_DoIndexing2(indexer, FFMS_IEH_ABORT, &errinfo);
  if (index == NULL) {
    /* handle error (you should know what to do by now) */
    printf("%s\n", errinfo.Buffer);
    exit(1);
  } 

  /* Retrieve the track number of the first video track */
  int trackno = FFMS_GetFirstTrackOfType(index, FFMS_TYPE_VIDEO, &errinfo);
  if (trackno < 0) {
    /* no video tracks found in the file, this is bad and you should handle it */
    /* (print the errmsg somewhere) */
    printf("%s\n", errinfo.Buffer);
    exit(1);
  }

  /* We now have enough information to create the video source object */
  FFMS_VideoSource *videosource = FFMS_CreateVideoSource(sourcefile, trackno, index, 1, FFMS_SEEK_NORMAL, &errinfo);
  if (videosource == NULL) {
    /* handle error */
    printf("%s\n", errinfo.Buffer);
    exit(1);
  }

  /* Since the index is copied into the video source object upon its creation,
  we can and should now destroy the index object. */
  FFMS_DestroyIndex(index);

  /* Retrieve video properties so we know what we're getting.
  As the lack of the errmsg parameter indicates, this function cannot fail. */
  const FFMS_VideoProperties *videoprops = FFMS_GetVideoProperties(videosource);

  /* Now you may want to do something with the info, like check how many frames the video has */
  int num_frames = videoprops->NumFrames;

  /* Get the first frame for examination so we know what we're getting. This is required
  because resolution and colorspace is a per frame property and NOT global for the video. */
  const FFMS_Frame *propframe = FFMS_GetFrame(videosource, 0, &errinfo);

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

  if (FFMS_SetOutputFormatV2(videosource, pixfmts, propframe->EncodedWidth, propframe->EncodedHeight,
                             FFMS_RESIZER_BICUBIC, &errinfo)) {
    /* handle error */
    printf("%s\n", errinfo.Buffer);
    exit(1);
  }

  /* now we're ready to actually retrieve the video frames */
  int framenumber = 100; /* valid until next call to FFMS_GetFrame* on the same video object */
  const FFMS_Frame *curframe = FFMS_GetFrame(videosource, framenumber, &errinfo);
  if (curframe == NULL) {
    /* handle error */
    printf("%s\n", errinfo.Buffer);
    exit(1);
  }
  /* do something with curframe */
  /* continue doing this until you're bored, or something */
  printf("%d %d, %d %d, %d %d %d %d\n",
    curframe->EncodedWidth, curframe->EncodedHeight,
    curframe->ScaledWidth, curframe->ScaledHeight,
    curframe->Linesize[0],
    curframe->Linesize[1],
    curframe->Linesize[2],
    curframe->Linesize[3]
  );

  write_ppm(curframe->Data[0], curframe->ScaledWidth, curframe->ScaledHeight, curframe->Linesize[0]);

  /* now it's time to clean up */
  FFMS_DestroyVideoSource(videosource);

  return 0;
}
