from flask import Blueprint

from . import video

controllers = Blueprint("controllers", __name__, url_prefix="/")
controllers.register_blueprint(video.router)
