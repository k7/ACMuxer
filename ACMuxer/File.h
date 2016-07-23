#pragma once
#include "ffmpeg.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class VideoStream;
using namespace std;

class File {
public:
	File(const File& other) = delete;

	File(File&& other)
		: avFormatContext(std::move(other.avFormatContext)) {
	}

	File& operator=(const File& other) = delete;

	File& operator=(File&& other) {
		if (this == &other)
			return *this;
		avFormatContext = std::move(other.avFormatContext);
		return *this;
	}

	File(string filename);
	void displayFileInfo() const;
	vector<int> getVideoStreamIndices() const;					// List of stream indexes containing video
	VideoStream getVideoStream(int streamIndex) const;
private:
	unique_ptr<AVFormatContext, sFormatContextDeleter> avFormatContext;
};

