from flask import Blueprint, current_app, jsonify, request, Response
import json
import random
bp = Blueprint("routes", __name__)


@bp.route('/api/temperature', methods=['GET'])
def get_temperature():
    return json.dumps([{
        "temperature": random.randint(0, 100),
        "timestamp": t
    } for t in range(10)])


@bp.route('/api/setmode', methods=['POST'])
def set_mode():
    body = request.get_json()
    print(body)
    return Response(status=200)


@bp.route('/api/setwindow', methods=['POST'])
def set_window_opening():
    body = request.get_json()
    print(body)
    return Response(status=200)


@bp.route('/api/getwindow', methods=['GET'])
def get_window_opening():
    return json.dumps({
        "value": random.randint(0, 100)
    })


@bp.route('/api/getstate', methods=['GET'])
def get_state():
    return json.dumps({
        "value": random.choice([
            "NORMAL",
            "HOT",
            "TOO_HOT",
            "ALARM"
        ])
    })


@bp.route('/api/alarm', methods=['POST'])
def alarm():
    return Response(status=200)
