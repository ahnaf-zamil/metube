#include "worker.hpp"
#include "encoder.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

using namespace std;

int rm_files(const char *pathname, const struct stat *sbuf, int type,
             struct FTW *ftwb) {
  if (remove(pathname) < 0) {
    perror("ERROR: remove");
    return -1;
  }
  return 0;
}

Worker::Worker(const std::string id) { upload_id = id; }

void Worker::_exit() { pthread_exit((void *)EXIT_FAILURE); }

void Worker::_check_bucket_existence() {
  bool bucket_exists;
  {
    s3::BucketExistsArgs args;
    args.bucket = bucket_name;

    s3::BucketExistsResponse resp = client->BucketExists(args);
    if (!resp) {
      cerr << "Couldn't check for bucket existence";
      return _exit();
    }

    bucket_exists = resp.exist;
  }

  if (!bucket_exists) {
    cerr << "Bucket " + bucket_name + " does not exist; quitting" << endl;
    return _exit();
  }
}

void Worker::_download_and_merge_chunks() {
  s3::ListObjectsV1Args listobj_args;
  listobj_args.bucket = bucket_name;
  listobj_args.prefix = upload_folder + upload_id;

  s3::ListObjectsResponse listobj_resp = client->ListObjectsV1(listobj_args);

  if (!listobj_resp) {
    cerr << "Failed to list objects for upload ID: " + upload_id << endl;
    return _exit();
  }

  // Creating folder for storing uploaded chunks
  string dir_name = temp_folder + upload_id;
  if (mkdir(dir_name.c_str(), 0777) < 0) {
    perror("mkdir");
    return _exit();
  }

  // Downloading chunks
  s3::DownloadObjectArgs dl_args;
  dl_args.bucket = bucket_name;

  // File to append chunks to
  FILE *out_file = fopen((dir_name + "/output").c_str(), "ab");

  for (const s3::Item &item : listobj_resp.contents) {
    string filename = item.name;
    dl_args.object = item.name;
    dl_args.filename =
        dir_name + "/" +
        filename.erase(0, (upload_folder + upload_id).length() + 1);

    s3::DownloadObjectResponse dl_resp = client->DownloadObject(dl_args);

    if (!dl_resp) {
      cerr << "Failed to download chunks for: " + upload_id + ": " +
                  dl_resp.Error().String()
           << endl;
      return _exit();
    }

    // Append chunk bytes to output file
    FILE *chunk_file = fopen(dl_args.filename.c_str(), "rb");
    fseek(chunk_file, 0L, SEEK_END);
    long fsize = ftell(chunk_file);
    rewind(chunk_file);
    char *buffer = (char *)malloc((fsize + 1) * sizeof(char));
    fread(buffer, 1, fsize, chunk_file);
    fwrite(buffer, fsize, 1, out_file);
    free(buffer);
    fclose(chunk_file);

    // Delete file chunk
    remove(dl_args.filename.c_str());

    s3::RemoveObjectArgs rem_args;
    rem_args.bucket = bucket_name;
    rem_args.object = item.name;

    s3::RemoveObjectResponse rem_resp = client->RemoveObject(rem_args);
    if (!rem_resp) {
      cerr << "Failed to delete chunks for: " + upload_id + ": " +
                  dl_resp.Error().String()
           << endl;
      return _exit();
    }
  }

  fclose(out_file);
  cout << "Downloaded and merged files for: " + upload_id << endl;
}

void *Worker::init_thread(void *ptr) {
  string upload_id((char *)ptr);
  cout << "Init worker for upload ID: " << upload_id << endl;

  Worker worker_instance(upload_id);
  worker_instance.start_job();

  return (void *)0;
}

void Worker::start_job() {
  // Initialize Minio client
  s3::BaseUrl base_url;
  base_url.host = getenv("MINIO_HOST");
  base_url.https = false;

  minio::creds::StaticProvider cred_provider(getenv("MINIO_USER"),
                                             getenv("MINIO_PASS"));
  client = new s3::Client(base_url, &cred_provider);

  // Check bucket existence
  _check_bucket_existence();

  // Download and merge chunks
  _download_and_merge_chunks();

  // Start encoding
  Encoder::start_encode(upload_id, temp_folder + upload_id + "/output");

  // Delete temp chunks
  if (nftw((temp_folder + upload_id).c_str(), rm_files, 10,
           FTW_DEPTH | FTW_MOUNT | FTW_PHYS) < 0) {
    perror("ERROR: ntfw");
    _exit();
  }
}
