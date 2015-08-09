#include "ffmpeg.h"
#include "InputVideo.h"
#include "ContratException.h"
#include <iostream>
#include <functional>

InputVideo::InputVideo(string filename) :filename(filename){
	av_register_all();
	
	// Allocate context memory
	pFormatContext = avformat_alloc_context();
	
	// Open context for file
	int ret = avformat_open_input(&pFormatContext, filename.c_str(), pInputFormat, pDictionary);
	ASSERTION(ret >= 0);
};

InputVideo::~InputVideo(){
	if (pInputFormat){
		// Close an opened input AVFormatContext. Free it and all its contents and set pointer to NULL.
		avformat_close_input(&pFormatContext);
		pInputFormat = 0;
	} 
	if (pFormatContext)	avformat_free_context(pFormatContext); // Free an AVFormatContext and all its streams.
	if (pCodecContext)	avcodec_free_context(&pCodecContext); // Free the codec context and everything associated with it and write NULL to the provided pointer.
}

void InputVideo::DisplayFileInfo(){
	if (pFormatContext){
		// Get stream info
		int ret = avformat_find_stream_info(pFormatContext, pDictionary);
		ASSERTION(ret >= 0);

		// Dump information about file onto standard error
		int streamIndex = 0;
		int isOutput = 0; // input
		av_dump_format(pFormatContext, streamIndex, filename.c_str(), isOutput);
	}
}

int InputVideo::getStreamCount(){
	return pFormatContext ? pFormatContext->nb_streams : 0;
}

vector<int> InputVideo::getVideoStreamIndices(){
	vector<int> indices;
	for (int i = 0; i < getStreamCount(); i++){
		if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			indices.push_back(i);
		}
	}
	return indices;
}

AVCodecContext * InputVideo::getCodecContext(int streamIndex){
	if (currentStreamIndex != streamIndex){
		if (pCodecContext){
			// Close and free previous codec context
			avcodec_close(pCodecContext);
			avcodec_free_context(&pCodecContext);
		}

		// Load stream info
		int ret = avformat_find_stream_info(pFormatContext, pDictionary);
		ASSERTION(ret >= 0);
		
		// Get a pointer to the codec context for the video stream
		AVCodecContext * pStreamCodecContext = 0;
		pStreamCodecContext = pFormatContext->streams[streamIndex]->codec;
		ASSERTION(pStreamCodecContext > 0);

		// Find the decoder for the video stream
		AVCodec * pCodec = 0;
		pCodec = avcodec_find_decoder(pStreamCodecContext->codec_id);
		ASSERTION(pCodec > 0); // Unsupported codec

		// Copy context. Note that we must not use the AVCodecContext from the video stream directly!
		pCodecContext = avcodec_alloc_context3(pCodec);
		ret = avcodec_copy_context(this->pCodecContext, pStreamCodecContext);
		ASSERTION(ret >= 0); // Couldn't copy codec context
		
		// Close source codec context
		ret = avcodec_close(pStreamCodecContext);
		ASSERTION(ret >= 0);

		// Open codec context
		ret = avcodec_open2(pCodecContext, pCodec, pDictionary);
		ASSERTION(ret >= 0);

		currentStreamIndex = streamIndex;
	}

	return pCodecContext;
}

AVFrame * InputVideo::getNextFrame(){
	AVFrame *pFrame = 0;
	pFrame = av_frame_alloc();
	ASSERTION(pFrame>0);

	AVFrame *pFrameRGB = 0;
	pFrameRGB = av_frame_alloc(); // intermediate pframe
	ASSERTION(pFrameRGB >0);

	uint8_t *buffer = 0;
	int numBytes;
	// Determine required buffer size and allocate buffer
	numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height);
	size_t size = numBytes*sizeof(uint8_t);
	buffer = (uint8_t *)av_malloc(size);
	int bufferint = (int)&buffer;

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	int bytesize = avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height);
	
	struct SwsContext *sws_ctx = NULL;
	int frameFinished;
	AVPacket packet;
	// initialize SWS context for software scaling
	sws_ctx = sws_getContext(pCodecContext->width,
		pCodecContext->height,
		pCodecContext->pix_fmt,
		pCodecContext->width,
		pCodecContext->height,
		PIX_FMT_RGB24,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
		);

	//int ret = av_read_frame(pFormatContext, &packet);
	//ASSERTION(ret >= 0);

	while (av_read_frame(pFormatContext, &packet) >= 0) {
		// Is this a packet from the video stream?
		if (packet.stream_index == currentStreamIndex) {
			// Decode video frame
			avcodec_decode_video2(pCodecContext, pFrame, &frameFinished, &packet);

			// Did we get a video frame?
			if (frameFinished) {
				
				// Convert the image from its native format to RGB
				sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data, pFrame->linesize, 0, pCodecContext->height, pFrameRGB->data, pFrameRGB->linesize);
				// Save the frame to disk
				//if (++i <= 5)
				//SaveFrame(pFrameRGB, pCodecCtx->width,pCodecCtx->height, i);

				break;
			}
		}
	}

	// Free the packet that was allocated by av_read_frame
	av_free_packet(&packet);
	
	return pFrame;
	//return pFrame;
}
