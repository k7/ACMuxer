
//#include "InputVideo.h"
#include <iostream>
//#include "VideoFrameRGB.h"
//#include "Picture.h"
#include "File.h"
#include "VideoStream.h"
using namespace std;

int main() {
	avformat_free_context(nullptr);
	File file("c:/Coding/video.flv");
	
	
	//cout << "getStreamCount: " << static_cast<int>(file.getVideoStreamIndices()[0]) << endl;;
	VideoStream vs = file.getVideoStream(0);
	file.displayFileInfo();

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