extern "C"
{
	//#include "dev\include\libavcodec\avcodec.h"
#include "ffmpeg\include\libavcodec\avcodec.h"
#include <libavformat/avformat.h>
#include <libswscale\swscale.h>
#include <libavutil\error.h>
	//#include <ffmpeg/swscale.h>
}

#pragma comment(lib, "ffmpeg\\lib\\avcodec.lib")
#pragma comment(lib, "ffmpeg\\lib\\avformat.lib")
#pragma comment(lib, "ffmpeg\\lib\\avformat.lib")
#pragma comment(lib, "ffmpeg\\lib\\avutil.lib")
#pragma comment(lib, "ffmpeg\\lib\\postproc.lib")
#pragma comment(lib, "ffmpeg\\lib\\swresample.lib")
#pragma comment(lib, "ffmpeg\\lib\\swscale.lib")
#pragma comment(lib, "ffmpeg\\lib\\avdevice.lib")
#pragma comment(lib, "ffmpeg\\lib\\avfilter.lib")