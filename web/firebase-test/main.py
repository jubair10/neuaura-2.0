
import firebase_admin
from firebase_admin import credentials, db
import time

from .app.utility.base.firebase_init import initialize_firebase

firebase_init.initialize_firebase()

# Path to your Firebase Admin SDK JSON file
# cred = credentials.Certificate("./project-aurixeon-firebase-adminsdk-wm66z-ca0d0a1fa7.json")

# Initialize Firebase Admin SDK
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://project-aurixeon-default-rtdb.firebaseio.com'
})

ref = db.reference('/image_urls')

# Store the last fetched value to detect changes
last_value = None
#  Data to append (can be any data type: dictionary, list, etc.)
new_data = {
    'value': 4,
    'timestamp': '2024-12-10T10:00:00'
}

# Append data by using push() method
new_ref = ref.push(new_data)

print(f"Data appended successfully. New data key: {new_ref.key}")
# while True:
#     # Fetch the data periodically
#     data = ref.get()

#     if data != last_value:
#         print("New data received:", data)
#         last_value = data  # Update the last value

#     time.sleep(2)  # Wait for 2 seconds before checking for updates again


