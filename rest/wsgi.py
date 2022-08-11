from dotenv import load_dotenv

load_dotenv()

from app import create_app, migrate
from app.model import *

app = create_app()

if __name__ == "__main__":
    app.run(port=5000, debug=True)
