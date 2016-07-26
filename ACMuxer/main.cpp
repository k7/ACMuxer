
//#include "InputVideo.h"
#include <iostream>
//#include "VideoFrameRGB.h"
//#include "Picture.h"
#include "VideoStream.h"
#include "FileIn.h"
#include <cassert>
using namespace std;

//#define INBUF_SIZE 4096

int main() {
	//avformat_free_context(nullptr);
	FileIn file("c:/Coding/video.flv");
	file.displayFileInfo();
	//cout << "getStreamCount: " << static_cast<int>(file.getVideoStreamIndices().size()) << endl;;
	VideoStream vs = file.getVideoStream(0);

	vector<Frame> v;
	for (int i = 0;; ++i) {
		auto f = vs.getNextFrame();
		if (!f) break;
		//av_opt_ptr
		v.push_back(std::move(f));
		//cout << i << endl;
		//cout << f.get()->pts << endl;
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