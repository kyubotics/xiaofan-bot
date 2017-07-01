import re
import requests

import config
import api_call


def handle(ev):
    if ev['post_type'] != 'message':
        return

    message = ev['message']
    message_str = ''
    for part in message:
        if part['type'] == 'text':
            message_str += ' ' + part['data']['text']

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
        requests.post(config.maker_webhook % 'send_tweet', json={'value1': msg})
        api_call.handle({
            'action': 'send_private_msg',
            'params': {
                'user_id': ev['user_id'],
                'message': '已经通知 IFTTT 了，应该稍等一会儿就会发送'
            }
        })
