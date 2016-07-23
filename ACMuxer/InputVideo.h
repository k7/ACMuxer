#include "ffmpeg.h"
#include <string>
#include <vector>
#include <iostream>
#include "VideoFrame.h"

using namespace std;

class InputVideo{
public:
	InputVideo(const InputVideo& other) = delete;				// Disable copy constructor
	InputVideo& operator=(const InputVideo& other) = delete;	// Disable copy assignment
	InputVideo(InputVideo&& other);								// Move constructor
	InputVideo& operator=(InputVideo&& other);					// Move assignment
	void freeResources();
	
	InputVideo(string filepath);								// Constructor
	~InputVideo();
	int getStreamCount() const;

	AVCodecContext * openStream(int streamIndex);			// Main external API structure.
	AVCodecContext * getCodecContextOLD(int streamIndex);		// Main external API structure.
	vector<int> getVideoStreamIndices() const;					// List of stream indexes containing video
	VideoFrame getNextFrame() const;
	//unique_ptr<AVFrame, AVPtrDeleter> getNextFrame();
	void DisplayFileInfo() const;

private:
	AVDictionary ** avDictionary = nullptr;						// Options, unused
	AVFormatContext * avFormatContext = nullptr;				// Format I/O context
	AVInputFormat * avInputFormat = nullptr;					// Input Format
	AVCodecContext *avCodecContext = nullptr;					// Codec Context
	int currentStreamIndex = -1;
	int currentFrameIndex = -1;
	//InputVideo(const InputVideo &) = delete;
	string filename;
	
};