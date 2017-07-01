import requests

import config


def handle(call):
    requests.post('https://ali-east-1.r-c.im:5701/' + call['action'],
                  json=call['params'],
                  headers={'Authorization': 'token ' + config.token})
