
#include "InputVideo.h"
#include <iostream>
#include "VideoFrameRGB.h"

using namespace std;

int main(){
	InputVideo input("d:/test2.flv");
	cout << "getStreamCount: " << input.getStreamCount() << endl;;
	input.getCodecContext(0);
	for (int i = 0; i < 15; i++) {
		auto f = input.getNextFrame();
		VideoFrameRGB picture(f);
		picture.SavePPM("test" + to_string(i));
	}
	
	return 0;
}