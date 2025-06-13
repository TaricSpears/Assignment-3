from flask import Blueprint, current_app, jsonify, request, Response
import json
import random
from app import system_state
from app.state import Mode
bp = Blueprint("routes", __name__)


@bp.route('/api/temperature', methods=['GET'])
def get_temperature():
    temperatures = system_state.get_measurements()
    return json.dumps([{
        "temperature": t.temperature,
        "timestamp": t.timestamp
    }for t in temperatures])


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
        "value": system_state.get_state().name
    })


@bp.route('/api/alarm', methods=['POST'])
def alarm():
    
    return Response(status=200)
