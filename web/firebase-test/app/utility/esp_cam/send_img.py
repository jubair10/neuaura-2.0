import firebase_admin
from firebase_admin import credentials, storage, db
import os

# Initialize Firebase with the credentials from the serviceAccountKey.json file

# Function to upload an image to Firebase Storage
def upload_image_to_firebase(image_path, storage_path):
    try:
        # Ensure Firebase is initialized
        if not firebase_admin._apps:
            initialize_firebase()

        # Get a reference to the Firebase Storage bucket
        bucket = storage.bucket()

        # Create a blob (reference) for the image in Firebase Storage
        blob = bucket.blob(storage_path)

        # Upload the image from the local file system
        blob.upload_from_filename(image_path)

        # Optionally, make the file publicly accessible
        blob.make_public()

        # Return the public URL of the uploaded image
        return blob.public_url

    except Exception as e:
        print(f"Error uploading image: {e}")
        return None

def fetch_image_urls_and_save_to_rtdb():
    try:
        # Initialize Firebase if not already done
        # if not firebase_admin._apps:
        #     initialize_firebase()

        # Get a reference to Firebase Storage bucket
        bucket = storage.bucket()

        # Get a reference to Firebase Realtime Database
        ref = db.reference('/images')  # Path in RTDB where you want to store image URLs

        # Fetch all image URLs manually (you can extend this to automate it if needed)
        image_paths = [
            'images/image.png',
            # 'images/image2.jpg',
            # 'images/image3.jpg'  # List all your image paths here
        ]

        # Create a dictionary to store image URLs
        image_urls = {}

        # Fetch the public URLs for each image
        for image_path in image_paths:
            blob = bucket.blob(image_path)
            image_url = blob.public_url
            clean_key = image_path.split('/')[-1].split('.')[0]
            image_urls[clean_key] = image_url.removeprefix('https://')

        print(image_urls)
        ref.set(image_urls)

        print("Image URLs saved to Realtime Database successfully!")

    except Exception as e:
        print(f"Error fetching image URLs and saving to RTDB: {e}")

# Example usage
if __name__ == "__main__":
    fetch_image_urls_and_save_to_rtdb()
# Example usage of the upload function
# if __name__ == "__main__":
#     image_path = 'image.png'  # Local image path
#     storage_path = 'images/image.png'     # Path where you want to store the image in Firebase Storage

#     # Upload image and get public URL
#     image_url = upload_image_to_firebase(image_path, storage_path)

#     if image_url:
#         print(f"File uploaded successfully! Public URL: {image_url}")
#     else:
#         print("Failed to upload image.")
