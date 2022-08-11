from io import BytesIO
from time import time
from flask import Blueprint, request
from ..lib.validators import validate_input
from ..schema.video import create_upload_session_schema

from ..lib.db import db
from ..lib.s3 import s3_client
from ..lib.amqp import MQPublisher
from ..model import UploadSession

import secrets


router = Blueprint("video", __name__, url_prefix="/video")

FINAL_FILE_SIZE_LIMIT = 10  # in GB
UPLOAD_PROCESS_QUEUE = "upload_process"  # Queue for dispatching upload event to media processors

mq_publisher = MQPublisher(UPLOAD_PROCESS_QUEUE).connect()


@router.post("/uploads/create")
def create_upload_session():
    validate_input(create_upload_session_schema, request)
    file_size = request.json["file_size"]

    new_upload = UploadSession(id=secrets.token_urlsafe(32), file_size=file_size)
    db.session.add(new_upload)
    db.session.commit()

    return {"token": new_upload.id}

@router.post("/upload")
def upload_video():
    upload_token = request.headers.get("x-utoken")
    total_uploaded_bytes = request.headers.get("content-range")

    if not upload_token:
        return {"error": "Invalid upload session"}, 403

    upload_session = UploadSession.query.filter_by(id=upload_token).first()
    if not upload_session:
        return {"error": "Invalid upload session"}, 403

    if int(total_uploaded_bytes) > upload_session.file_size:
        return {"error": "File size limit reached"}, 403

    chunk_name = str(int(time() * 1000))

    buf = BytesIO()
    buf.write(request.data)
    buf.seek(0)

    s3_client.put_object("metube", f"raw_uploads/{upload_token}/{chunk_name}", buf, buf.getbuffer().nbytes)

    if int(total_uploaded_bytes) == upload_session.file_size:
        # File has finished uploading, now dispatching event to processing services
        mq_publisher.publish({
            "upload_id": upload_token,
            "initiated_on": str(int(time()))
        })
        return {"status": "Processing video"}, 201
    
    return "", 204
