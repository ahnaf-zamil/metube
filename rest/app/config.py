import os


class AppConfig:
    SQLALCHEMY_DATABASE_URI = f"postgresql://{os.environ['DB_USERNAME']}:{os.environ['DB_PASSWORD']}@127.0.0.1/metube"