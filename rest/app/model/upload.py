from ..lib.db import db
from datetime import datetime


class UploadSession(db.Model):
    __tablename__ = "upload_session"

    id = db.Column(db.String(50), primary_key=True)
    file_size = db.Column(db.BigInteger, nullable=False)
    has_uploaded = db.Column(db.Boolean, nullable=False, default=False)
    created_at = db.Column(db.DateTime(), default=datetime.utcnow(), nullable=False)
