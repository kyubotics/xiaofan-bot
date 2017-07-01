import requests

import config
from helpers import *


def handle(call):
    go(requests.post,
       'https://ali-east-1.r-c.im:5701/' + call['action'],
       json=call['params'],
       headers={'Authorization': 'token ' + config.token})
