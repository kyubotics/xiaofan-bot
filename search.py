from concurrent.futures import ThreadPoolExecutor

import requests
from cqhttp import CQHttp

bot = CQHttp(api_root='http://127.0.0.1:5700/')

executor = ThreadPoolExecutor(max_workers=20)
session = requests.Session()


def search_and_reply(ctx, keyword):
    session.headers.update({
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3159.6 Safari/537.36',
        'Referer': 'http://www.pansou.com/',
        'Connection': 'keep-alive',
    })
    resp = session.get(
        'http://api.pansou.com/search_new.php',
        params={
            'callback': 'jQuery172042529061511397237_1500990957874',
            'q': keyword,
            'p': 1,
        },
    )
    if resp and resp.status_code == 200:
        data = resp.json().get('list', {}).get('data')
        if not data:
            bot.send(ctx, '没搜到 ' + keyword)
        else:
            bot.send(ctx, '\r\n'.join(
                ['{title}：{link}'.format(**item) for item in data[:5]]))


@bot.on_message()
def handle_message(ctx):
    text = ''.join([part['data']['text'] for part in ctx['message']
                    if part['type'] == 'text'])
    if not text.startswith('搜'):
        return

    keyword = text.lstrip('搜').strip()
    executor.submit(search_and_reply, ctx=ctx, keyword=keyword)


bot.run(host='127.0.0.1', port=8080)
executor.shutdown(wait=True)
