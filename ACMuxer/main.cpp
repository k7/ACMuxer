
//#include "InputVideo.h"
#include <iostream>
//#include "VideoFrameRGB.h"
//#include "Picture.h"
#include "VideoStream.h"
#include "FileIn.h"
#include "FileOut.h"
#include <cassert>
//#include <inttypes.h>
using namespace std;
#define AV_TS_MAX_STRING_SIZE 32

//#define INBUF_SIZE 4096
char *av_ts_make_time_string(char *buf, int64_t ts, AVRational *tb)
{
	if (ts == AV_NOPTS_VALUE) snprintf(buf, AV_TS_MAX_STRING_SIZE, "NOPTS");
	else                      snprintf(buf, AV_TS_MAX_STRING_SIZE, "%.6g", av_q2d(*tb) * ts);
	return buf;
}

int main() {
	

	FileIn file("c:/Coding/video.flv");
	file.displayFileInfo();
	//cout << "getStreamCount: " << static_cast<int>(file.getVideoStreamIndices().size()) << endl;;
	VideoStream vs = file.getVideoStream(0);
	

	FileOut encode("c:/Coding/out.mp4", 1376, 768);
	
	uint64_t lastpts = 0;
	uint64_t pts = 0;
	for(int i = 0; i<300;++i) {
		auto frame = vs.getNextFrame();
		if (!frame) break;
		lastpts = pts;
		pts = frame.getRelativePts();
		cout << "duration: " << pts - lastpts << endl;
		//cout << "PTS: " << pts << endl;
		//uint64_t pts = static_cast<uint64_t>(frame.avFrame.get()->pts);
		//char ts[AV_TS_MAX_STRING_SIZE];
		//char * p = &(ts[0]);
		
		//AVRational r { 1,1000 };
		//av_ts_make_time_string(p, frame.pkt.pts, &r);
		
		//auto ts2 = av_q2d(vs.avCodecContext->time_base) * pts;

		//int64_t bestpts = av_frame_get_best_effort_timestamp(frame.avFrame.get());
		//auto ctx = file.avFormatContext.get();
		
		//AVRational avTimeBaseQ{ 1, AV_TIME_BASE }; // microsecond (internal representation)
		//bestpts = av_rescale_q(bestpts, ctx->streams[0]->time_base, avTimeBaseQ);

		//cout << bestpts << endl;
		//cout << frame.pkt.pts << endl;
		//cout << frame.getRelativePts() << endl;

		
		encode.encodeFrame(frame,pts);
		
	}
	encode.encodeFinish(pts);
	return 0;
}
//int mainOLD(){
//	// Testavformat version
//	if (LIBAVFORMAT_VERSION_INT != avformat_version()) {
//		cout << "Mismatching dll/h versions" << endl;
//		return 0;
//	}
//	InputVideo input("c:/Coding/video.flv");
//	input.DisplayFileInfo();
//
//	//return 0;
//	
//	cout << "getStreamCount: " << input.getStreamCount() << endl;;
//	
//	
//	
//	//return 0;
//	input.openStream(0);
//	
//	for (int i = 0; i < 15; i++) {
//		auto f = input.getNextFrame();
//		Picture picture(f);
//		picture.SavePPM("c:/Coding/p" + to_string(i));
//	}
//	
//	return 0;
//}
