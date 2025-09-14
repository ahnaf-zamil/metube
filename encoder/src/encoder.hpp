#ifndef ENCODER_H
#define ENCODER_H

#include <string>

extern "C" {
#include <libavformat/avformat.h>
}

// Available resolutions
extern const int res_list[];

// Encoder namespace
namespace Encoder {
extern std::string cmd_template;

AVStream *get_input_stream(AVFormatContext *pFormatCtx, const char *filename);
void encode_res(char *exec_cmd);
void start_encode(std::string upid, std::string fname);
} // namespace Encoder

#endif // ENCODER_H
