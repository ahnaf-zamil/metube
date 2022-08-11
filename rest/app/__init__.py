from flask import Flask
from flask_cors import CORS

from .controller import controllers
from .lib.db import db, migrate
from .config import AppConfig


def create_app():
    app = Flask(__name__)
    app.config.from_object(AppConfig)

    db.init_app(app)
    migrate.init_app(app, db)

    CORS(app, supports_credentials=True)

    app.register_blueprint(controllers)

    return app