import os
from pathlib import Path
from typing import Optional, Union, Literal


if __name__ != '__main__':
    __path = Path(__file__)
    prefix = Path(__path.__str__().removesuffix(__path.name))
    
def list_creds():
    '''
    List all credentials filename
    '''
    folder = list(os.walk(prefix + '_creds'))
    return folder[0][2]


def get_credential(credentials_path, out:Literal['path', 'buffer']) -> Optional[Union[Path, str]]:
    """
    Retrieve the credentials file path or content.
    
    Parameters:
    - credentials_path (str): The relative path of the credentials file without the `.json` extension.
    - out (str): Specify 'path' to get the file path or 'buffer' to read and return the file content.
    
    Returns:
    - Path: If 'out' is 'path', returns the file path as a Path object.
    - str: If 'out' is 'buffer', returns the file content as a string.
    - None: If the file is not found or an invalid 'out' value is provided.
    """
    
    stream_path = prefix.joinpath('_creds/'+credentials_path + '.json')
    try:
        
        if out[0] == 'path' :
            if not stream_path.exists():
                print(f'Credential file not found: {credentials_path}')
                exit()
            return stream_path
        elif out[0] == 'buffer':
            with open(stream_path, 'r') as stream:
                data = stream.read()
            return data
            
        
        
        
    except FileNotFoundError as e:
        raise f'Credential file not found: {credentials_path}'
    