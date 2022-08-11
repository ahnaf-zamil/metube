from minio import Minio
import os

s3_client = Minio('127.0.0.1:9000', access_key=os.environ["S3_ACCESS_KEY"], secret_key=os.environ["S3_SECRET_KEY"], secure=False)

if not s3_client.bucket_exists("metube"):
    s3_client.make_bucket("metube")