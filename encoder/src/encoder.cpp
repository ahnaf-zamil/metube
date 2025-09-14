#include "encoder.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/stat.h>

const int res_list[] = {1080, 720, 480, 360, 240};

namespace Encoder {

std::string cmd_template = "ffmpeg -hide_banner -loglevel warning -i %s -vf "
                           "\"scale=-2:%d%s\" -crf 24 %s";

AVStream *get_input_stream(AVFormatContext *pFormatCtx, const char *filename) {
  // Open input file
  if (avformat_open_input(&pFormatCtx, filename, NULL, 0) != 0) {
    exit(-1);
  }

  avformat_find_stream_info(pFormatCtx, NULL);
  av_dump_format(pFormatCtx, 0, filename, 0);

  int videoStream = -1;
  for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
    if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      videoStream = i;
      break;
    }
  }

  if (videoStream == -1) {
    exit(-1); // Didn't find a video stream
  }

  return pFormatCtx->streams[videoStream];
}

void encode_res(char *exec_cmd) {
  FILE *fp;
  char path[1035];

  fp = popen(exec_cmd, "r");
  if (fp == NULL) {
    std::cerr << "Failed to run command\n";
    return;
  }

  while (fgets(path, sizeof(path), fp) != NULL) {
    std::cout << path << std::endl;
  }

  pclose(fp);
}

void start_encode(std::string upid, std::string fname) {
  std::cout << "(" + upid + ") Started encoder" << std::endl;

  const char *upload_id = upid.c_str();
  const char *file_name = fname.c_str();

  AVFormatContext *ctx = NULL;
  AVStream *stream = get_input_stream(ctx, file_name);

  mkdir(upload_id, 0777);

  for (size_t i = 0; i < sizeof(res_list) / sizeof(int); i++) {
    int selected_res = res_list[i];
    if (selected_res <= stream->codecpar->height) {
      std::string output_name(upload_id);
      output_name += "/" + std::to_string(selected_res) + ".mp4";

      std::string fps_flag_len;
      if (selected_res < 720) {
        fps_flag_len = ",fps=fps=30";
      }

      int buff_len = cmd_template.length() + strlen(file_name) +
                     std::to_string(selected_res).length() +
                     fps_flag_len.length() + output_name.length() + 1;

      char *buff = (char *)malloc(buff_len * sizeof(char));
      std::snprintf(buff, buff_len, cmd_template.c_str(), file_name,
                    selected_res, fps_flag_len.c_str(), output_name.c_str());

      std::cout << "(" + upid + ") Encoding to " << selected_res
                << "p resolution" << std::endl;
      encode_res(buff);
      free(buff);
    }
  }

  std::cout << "(" + upid + ") Finished encoding" << std::endl;
  avformat_close_input(&ctx);
}

} // namespace Encoder
