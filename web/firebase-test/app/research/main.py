# import random
# import time
# import json
# from datetime import datetime
from utility.base.firebase_init import initialize_firebase
# from firebase_admin import db

# # Initialize Firebase
initialize_firebase()

# # Reference to Firebase Realtime Database path
# ref = db.reference('/sensor_data')

# # Function to generate fake sensor data with timestamps
# def generate_fake_sensor_data():
#     # Randomly generating temperature and humidity
#     temperature = round(random.uniform(20.0, 30.0), 2)  # Random temperature between 20 and 30°C
#     humidity = random.randint(40, 80)  # Random humidity between 40 and 80%
    
#     # Get the current timestamp in ISO 8601 format
#     timestamp = datetime.now().isoformat()  # Format: "2024-12-15T16:00:00"

#     # Creating a dictionary for the sensor data (JSON-like format)
#     sensor_data = {
#         "temperature": temperature,
#         "humidity": humidity,
#         "timestamp": timestamp
#     }
    
#     return sensor_data

# # Function to populate Firebase with fake sensor data
# def populate_data():
#     # Populating 20 entries of fake data
#     for i in range(1, 21):  # Populating 20 data entries
#         fake_data = generate_fake_sensor_data()
#         data_path = f"/sensor_data/data_{i}"  # Firebase path for each data entry
#         ref.update({data_path: fake_data})  # Update Firebase with the JSON-like data
#         print(f"Added: {fake_data}")
#         time.sleep(1)  # Delay to simulate real-time data feed

# # Function to retrieve the most recent 10 data entries
# # def get_recent_data():
# #     print("Fetching recent 10 data...")

# #     # Fetch all data from Firebase
# #     all_data = ref.get()

# #     # Check if data exists
# #     if all_data:
# #         # Retrieve and sort data by timestamp
# #         # Sort the data based on the 'timestamp' value in descending order
# #         sorted_data = sorted(all_data.items(), key=lambda x: x[1], reverse=True)[0][1]
# #         # Extract the most recent 10 data entries
# #         recent_data = []
# #         for i in range(min(10, len(sorted_data))):  # Limit to 10 entries
# #             recent_data.append(sorted_data[i][1])  # Extract only the data (value, not key)
        
# #         # Return the list of most recent data entries
# #         return recent_data
# #     else:
# #         print("No data found!")
# #         return []


# def get_recent_data():
#     print("Fetching recent 10 data...")

#     # Fetch all data ordered by timestamp
#     all_data = ref.order_by_child('timestamp').limit_to_last(10).get()

#     # Check if data exists
#     if all_data:
#         recent_data = []
#         for item in all_data.values():  # Firebase returns data as a dictionary
#             recent_data.append(item)  # Append the data to the list
        
#         # Return the list of most recent data entries
#         return recent_data
#     else:
#         print("No data found!")
#         return []
# # Example: Populating data and retrieving the most recent 10 entries
# # populate_data()  # Populate Firebase with fake sensor data (can be run once)
# recent_data = get_recent_data()  # Fetch the most recent 10 entries from Firebase

# # Print the recent data
# for entry in recent_data:
#     print(entry)


# import socket

# socket.create_server()



import firebase_admin
from firebase_admin import credentials, db
import random
import time

# Initialize Firebase Admin SDK

# Function to populate /data node
def populate_data():
    i = 1
    while True:
        try:
            ref = db.reference('/data')
            
            sample_data = {
                f"entry{i}": {
                    "timestamp": int(time.time()),
                    "temperature": round(random.uniform(20.0, 35.0), 2),
                    "humidity": round(random.uniform(30.0, 70.0), 2),
                    "status": "ok"
                }
            }
            
            # Push data to Firebase
            ref.update(sample_data)
            print("Data successfully populated!")
            
            time.sleep(2)

        except Exception as e:
            print(f"Error populating data: {e}")
        i = i + 1

# Main execution
populate_data()




