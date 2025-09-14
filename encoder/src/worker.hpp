#ifndef WORKER_H
#define WORKER_H

#include <ftw.h>
#include <pthread.h>
#include <string>
#include <sys/stat.h>

#include "minio/client.h"

namespace s3 = minio::s3;

// Helper function for nftw
int rm_files(const char *pathname, const struct stat *sbuf, int type,
             struct FTW *ftwb);

class Worker {
private:
  std::string upload_id;
  s3::Client *client;

  const std::string temp_folder = "/tmp/";
  const std::string bucket_name = "metube";
  const std::string upload_folder = "raw_uploads/";

  void _exit();
  void _check_bucket_existence();
  void _download_and_merge_chunks();

public:
  static void *init_thread(void *ptr);
  void start_job();
  explicit Worker(const std::string id);
};

#endif // WORKER_H
