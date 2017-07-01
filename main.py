import json
import sys
import requests

data_file = sys.argv[1]

with open(data_file, 'r', encoding='utf-8') as f:
    data = json.loads(f.read())

token = data['token']
event = data['event']
maker_webhook = data['maker_webhook']

if event['post_type'] != 'message':
    exit(0)

message = event['message']
message_str = ''
image_urls = []
for part in message:
    if part['type'] == 'text':
        message_str += ' ' + part['data']['text']
    elif part['type'] == 'image':
        url = part['data'].get('url')
        if url:
            image_urls.append(url)

message_str = message_str.strip()

requests.post('https://ali-east-1.r-c.im:5701/send_private_msg',
              headers={'Authorization': 'token ' + token},
              json={'user_id': event['user_id'], 'message': message_str})

