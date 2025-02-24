# import time
# from firebase_admin import db
# from utility.base.firebase_init import initialize_firebase
# import random


# initialize_firebase()

# ref = db.reference("/image_urls")

# new_data = {f"{int(random.random()*1000)}": {"value": 4, "timestamp": "2024-12-10T10:00:00"}}

# new_ref = ref.update(new_data)


# def func(snapshot):
#     print(f"New data: {snapshot.data}")
#     print(f"New data: {snapshot.__dir__()}")
#     # print(f"Previous data: {prev_sibling.val()}")
#     print("---")

import random
import requests

# URL of the server
url = "http://192.168.1.11:5000/esp-data"

# Data to send in the POST request
data = {
    "fire_detected": True,       # Replace with actual data
    "gas_detected": True,       # Replace with actual data
    "temperature": round(random.random()*10,1),         # Replace with actual temperature
    "humidity": 60,              # Replace with actual humidity
    "sound_freq": 100           # Replace with actual sound frequency
}

# Send the POST request
response = requests.post(url, json=data)


if response.status_code == 200:
    print("Request successful!")
    print(response.json())
else:
    
    print("Request failed with status code:", response.status_code)
