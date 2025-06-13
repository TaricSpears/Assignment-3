from app.state import SystemState
from flask import Flask
from flask_cors import CORS

system_state = SystemState()


def create_app():
    app = Flask(__name__)
    CORS(app, resources={r"/api/*": {"origins": "http://127.0.0.1:5500"}})

    from app.routes import bp as routes_bp
    app.register_blueprint(routes_bp)

    return app
