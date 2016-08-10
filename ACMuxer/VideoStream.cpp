#include "VideoStream.h"
#include <iostream>
#include <assert.h>

VideoStream::VideoStream(const unique_ptr<AVFormatContext, AVFormatContextDeleter>& avFormatCtx, int streamIndex) 
  : avFormatContext(avFormatCtx), streamIndex(streamIndex){
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
			if(this->firstFramePts == -1) {
			  firstFramePts = pkt.get()->pts;
			}
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
	Frame f(std::move(frame), *pkt.get());
	f.firstFramePts = firstFramePts; // meh, parent class should be called and know all this.
	return f;
	//return Frame(std::move(frame), *pkt.get());
}

int VideoStream::readPacket() {
	// https://www.ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#details
	// Read the next avFrame from file and fill packet with compressed data
	// If pkt->buf is NULL, then the packet is valid until the next av_read_frame() or until avformat_close_input().
	// Otherwise the packet is valid indefinitely.In both cases the packet must be freed with av_packet_unref when it is no longer needed.
	
	int ret = av_read_frame(avFormatContext.get(), pkt.get());
	if (ret < 0) {
		cout << "Error or end of file." << endl;
		eof = true;
	}
	else {
		// Send packet to decoder
		ret = avcodec_send_packet(avCodecContext.get(), pkt.get());
		assert(ret == 0);
	}

	return ret;
}

string VideoStream::getErrorReadPacket(int ret) {
	switch(ret) {
	case 0: return "Success";
	case AVERROR(EAGAIN): return " input is not accepted right now - the packet must be resent after trying to read output AVERROR_EOF: the decoder has been flushed, and no new packets can be sent to it (also returned if more than 1 flush packet is sent)";
	}
}


