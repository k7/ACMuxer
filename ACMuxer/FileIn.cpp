#include "FileIn.h"
#include "VideoStream.h"
#include <assert.h>

FileIn::FileIn(string filename) {
	av_register_all();

	// Allocate format context memory
	avFormatContext.reset(avformat_alloc_context());
	
	// Open an input stream and read the header. The codecs are not opened. 
	AVFormatContext* avFormatContextPtr = avFormatContext.get();
	int ret = avformat_open_input(&avFormatContextPtr, filename.c_str(), nullptr, nullptr);
	assert(ret >= 0);

	// Get stream info (this initializes the number of streams, etc.)
	ret = avformat_find_stream_info(avFormatContext.get(), nullptr);
	assert(ret >= 0);
}

void FileIn::displayFileInfo() const {
	// Dump information about file onto standard error
	int streamIndex = 0;
	int isOutput = 0; // input
	av_dump_format(avFormatContext.get(), streamIndex, this->avFormatContext->filename, isOutput);
}

// Get indices of video streams in file
vector<int> FileIn::getVideoStreamIndices() const {
	vector<int> indices;
	for (unsigned int i = 0; i < avFormatContext->nb_streams; i++) {
		if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			indices.push_back(i);
		}
	}
	return indices;
}

VideoStream FileIn::getVideoStream(int streamIndex) const {
	return VideoStream(avFormatContext, streamIndex);
}
