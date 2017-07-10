import json
import re

import requests

from cqhttp import CQHttp

bot = CQHttp(api_root='http://127.0.0.1:5700/')

state = {
    'dump': False
}


@bot.on_event('group_increase')
def handle_group_increase(ctx):
    # noinspection PyBroadException
    try:
        nickname = bot.get_group_member_info(group_id=ctx['group_id'],
                                             user_id=ctx['user_id']).get('nickname')
        name = nickname if nickname else '新人'
        bot.send(ctx, message='欢迎{}～'.format(name), is_raw=True)
    except:
        pass


# noinspection PyBroadException
@bot.on_request('group')
def handle_group_request(ctx):
    print(ctx)
    if ctx['sub_type'] != 'add':
        return

    if ctx['group_id'] == 201865589:
        # CoolQ HTTP API group
        message = ctx['message'].strip()
        if not message:
            return
        m = re.search(r'答案：(.*)$', message)
        if not m:
            return

        message = m.group(1)
        if message.startswith('v'):
            message = message.lstrip('v')

        try:
            resp = requests.get('https://api.github.com/repos/richardchien/coolq-http-api/releases/latest')
            version = resp.json().get('tag_name').lstrip('v')
            if message == version or message == version.replace('.', ''):
                return {'approve': True}
        except:
            return


@bot.on_message()
def handle_message(ctx):
    if state['dump']:
        bot.send(ctx, json.dumps(ctx), is_raw=True)

    if not isinstance(ctx['message'], list):
        return

    message = ctx['message']
    if ctx['message_type'] != 'private':
        first_seg = message[0]
        self_id = bot.get_login_info()['user_id']
        if first_seg['type'] != 'at' or first_seg['data']['qq'] != str(self_id):
            return
        message = message[1:]

    if len(message) == 0 or message[0]['type'] != 'text':
        return

    sp = message[0]['data']['text'].lstrip().split(maxsplit=1)
    if len(sp) < 2:
        return

    cmd, text = sp
    reply = None
    if cmd == 'say':
        reply = say(ctx, text)
    elif cmd == 'dump':
        text = text.strip()
        if text == 'on':
            state['dump'] = True
        elif text == 'off':
            state['dump'] = False

    if reply:
        return {'reply': reply, 'at_sender': False}


# noinspection PyBroadException
def say(ctx, text):
    reply = None
    if text.startswith('['):
        # try json
        try:
            reply = json.loads(text, encoding='utf-8')
        except:
            pass
        if not reply and ctx['user_id'] == 1002647525:
            # try python eval
            try:
                reply = eval(text)  # this is dangerous
            except:
                pass
    if not reply:
        reply = text.replace('&#91;', '[').replace('&#93', ']').replace('&#44', ',').replace('&amp;', '&')

    return reply


bot.run(host='127.0.0.1', port=9000)
