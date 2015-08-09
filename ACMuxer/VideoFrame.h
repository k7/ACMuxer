#pragma once
#include "ffmpeg.h"
#include "ContratException.h"

using namespace std;

/* AVFrame
http://ffmpeg.org/doxygen/trunk/structAVFrame.html

This structure describes decoded (raw) audio or video data.

AVFrame must be allocated using av_frame_alloc(). Note that this only allocates the AVFrame itself, the buffers for the
data must be managed through other means (see below). AVFrame must be freed with av_frame_free().

AVFrame is typically allocated once and then reused multiple times to hold different data (e.g. a single AVFrame to hold
frames received from a decoder). In such a case, av_frame_unref() will free any references held by the frame and reset
it to its original clean state before it is reused again.

The data described by an AVFrame is usually reference counted through the AVBuffer API. The underlying buffer references
are stored in AVFrame.buf / AVFrame.extended_buf. An AVFrame is considered to be reference counted if at least one
reference is set, i.e. if AVFrame.buf[0] != NULL. In such a case, every single data plane must be contained in one of
the buffers in AVFrame.buf or AVFrame.extended_buf. There may be a single buffer for all the data, or one separate
buffer for each plane, or anything in between.

sizeof(AVFrame) is not a part of the public ABI, so new fields may be added to the end with a minor bump. Similarly
fields that are marked as to be only accessed by av_opt_ptr() can be reordered. This allows 2 forks to add fields
without breaking compatibility with each other.
*/
class VideoFrame {
public:
	VideoFrame() = delete;
	VideoFrame(AVCodecContext * codecContext);
	VideoFrame(const VideoFrame& other) = delete;				// Disable copy constructor
	VideoFrame& operator=(const VideoFrame& other) = delete;	// Disable copy assignment
	VideoFrame(VideoFrame&& other);								// Move constructor
	VideoFrame& operator=(VideoFrame&& other);					// Move assignment
	~VideoFrame();

	void freeResources();
	void reset(AVFrame * avFrame); // TODO: For reuse

	AVCodecContext * getAVCodecContext();
	AVFrame * getAVFrame();
	int getWidth();
	int getHeight();

protected:
	AVPixelFormat getAVPixelFormat();
	AVCodecContext * avCodecContext;
	AVFrame * avFrame = nullptr;
private:
	int streamIndex;
	int frameIndex = 0;
};