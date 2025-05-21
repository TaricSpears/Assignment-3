from flask_cors import CORS
import json
from flask import Flask, request
app = Flask(__name__)
CORS(app, resources={r"/api/*": {"origins": "http://127.0.0.1:5500"}})


@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"


@app.route('/api/temperature', methods=['GET'])
def get_temperature():
    return json.dumps([{
        "temperature": 22.5,
        "timestamp": t
    } for t in range(10)])
