# -*- coding: UTF-8 -*-

import requests
import math
import json


def getMusicInfo(music_id):
    path = math.floor(music_id / 1000) + 1
    url = 'http://www.9ku.com/html/playjs/{0}/{1}.js'.format(path, music_id)
    req = requests.get(url)
    if 200 != req.status_code:
        return False
    content = req.content.decode()
    song_josn = json.loads(content[1:-1])
    print(song_josn)
    data = {'name': song_josn['mname'], 'singer': song_josn['singer'], 'url': song_josn['wma'], 'id': song_josn['id']}
    return json.dumps(data)


if __name__ == 'main':
    getMusicInfo()