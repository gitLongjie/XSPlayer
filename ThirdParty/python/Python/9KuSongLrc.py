# -*- coding: UTF-8 -*-

import requests
import math
import json
from bs4 import BeautifulSoup


def getMusicLrcContent(song_id):
    url = "http://www.9ku.com/play/{}.htm".format(song_id)
    req = requests.get(url)
    if 200 != req.status_code:
        return ''
    content = req.content.decode()
    soup = BeautifulSoup(content, "html5lib")
    lrc_content = soup.find('textarea', id='lrc_content')
    return lrc_content.text


def getMusicId(url):
    dot_pos = url.rfind('.')
    line_pos = url.rfind('/')
    if dot_pos > 0 and line_pos > 0:
        music_id = url[line_pos + 1: dot_pos]
        return int(music_id)
    return 0


def getMusicLrc(url):
    music_id = getMusicId(url)
    return getMusicLrcContent(music_id)


def Test():
    # http://mp32.9ku.com/upload/128/2020/08/12/1008304.mp3
    music_id = getMusicId('http://mp32.9ku.com/upload/128/2020/08/12/1008304.mp3')
    print(getMusicLrcContent(music_id))


if __name__ == '__main__':
    Test()
