from flask import Flask, request, jsonify
from flask_socketio import SocketIO, emit
import cv2
import base64
import numpy as np
import paho.mqtt.client as mqtt

app = Flask(__name__)

current_mode = "motion"

# GET endpoint to return the current mode.
@app.route('/esp/mode', methods=['GET'])
def get_mode():
    return jsonify({"mode": current_mode})

# POST endpoint to update the current mode.
@app.route('/esp/mode', methods=['POST'])
def set_mode():
    global current_mode
    data = request.get_json()
    mode = data.get("mode")
    if mode in ["stream", "motion"]:
        current_mode = mode
        return jsonify({"status": "success", "mode": current_mode})
    else:
        return jsonify({"status": "error", "message": "Invalid mode"}), 400

if __name__ == '__main__':
    app.run(app, host='0.0.0.0', port=5000)
