#include "Picture.h"

Picture::Picture(VideoFrame& videoFrame) : VideoFrame(videoFrame.getAVCodecContext()) {
	// Allocate memory for the pixels of a picture and setup the AVPicture fields for it.
	avpicture_alloc(reinterpret_cast<AVPicture*>(avFrame), avPixelFormat, videoFrame.getWidth(), videoFrame.getHeight());

	// initialize SWS context for software scaling
	swsContext = sws_getContext(
		videoFrame.getWidth(),			// source
		videoFrame.getHeight(),
		videoFrame.getAVPixelFormat(),
		videoFrame.getWidth(),			// destination
		videoFrame.getHeight(),
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
		static_cast<uint8_t const * const *>
			(videoFrame.getAVFrame()->data),					// srcSlice[]
		videoFrame.getAVFrame()->linesize,						// srcStride[]
		0,														// srcSliceY
		videoFrame.getHeight(),									// srcSliceH
		avFrame->data,										// dst[]
		avFrame->linesize										// dstStride[]
		);

}

Picture::~Picture() {
	freeResources();
}

void Picture::freeResources() {
	avpicture_free(reinterpret_cast<AVPicture*>(avFrame));
}

void Picture::SavePPM(const string& filename) {
	// Open file
	FILE *pFile;
	pFile = fopen((filename + ".ppm").c_str(), "wb");
	if (pFile != nullptr) {
		// Write header
		fprintf(pFile, "P6\n%d %d\n255\n", getWidth(), getHeight());

		// Write pixel data
		for (int y = 0; y < getHeight(); y++)
			fwrite(avFrame->data[0] + y*avFrame->linesize[0], 1, getWidth() * 3, pFile);

		// Close file
		fclose(pFile);
	}
}