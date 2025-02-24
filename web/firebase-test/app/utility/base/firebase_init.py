import firebase_admin
from firebase_admin import credentials
from creds.base import get_credential
import dotenv
dotenv.load_dotenv('.env')

def initialize_firebase():
    creds_key = dotenv.get_key('.env','FIREBASE_CREDS')
    if not creds_key:
        raise "Credential's name is not incorrect !!!"
    
    creds_path = get_credential(creds_key,out=['path'])
        
    cred = credentials.Certificate(creds_path)
    firebase_admin.initialize_app(cred, {
        'storageBucket': dotenv.get_key('.env', 'BUCKET_ID'),  # Replace with your Firebase Storage Bucket ID
        'databaseURL': dotenv.get_key('.env', 'DATABASE_URL')
    })