#include "FileOut.h"
#include <cassert>
#include <thread>

FileOut::FileOut(string filename, int width, int height) : FileOut(filename, width, height, AV_CODEC_ID_H264) {

}
FileOut::FileOut(string filename, int width, int height, AVCodecID avCodecID_video) : filename(filename) {
   av_register_all();

   // Initialize AVPacket
   pkt.reset(&_pkt);
   av_init_packet(pkt.get());
   pkt.get()->data = nullptr;
   pkt.get()->size = 0;

   // Get Codec Context
   AVCodec* videoCodec = avcodec_find_encoder(avCodecID_video);
   assert(videoCodec != nullptr);
   ccv.reset(avcodec_alloc_context3(videoCodec));
   assert(ccv);

   // TEST CODE
   /* put sample parameters */
   //ccv.get()->bit_rate = 400000;
   /* resolution must be a multiple of two */
   ccv.get()->width = width;
   ccv.get()->height = height;
   /* frames per second */

   AVRational r { 1, 30 };
   ccv.get()->time_base = r;
   /* emit one intra frame every ten frames
   * check frame pict_type before passing frame
   * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
   * then gop_size is ignored and the output of encoder
   * will always be I frame irrespective to gop_size
   */
   ccv.get()->gop_size = 0;
   ccv.get()->max_b_frames = 0;
   ccv.get()->pix_fmt = AV_PIX_FMT_YUV420P;
   //ccv.get()->pkt_timebase = r;
   ccv.get()->time_base = r;
   ccv.get()->ticks_per_frame = 2;
   //ccv.get()->timecode_frame_start = 1;
   av_codec_set_pkt_timebase(ccv.get(), r);

   if (avCodecID_video == AV_CODEC_ID_H264)
	av_opt_set(ccv.get()->priv_data, "preset", "slow", 0);

   assert(avcodec_open2(ccv.get(), videoCodec, NULL) == 0);

   f = fopen(filename.c_str(), "wb");
   assert(f);
}

void FileOut::encodeFrameTest() const {
   // dummy frame
   AVFrame * frame = av_frame_alloc();
   if (!frame) {
	fprintf(stderr, "Could not allocate video frame\n");
	exit(1);
   }
   frame->format = ccv.get()->pix_fmt;
   frame->width = ccv.get()->width;
   frame->height = ccv.get()->height;

   /* the image can be allocated by any means and av_image_alloc() is
   * just the most convenient way if av_malloc() is to be used */
   int got_output;
   int ret = av_image_alloc(frame->data, frame->linesize, ccv.get()->width, ccv.get()->height, ccv.get()->pix_fmt, 32);
   assert(ret >= 0);


   /* encode 1 second of video */
   AVPacket p;
   int i, x, y;
   auto c = ccv.get();

   for (i = 0; i < 100; i++) {
	av_init_packet(&p);
	p.data = NULL;    // packet data will be allocated by the encoder
	p.size = 0;

	fflush(stdout);
	/* prepare a dummy image */
	/* Y */
	for (y = 0; y < c->height; y++) {
	   for (x = 0; x < c->width; x++) {
		frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
	   }
	}

	/* Cb and Cr */
	for (y = 0; y < c->height / 2; y++) {
	   for (x = 0; x < c->width / 2; x++) {
		frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
		frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
	   }
	}

	//frame->pts = i;

	ret = avcodec_send_frame(ccv.get(), frame);
	assert(ret == 0);

	ret = avcodec_receive_packet(ccv.get(), &p);

	if(ret == AVERROR(EAGAIN)) {
	   cout << "AVERROR(EAGAIN) : output is not available right now - user must try to send input" << endl;
	}else if(ret == AVERROR_EOF) {
	   cout << "AVERROR_EOF: the encoder has been fully flushed, and there will be no more output packets" << endl;
	}else if (ret == AVERROR(EINVAL)) {
	   cout << "AVERROR(EINVAL) : codec not opened, or it is an encoder" << endl;
	}else if (ret ==0) {
	   cout << "Success - got packet" << endl;
	   if (ret == 0) {
		printf("Write frame %3d (size=%5d)\n", i, p.size);
		fwrite(p.data, 1, p.size, f);
		av_packet_unref(&p);
	   }
	} else {
	   cout << "Other errors" << endl;
	}
	

	/* encode the image */
	/*ret = avcodec_encode_video2(c, &p, frame, &got_output);
	if (ret < 0) {
	   fprintf(stderr, "Error encoding frame\n");
	   exit(1);
	}

	if (got_output) {
	   printf("Write frame %3d (size=%5d)\n", i, p.size);
	   fwrite(p.data, 1, p.size, f);
	   av_packet_unref(&p);
	}*/
   }


   /* get the delayed frames */
   ret = avcodec_send_frame(ccv.get(), nullptr);
   assert(ret == 0);

   ret = avcodec_receive_packet(ccv.get(), &p);
   if (ret == 0) {
	printf("Write frame %3d (size=%5d)\n", i, p.size);
	fwrite(p.data, 1, p.size, f);
	av_packet_unref(&p);
   }

   /* get the delayed frames */
   /*for (got_output = 1; got_output; i++) {
	fflush(stdout);

	ret = avcodec_encode_video2(c, &p, NULL, &got_output);
	if (ret < 0) {
	   fprintf(stderr, "Error encoding frame\n");
	   exit(1);
	}

	if (got_output) {
	   printf("Write frame %3d (size=%5d)\n", i, p.size);
	   fwrite(p.data, 1, p.size, f);
	   av_packet_unref(&p);
	}
   }*/

   /* add sequence end code to have a real MPEG file */
   uint8_t endcode[] = { 0, 0, 1, 0xb7 };
   fwrite(endcode, 1, sizeof(endcode), f);
   fclose(f);

   avcodec_close(c);
   av_free(c);
   av_freep(&frame->data[0]);
   av_frame_free(&frame);
   printf("\n");



}

