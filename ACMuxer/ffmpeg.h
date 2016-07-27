#pragma once
#include <functional>

// why extern: http://stackoverflow.com/questions/67894/why-do-we-need-extern-c-include-foo-h-in-c
extern "C"
{
	// Requires ffmpeg folder added to VC++ include directory
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/opt.h>
	#include <libavutil/error.h>
	//#include <libavutil/timestamp.h>
}

// This replaces Additional Dependencies under Linker
// Requires ffmpeg\lib folder to VC++ Libraries directory
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
//#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
//#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")

// DLL files location is specified in Project properties...Debugging...Environment

#include <memory>
#include <iostream>

using namespace std;
struct AVCodecContextDeleter {
	void operator()(AVCodecContext* avCodecContext) const {
		cout << "AVCodecContextDeleter" << endl;
		avcodec_free_context(&avCodecContext);
	};
};
struct AVFormatContextDeleter {
	void operator()(AVFormatContext* avFormatContext) const {
		cout << "AVFormatContextDeleter" << endl;
		// Close an opened input AVFormatContext
		avformat_close_input(&avFormatContext);
	}
};
struct AVPacketDeleter {
	void operator()(AVPacket* avPacket) const {
		cout << "AVPacketDeleter" << endl;
		// Wipe the packet & unreferences the buffer referenced by the packet
		av_packet_unref(avPacket);
	}
};
struct AVFrameDeleter {
	void operator()(AVFrame* avFrame) const {
		cout << "AVFrameDeleter" << endl;
		av_frame_free(&avFrame);
	}
};
