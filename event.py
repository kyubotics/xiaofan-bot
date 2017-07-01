import re
import requests

import config
import api_call
from helpers import *


def handle(ev):
    if ev['post_type'] != 'message':
        return

    message = ev['message']
    message_str = ''
    image_urls = []
    for part in message:
        if part['type'] == 'text':
            message_str += ' ' + part['data']['text']
        elif part['type'] == 'emoji':
            message_str += chr(int(part['data']['id']))
        elif part['type'] == 'image':
            url = part['data'].get('url')
            if url:
                image_urls.append(url)

    message_str = message_str.strip()
    sp = re.split('\s|:|：', message_str, maxsplit=1)
    if len(sp) < 2:
        # not a command, skip it
        return

    cmd = sp[0]
    msg = sp[1].strip()

    if cmd == '发推':
        if str(ev['user_id']) != str(config.super_id):
            return
        go(requests.post, config.maker_webhook % 'send_tweet', json={'value1': msg})
        api_call.handle({
            'action': 'send_private_msg',
            'params': {
                'user_id': ev['user_id'],
                'message': '发好了[CQ:face,id=21]'
            }
        })
    elif cmd == '发微博':
        if str(ev['user_id']) != str(config.super_id):
            return
        go(requests.post, config.maker_webhook % 'send_weibo', json={'value1': msg})
        api_call.handle({
            'action': 'send_private_msg',
            'params': {
                'user_id': ev['user_id'],
                'message': '好啦[CQ:face,id=21]'
            }
        })