void FileOut::encodeFrame(Frame& frame, uint64_t pts) {
   //frame.avFrame.get()->pts = ccv.get()->frame_number;
   auto fr = frame.avFrame.get();
   fr->pts = frame.getRelativePts();
   
   fr->best_effort_timestamp = 0;
   fr->pkt_pts = fr->pts;
   fr->pkt_dts = 0;
   fr->pkt_duration = 100;
   fr->pkt_pos = 0;
   fr->pkt_size = 0;
   pkt->duration = 100;

   int ret = avcodec_send_frame(ccv.get(), frame.avFrame.get());
   frameNo++;
   assert(ret == 0);
   // packet.pts = av_rescale_q ( frame. pts , frameTimeBase,packetTimeBase )
   pkt.get()->pts = av_rescale_q(fr->pts, ccv->time_base, ccv->time_base);
   ret = avcodec_receive_packet(ccv.get(), pkt.get());
   if (ret == AVERROR(EAGAIN)) {
	cout << "AVERROR(EAGAIN) : output is not available right now - user must try to send input" << endl;
   }
   else if (ret == AVERROR_EOF) {
	cout << "AVERROR_EOF: the encoder has been fully flushed, and there will be no more output packets" << endl;
   }
   else if (ret == AVERROR(EINVAL)) {
	cout << "AVERROR(EINVAL) : codec not opened, or it is an encoder" << endl;
   }

   else if (ret == 0) {
	cout << "Success - got packet" << endl;
	if (ret == 0) {
	   pkt->duration = 100;
	   printf("Write frame %3d (size=%5d)\n", frameNo, pkt.get()->size);
	   fwrite(pkt.get()->data, 1, pkt.get()->size, f);
	   av_packet_unref(pkt.get());
	}
   }
   else {
	cout << "Other errors" << endl;
   }
}

void FileOut::encodeFinish(uint64_t pts) {
   int ret = avcodec_send_frame(ccv.get(), nullptr);
   assert(ret == 0);

   ret = avcodec_receive_packet(ccv.get(), pkt.get());
   if (ret == 0) {
	printf("Write frame %3d (size=%5d)\n", frameNo++, pkt.get()->size);
	fwrite(pkt.get()->data, 1, pkt.get()->size, f);
	av_packet_unref(pkt.get());
   }

   /* get the delayed frames */
   /*for (got_output = 1; got_output; i++) {
   fflush(stdout);

   ret = avcodec_encode_video2(c, &p, NULL, &got_output);
   if (ret < 0) {
   fprintf(stderr, "Error encoding frame\n");
   exit(1);
   }

   if (got_output) {
   printf("Write frame %3d (size=%5d)\n", i, pkt.get()->size);
   fwrite(pkt.get()->data, 1, pkt.get()->size, f);
   av_packet_unref(&p);
   }
   }*/

   /* add sequence end code to have a real MPEG file */
   uint8_t endcode[] = { 0, 0, 1, 0xb7 };
   fwrite(endcode, 1, sizeof(endcode), f);
   fclose(f);

   avcodec_close(ccv.get());
   av_free(ccv.get());
   //av_freep( &frame->data[0]);
   //av_frame_free(&frame);
   printf("\n");
}