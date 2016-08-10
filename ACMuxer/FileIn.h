#pragma once
#include "ffmpeg.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class VideoStream;
using namespace std;

class FileIn {
public:
	FileIn(const FileIn& other) = delete;

	FileIn(FileIn&& other)
		: avFormatContext(std::move(other.avFormatContext)) {
	}

	FileIn& operator=(const FileIn& other) = delete;

	FileIn& operator=(FileIn&& other) {
		if (this == &other)
			return *this;
		avFormatContext = std::move(other.avFormatContext);
		return *this;
	}

	FileIn(string filename);
	void displayFileInfo() const;
	vector<int> getVideoStreamIndices() const;					// List of stream indexes containing video
	VideoStream getVideoStream(int streamIndex) const;
	
	
private:
  unique_ptr<AVFormatContext, AVFormatContextDeleter> avFormatContext;

	
};

