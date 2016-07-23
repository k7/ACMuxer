#include "ffmpeg.h"
#include "InputVideo.h"
#include "ContratException.h"
#include <iostream>
#include <functional>

// Move constructor
InputVideo::InputVideo(InputVideo&& other) : 
	avDictionary(other.avDictionary),
	avFormatContext(other.avFormatContext),
	avInputFormat(other.avInputFormat),
	avCodecContext(other.avCodecContext) {}

// Move assignment operator
InputVideo& InputVideo::operator=(InputVideo&& other) {
	if (this == &other)
		return *this;
	avDictionary = other.avDictionary;
	avFormatContext = other.avFormatContext;
	avInputFormat = other.avInputFormat;
	avCodecContext = other.avCodecContext;
	// Free other object so destructor does not free it
	avDictionary = nullptr;
	avFormatContext = nullptr;
	avInputFormat = nullptr;
	avCodecContext = nullptr;

	return *this;
}

void InputVideo::freeResources() {
	if (avInputFormat) {
		// Close opened input AVFormatContext.
		avformat_close_input(&avFormatContext);
		avInputFormat = nullptr;
	}
	if (avFormatContext) {
		avformat_free_context(avFormatContext); // Free an AVFormatContext and all its streams.
	}
	if (avCodecContext)	avcodec_free_context(&avCodecContext); // Free the codec context and everything associated with it and write NULL to the provided pointer.
}

// Constructor
InputVideo::InputVideo(string filename) :filename(filename){
	av_register_all();
	
	// Allocate context memory
	avFormatContext = avformat_alloc_context();
	
	// Open context for file
	int ret = avformat_open_input(&avFormatContext, filename.c_str(), avInputFormat, avDictionary);
	ASSERTION(ret >= 0);
};

InputVideo::~InputVideo(){
	freeResources();
}

void InputVideo::DisplayFileInfo() const {
	if (avFormatContext){
		// BUG: Further actions crash following avformat_find_stream_info, use a new instance
		InputVideo iv(filename);

		// Get stream info
		int ret = avformat_find_stream_info(iv.avFormatContext, nullptr);
		ASSERTION(ret >= 0);

		// Dump information about file onto standard error
		int streamIndex = 0;
		int isOutput = 0; // input
		av_dump_format(iv.avFormatContext, streamIndex, filename.c_str(), isOutput);
	}
}

int InputVideo::getStreamCount() const {
	return avFormatContext ? avFormatContext->nb_streams : 0;
}

vector<int> InputVideo::getVideoStreamIndices() const {
	vector<int> indices;
	for (int i = 0; i < getStreamCount(); i++){
		if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			indices.push_back(i);
		}
	}
	return indices;
}

/* TODO: Class instance should be per-stream with parent class providing streams list,
this would be in constructor (too much maintenance to support changing streams in class) */
AVCodecContext * InputVideo::openStream(int streamIndex) {
	if (currentStreamIndex != streamIndex) {
		if (avCodecContext) {
			// Close and free previous codec context
			avcodec_close(avCodecContext);
			avcodec_free_context(&avCodecContext);
		}

		// Load stream info
		int ret = avformat_find_stream_info(avFormatContext, avDictionary);
		ASSERTION(ret >= 0);

		// Get CodecParameters instead of CodecContext (why is this needed?)
		// See: https://ffmpeg.org/pipermail/ffmpeg-cvslog/2016-April/099152.html
		// Those problems are resolved by replacing the AVStream embedded codec
		// context with a newly added AVCodecParameters instance, which stores only
		// the stream parameters exported by the demuxers or read by the muxers.
		AVCodecParameters * pStreamCodecParameters = nullptr;
		pStreamCodecParameters = avFormatContext->streams[streamIndex]->codecpar;
		//pStreamCodecContext->thread_count = 1;
		ASSERTION(pStreamCodecParameters > nullptr);

		// Find the decoder for the video stream
		AVCodec * pCodec = nullptr;
		pCodec = avcodec_find_decoder(pStreamCodecParameters->codec_id);
		ASSERTION(pCodec > nullptr); // Unsupported codec

		// Open codec context
		ret = avcodec_open2(avCodecContext, pCodec, avDictionary);
		ASSERTION(ret >= 0);
	}
	return avCodecContext;
}

AVCodecContext * InputVideo::getCodecContextOLD(int streamIndex){
	if (currentStreamIndex != streamIndex) {
		if (avCodecContext) {
			// Close and free previous codec context
			avcodec_close(avCodecContext);
			avcodec_free_context(&avCodecContext);
		}

		// Load stream info
		int ret = avformat_find_stream_info(avFormatContext, avDictionary);
		ASSERTION(ret >= 0);

		// Get a pointer to the codec context for the video stream
		AVCodecContext * pStreamCodecContext = 0;
		pStreamCodecContext = avFormatContext->streams[streamIndex]->codec;
		//pStreamCodecContext->thread_count = 1;
		ASSERTION(pStreamCodecContext > 0);

		// Find the decoder for the video stream
		AVCodec * pCodec = 0;
		pCodec = avcodec_find_decoder(pStreamCodecContext->codec_id);
		ASSERTION(pCodec > 0); // Unsupported codec

		// Copy context. Note that we must not use the AVCodecContext from the video stream directly!
		avCodecContext = avcodec_alloc_context3(pCodec);
		ret = avcodec_copy_context(this->avCodecContext, pStreamCodecContext);
		ASSERTION(ret >= 0); // Couldn't copy codec context

		// Close source codec context
		ret = avcodec_close(pStreamCodecContext);
		ASSERTION(ret >= 0);

		// Open codec context
		ret = avcodec_open2(avCodecContext, pCodec, avDictionary);
		ASSERTION(ret >= 0);

		currentStreamIndex = streamIndex;
	}

	return avCodecContext;
}

VideoFrame InputVideo::getNextFrame() const {
	// Allocate AVFrame
	VideoFrame avFrame(avCodecContext);

	//struct SwsContext *sws_ctx = NULL;
	int frameFinished;
	AVPacket packet;

	while (av_read_frame(avFormatContext, &packet) >= 0) {
		// Is this a packet from the video stream?
		if (packet.stream_index == currentStreamIndex) {
			// Decode video frame
			avcodec_decode_video2(avCodecContext, avFrame.getAVFrame(), &frameFinished, &packet);

			// Did we get a video frame?
			if (frameFinished) {
				break;
			}
		}
	}

	// Free the packet that was allocated by av_read_frame
	av_free_packet(&packet);
	
	return avFrame;
}
