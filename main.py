import json
import sys
import re
import requests

data_file = sys.argv[1]

with open(data_file, 'r', encoding='utf-8') as f:
    data = json.loads(f.read())

super_id = data['super_id']
token = data['token']
event = data['event']
maker_webhook = data['maker_webhook']

if event['post_type'] != 'message':
    exit(0)

message = event['message']
message_str = ''
for part in message:
    if part['type'] == 'text':
        message_str += ' ' + part['data']['text']

message_str = message_str.strip()
sp = re.split('\s|:|：', message_str, maxsplit=1)
if len(sp) < 2:
    # not a command, skip it
    exit(0)

cmd = sp[0]
msg = sp[1].strip()

if cmd == '发推':
    if str(event['user_id']) != str(super_id):
        exit(0)

    requests.post(maker_webhook % 'send_tweet', json={'value1': msg})
