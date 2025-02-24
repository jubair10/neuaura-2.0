import os
from pathlib import Path
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)-s %(levelname)-s %(message)s')

# File List
list_of_files = [
    'main.py',
    # app
    'app/api.py',
    'app/database.py',

    'utility/__init__.py',
    'utility/base/firebase_init.py',
    
    # 'utility/esp_cam
    'utility/esp_cam/__init__.py',
    'utility/esp_cam/logic.py',
    'utility/esp_cam/send_img.py',
    'utility/esp_cam/fetch_img.py',
    # logging
    "logger/__init__.py",
    "logger/logger_config.py",
    
    # folders
    'creds/__init__.py',
    'creds/base.py'
]



for file_path in list_of_files:
    file_path = Path(file_path)
    filedir, filename = os.path.split(file_path)
    
    if filedir != "":
        os.makedirs(filedir, exist_ok=True)
        logging.info(f"Creating directory: {filedir} for the file {filename}")

    if not os.path.exists(file_path) or (os.path.getsize(file_path) == 0):
        with open(file_path, "w") as f:
            f.close()
            logging.info(f"Creating empty file: {file_path}")
            
    else:
        logging.info(f"{filename} already exists")
