#pragma once
#include <memory>
#include "ffmpeg.h"
#include "Frame.h"
#include <string>

using namespace std;
class FileOut {
public:
   FileOut(string filename, int width, int height);
   FileOut(string filename, int width, int height, AVCodecID avCodecID_video);
   void encodeFrameTest() const;
   void encodeFrame(Frame& frame, uint64_t pts);
   void encodeFinish(uint64_t);
   unique_ptr<AVCodecContext, AVCodecContextDeleter> ccv;
   AVPacket _pkt;
   unique_ptr<AVPacket, AVPacketDeleter> pkt; // Wrap _pkt to ensures last unref on destruction
private:

   int frameNo = 0;
   string filename;
   FILE *f;
};

