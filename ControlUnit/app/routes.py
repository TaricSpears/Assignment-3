from flask import Blueprint, current_app, jsonify, request, Response
import json
import random
from app import system_state
from app.state import Mode
bp = Blueprint("routes", __name__)


@bp.route('/api/temperature', methods=['GET'])
def get_temperature():
    return json.dumps([{
        "temperature": random.randint(0, 100),
        "timestamp": t
    } for t in range(10)])


@bp.route('/api/setmode', methods=['POST'])
def set_mode():
    print("Setting mode 123 ", request.get_json())
    mode = request.get_json().get("mode", "AUTOMATIC")
    if mode == "MANUAL":
        system_state.set_mode(Mode.MANUAL)
    elif mode == "AUTOMATIC":
        system_state.set_mode(Mode.AUTOMATIC)
    else:
        return jsonify({"error": "Invalid mode"}), 400
    return Response(status=200)


@bp.route('/api/getmode', methods=['GET'])
def get_mode():
    print("Getting mode")
    mode = system_state.get_mode()
    return json.dumps({
        "mode": mode.name
    })

@bp.route('/api/setwindow', methods=['POST'])
def set_window_opening():
    system_state.set_window_opening(request.get_json().get("percentage", 0.0))
    return Response(status=200)


@bp.route('/api/getwindow', methods=['GET'])
def get_window_opening():
    return json.dumps({
        "value": system_state.get_window_opening()
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
