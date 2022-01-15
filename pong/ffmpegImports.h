extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/avfft.h>

#include <libavdevice/avdevice.h>

#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include <libavformat/avformat.h>
#include <libavformat/avio.h>

	// libav resample

#include <libavutil/opt.h>
#include <libavutil/common.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/file.h>
// lib swresample

#include <libswscale/swscale.h>
}

class VideoCapture {
public:

	VideoCapture();

	~VideoCapture();

	void Init(int width, int height, int fpsrate, int bitrate);

	void AddFrame(uint8_t* data);

	void Finish();

private:

	AVOutputFormat* oformat;
	AVFormatContext* ofctx;


	AVStream* videoStream;
	AVFrame* videoFrame;

	AVCodec* codec;
	AVCodecContext* cctx;

	SwsContext* swsCtx;

	int frameCounter;

	int fps;

	void Free();

	void Remux();
};
