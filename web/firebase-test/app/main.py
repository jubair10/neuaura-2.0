from flask import Flask, jsonify, make_response, request
import os
import time
import base64
from logger import setup_logger
from utility.base.firebase_init import initialize_firebase
from utility.esp_cam import fetch_image_urls_and_save_to_rtdb
from firebase_admin import db


initialize_firebase()
logger = setup_logger()

UPLOAD_FOLDER = "logger"


app = Flask(__name__)
current_mode = "stream"

@app.route("/")
def func():
    return make_response("Hey, Bruh!")

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
        print(current_mode)
        return jsonify({"status": "success", "mode": current_mode})
    else:
        return jsonify({"status": "error", "message": "Invalid mode"}), 400


@app.route("/upload-url", methods=["POST"])
def handle_upload():
    """
    Handles incoming POST requests with a URL and timestamp from the ESP32.
    """
    data = request.get_json()
    if not data:
        return jsonify({"error": "Invalid or missing JSON payload"}), 400

    url = data.get("url")
    timestamp = data.get("timestamp")

    if not url or not timestamp:
        return jsonify({"error": "Missing 'url' or 'timestamp' in payload"}), 400

    metadata = {
        "url": base64.b64encode(url.encode("utf-8")).decode("utf8"),
        "timestamp": timestamp,
        "received_at": int(time.time()),
    }
    metadata_file = os.path.join(UPLOAD_FOLDER, "metadata.txt")
    ref = db.reference("image_urls/")
    log_ = ref.update({timestamp: metadata})
    logger.info("Realtime DB Updated due to alert image")

    with open(metadata_file, "a") as f:
        f.write(str(metadata) + "\n")

    return make_response("success")


@app.route("/esp-data", methods=["POST"])
def esp_data():
    # Check if request contains JSON data
    if request.is_json:
        data = request.get_json()

        # Extract the necessary fields from the received JSON
        fire_detected = data.get("fire_detected")
        gas_detected = data.get("gas_detected")
        temperature = data.get("temperature")
        humidity = data.get("humidity")
        sound_freq = data.get("sound_freq")

        # Print the received data (You can save or process the data here)
        print(f"Fire Detected: {fire_detected}")
        print(f"Gas Detected: {gas_detected}")
        print(f"Temperature: {temperature}")
        print(f"Humidity: {humidity}")
        print(f"Sound Frequency: {sound_freq}")

        total_ref = db.reference("/sensor_data/totals")
        total_fire_ignited_prev = total_ref.child("total_fire_ignited").get() or 0
        total_gas_detected_prev = total_ref.child("total_gas_detected").get() or 0

        total_fire_ignited = total_fire_ignited_prev + (1 if fire_detected else 0)
        total_gas_detected = total_gas_detected_prev + (1 if gas_detected else 0)

        total_ref.update(
            {
                "total_fire_ignited": total_fire_ignited,
                "total_gas_detected": total_gas_detected,
            }
        )

        timestamp = int(time.time())

        main_ref = db.reference("/data")
        main_ref.update(
            {
                "temperature": temperature,
                "humidity": humidity,
                "fire_detected": fire_detected,
                "gas_detected": gas_detected,
                "sound_frequency": sound_freq,
                "total_fire_ignited": total_fire_ignited,
                "total_gas_detected": total_gas_detected,
                "timestamps": timestamp,
            }
        )

        log_num = db.reference("/sensor_data/log_num")
        log_num_val = log_num.get() or 0

        log_ref = db.reference("/sensor_data/log_data")
        log_ref.update(
            {
                f"data_{log_num_val}": {
                    "temperature": temperature,
                    "humidity": humidity,
                    "fire_detected": fire_detected,
                    "gas_detected": gas_detected,
                    "sound_frequency": sound_freq,
                    "total_fire_ignited": total_fire_ignited,
                    "total_gas_detected": total_gas_detected,
                    "timestamps": timestamp,
                }
            }
        )

        log_num.set(log_num_val + 1)

        logger.info("Event received from ESP")
        return jsonify({"message": "Data received successfully"}), 200
    else:
        logger.error("ESP DATA NOT FOUND or Invalid JSON")
        return jsonify({"error": "Request must be in JSON format"}), 400


app.run(
    host="0.0.0.0",
)
