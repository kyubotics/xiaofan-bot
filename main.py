import json
import sys
import re
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
sp = re.split('\s|:|：', message_str, maxsplit=1)
if len(sp) < 2:
    # not a command, skip it
    exit(0)

cmd = sp[0]
msg = sp[1].strip()

if cmd == '发推':
    if not image_urls:
        requests.post(maker_webhook % 'send_tweet',
                      json={'value1': msg})
    else:
        requests.post(maker_webhook % 'send_tweet_with_image',
                      json={'value1': msg, 'value2': image_urls[0]})
