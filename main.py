import json
import sys
import requests

json_file = sys.argv[1]
token = sys.argv[2]

with open(json_file, 'r', encoding='utf-8') as f:
    event = json.loads(f.read())

if event['post_type'] == 'message':
    requests.get('https://ali-east-1.r-c.im:5701/send_private_msg',
                 headers={'Authorization': 'token ' + token},
                 params={'user_id': event['user_id'], 'message': event['message']})
