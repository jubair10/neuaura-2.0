import cv2

# url = r'http://192.168.1.2:4747/video'
url = r'http://192.168.1.10/stream'

cap = cv2.VideoCapture(url)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break



