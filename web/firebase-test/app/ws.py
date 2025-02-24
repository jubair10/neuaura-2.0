# # # app.py (Flask WebSocket server)

# # from flask import Flask, request, jsonify
# # from flask_socketio import SocketIO, emit
# # import cv2
# # import base64
# # import numpy as np

# # app = Flask(__name__)
# # socketio = SocketIO(app, cors_allowed_origins="*")

# # # Streaming status
# # streaming = False

# # # Default route
# # @app.route('/')
# # def index():
# #     return "ESP32-CAM Flask Server with WebSocket is Running"

# # # Endpoint to set streaming mode
# # @app.route('/set_mode', methods=['POST'])
# # def set_mode():
# #     global streaming
# #     mode = request.json.get('mode')
# #     if mode in ['stream', 'capture']:
# #         streaming = (mode == 'stream')
# #         return jsonify({"status": "Mode updated", "mode": mode})
# #     return jsonify({"status": "Invalid mode"}), 400

# # # WebSocket for receiving frames
# # @socketio.on('connect')
# # def handle_connect():
# #     print("ESP32 Connected to WebSocket server")

# # @socketio.on('stream')
# # def handle_frame(data):
# #     """ Handle frame from ESP32 """
# #     if streaming:
# #         # Decode frame from ESP32 (base64 string to image)
# #         img_data = base64.b64decode(data)
# #         nparr = np.frombuffer(img_data, np.uint8)
# #         frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

# #         # Display frame (optional for testing)
# #         cv2.imshow("ESP32-CAM Stream", frame)
# #         cv2.waitKey(1)

# #         # Acknowledge frame reception
# #         emit('ack', {'message': 'Frame received'})

# #     else:
# #         print("Streaming stopped. Frame ignored.")

# # @socketio.on('disconnect')
# # def handle_disconnect():
# #     print("ESP32 Disconnected from WebSocket server")

# # if __name__ == '__main__':
# #     socketio.run(app, host='0.0.0.0', port=5001)



# from flask import Flask, jsonify
# from flask_socketio import SocketIO, emit
# import base64
# import numpy as np
# import cv2

# app = Flask(__name__)
# socketio = SocketIO(app, cors_allowed_origins="*")

# streaming = False  # Flag for streaming status

# @app.route('/')
# def index():
#     return "ESP32-CAM Flask Server with WebSocket is Running"

# # WebSocket Event: Frame reception from client (ESP32)
# @socketio.on('frame')
# def handle_frame(data):
#     global streaming
#     if streaming:
#         # Decode the incoming image frame
#         img_data = base64.b64decode(data)
#         nparr = np.frombuffer(img_data, np.uint8)
#         frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

#         # Show the frame
#         cv2.imshow("ESP32-CAM Stream", frame)
#         if cv2.waitKey(1) & 0xFF == ord('q'):
#             pass
        
#         emit('ack', {'message': 'Frame received'})
#     else:
#         print("Streaming stopped. Frame ignored.")

# if __name__ == '__main__':
#     # Run the WebSocket server on port 5001
#     socketio.run(app, host='0.0.0.0', port=5001, )



from fastapi import FastAPI, Response
import cv2

app = FastAPI()

# URL of your ESP32-CAM stream
esp32_cam_url = "http://192.168.0.103:80"
cap = cv2.VideoCapture(esp32_cam_url)

def generate_frames():
    while True:
        ret, frame = cap.read()
        if not ret:
            continue

        # Encode frame as JPEG
        ret, buffer = cv2.imencode('.jpg', frame)
        if not ret:
            continue
        
        # Convert to bytes and yield as part of MJPEG stream
        frame_bytes = buffer.tobytes()

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

@app.get("/video_feed")
def video_feed():
    return Response(generate_frames(), media_type='multipart/x-mixed-replace; boundary=frame')


# import requests

# url = "http://192.168.0.101:5000/esp/mode"

# payload = {"mode":"stream"}


# response = requests.request("POST", url, json=payload)

# print(response.text)
