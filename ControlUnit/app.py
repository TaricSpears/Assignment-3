from flask_cors import CORS
import json
from flask import Flask, request, Response
import random
app = Flask(__name__)
CORS(app, resources={r"/api/*": {"origins": "http://127.0.0.1:5500"}})


@app.route('/api/temperature', methods=['GET'])
def get_temperature():
    return json.dumps([{
        "temperature": random.randint(0, 100),
        "timestamp": t
    } for t in range(10)])


@app.route('/api/setmode', methods=['POST'])
def set_mode():
    body = request.get_json()
    print(body)
    return Response(status=200)


@app.route('/api/setwindow', methods=['POST'])
def set_window_opening():
    body = request.get_json()
    print(body)
    return Response(status=200)


@app.route('/api/getwindow', methods=['GET'])
def get_window_opening():
    return json.dumps({
        "value": random.randint(0, 100)
    })


@app.route('/api/getstate', methods=['GET'])
def get_state():
    return json.dumps({
        "value": random.choice([
            "NORMAL",
            "HOT",
            "TOO_HOT",
            "ALARM"
        ])
    })


@app.route('/api/alarm', methods=['POST'])
def alarm():
    return Response(status=200)
