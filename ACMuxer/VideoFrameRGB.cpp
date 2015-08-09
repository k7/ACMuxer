#include "VideoFrameRGB.h"



VideoFrameRGB::VideoFrameRGB(VideoFrame& videoFrame) : VideoFrame(videoFrame.getAVCodecContext()){
	fillRGB(videoFrame);
}

// AVPicture is a subset of AVFrame
AVPicture* VideoFrameRGB::getAVPicture() {
	return reinterpret_cast<AVPicture *>(avFrame);
}

void VideoFrameRGB::SavePPM(string filename) {
	filename += ".ppm";

	// Open file
	FILE *pFile;
	pFile = fopen(filename.c_str(), "wb");
	if (pFile != nullptr) {
		// Write header
		fprintf(pFile, "P6\n%d %d\n255\n", getWidth(), getHeight());

		// Write pixel data
		for (int y = 0; y < getHeight(); y++)
			fwrite(getAVFrame()->data[0] + y*getAVFrame()->linesize[0], 1, getWidth() * 3, pFile);

		// Close file
		fclose(pFile);
	}
}

void VideoFrameRGB::fillRGB(VideoFrame& videoFrame) {
	// Determine required buffer size and allocate buffer
	int numBytes = avpicture_get_size(PIX_FMT_RGB24, getWidth(), getHeight());
	size_t size = numBytes*sizeof(uint8_t);
	uint8_t * buffer = static_cast<uint8_t *>(av_malloc(size));

	// Assign appropriate parts of buffer to image planes
	// https://www.ffmpeg.org/doxygen/2.6/group__lavc__picture.html#gac1b22e9b246cb8b0928cca8f5fa524d4
	int bytesize = avpicture_fill(getAVPicture(), buffer, avPixelFormat, getWidth(), getHeight());

	// initialize SWS context for software scaling
	swsContext = sws_getContext(
		getWidth(),			// source
		getHeight(),
		getAVPixelFormat(),
		getWidth(),			// destination
		getHeight(),
		avPixelFormat,
		SWS_BILINEAR,		// flags
		nullptr,			// srcFilter
		nullptr,			// dstFilter
		nullptr				// param
		);
	ASSERTION(swsContext != nullptr);

	// Convert the image from its native format to RGB
	// https://www.ffmpeg.org/doxygen/2.6/group__libsws.html#gae531c9754c9205d90ad6800015046d74
	sws_scale(
		swsContext,												// c
		(uint8_t const * const *)videoFrame.getAVFrame()->data, // srcSlice[]
		videoFrame.getAVFrame()->linesize,						// srcStride[]
		0,														// srcSliceY
		getHeight(),											// srcSliceH
		getAVFrame()->data,										// dst[]
		getAVFrame()->linesize									// dstStride[]
		);

}