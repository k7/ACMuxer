
#include "InputVideo.h"
#include <iostream>

using namespace std;

int main(){
	InputVideo input("d:/test2.flv");
	cout << "getStreamCount: " << input.getStreamCount() << endl;;
	input.getCodecContext(0);
	auto f = input.getNextFrame();
	return 0;
}