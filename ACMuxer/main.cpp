
//#include "InputVideo.h"
#include <iostream>
//#include "VideoFrameRGB.h"
//#include "Picture.h"
#include "VideoStream.h"
#include "FileIn.h"
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
      __int64 test = 0;
	FileIn file("c:/Coding/video.flv");
	file.displayFileInfo();
	//cout << "getStreamCount: " << static_cast<int>(file.getVideoStreamIndices().size()) << endl;;
	VideoStream vs = file.getVideoStream(0);
	for(int i = 0; i<10;++i) {
		auto frame = vs.getNextFrame();
		uint64_t pts = static_cast<uint64_t>(frame.avFrame.get()->pts);
		char ts[AV_TS_MAX_STRING_SIZE];
		char * p = &(ts[0]);
		//*p = ts[0];
		
		av_ts_make_time_string(p, frame.pkt.pts, &(vs.avCodecContext->time_base));
		//auto ts2 = av_q2d(vs.avCodecContext->time_base) * pts;
		cout << ts << endl;
	}
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