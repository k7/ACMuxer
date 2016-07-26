#include "VideoStream.h"
#include <iostream>
#include <assert.h>

VideoStream::VideoStream(const unique_ptr<AVFormatContext, AVFormatContextDeleter>& avFormatCtx, int streamIndex) : avFormatContext(avFormatCtx) {
	// Initialize AVPacket
	pkt.reset(&_pkt);
	av_init_packet(pkt.get());
	
	// Get a pointer to the codec context for the video stream
	AVCodecParameters * pStreamCodecParameters = avFormatContext->streams[streamIndex]->codecpar;
	
	// Find the decoder for the video stream
	AVCodec * pCodec = avcodec_find_decoder(pStreamCodecParameters->codec_id);
	assert(pCodec > nullptr); // Unsupported codec
	
	// Initialize the AVCodecContext to use the given AVCodec. The resulting struct should be freed with avcodec_free_context().
	avCodecContext.reset(avcodec_alloc_context3(pCodec));
	
	// Initialize the AVCodecContext to use the given AVCodec. 
	int ret = avcodec_open2(avCodecContext.get(), pCodec, nullptr);
}

Frame VideoStream::getNextFrame() {
	unique_ptr<AVFrame, AVFrameDeleter> frame = nullptr;
	if (state != AVERROR_EOF && !eof) {
		// Receive avFrame from decoder
		frame.reset(av_frame_alloc());
		state = avcodec_receive_frame(avCodecContext.get(), frame.get());
		if (state == AVERROR(EAGAIN)) {
			readPacket();
			state = avcodec_receive_frame(avCodecContext.get(), frame.get());
		}

		assert(state != AVERROR(EINVAL)); // "codec not opened, or it is an encoder" << endl;
		if (state == AVERROR(EAGAIN)) {
			//cout << "output is not available right now - user must try to send new input " << endl;
		}
		else if (state == AVERROR_EOF) {
			//cout << " the decoder has been fully flushed, and there will be no more output frames" << endl;
		}
		else {
			assert(state >= 0);
		}
	}
	return Frame(std::move(frame));
}

int VideoStream::readPacket() {
	// https://www.ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#details
	// Read the next avFrame from file and fill packet with compressed data
	// If pkt->buf is NULL, then the packet is valid until the next av_read_frame() or until avformat_close_input().
	// Otherwise the packet is valid indefinitely.In both cases the packet must be freed with av_packet_unref when it is no longer needed.

	// temp packet copy
	/*AVPacket cpy;
	cpy.buf = nullptr;
	av_copy_packet(&cpy, pkt.get());*/
	
	int ret = av_read_frame(avFormatContext.get(), pkt.get());
	if (ret < 0) {
		cout << "Error or end of file." << endl;
		eof = true;
	}
	else {
		// Send packet to decoder
		cout << pc++ << endl;
		ret = avcodec_send_packet(avCodecContext.get(), pkt.get());
		assert(ret == 0);
	}

	return ret;
}

	


