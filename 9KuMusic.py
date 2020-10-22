# -*- coding: UTF-8 -*-

import requests
import math
import json
import threadpool
import multiprocessing
from bs4 import BeautifulSoup


def getMusicLrcContent(html):
    soup = BeautifulSoup(html, "html5lib")
    lrc_content = soup.find('textarea', id='lrc_content')
    return lrc_content.text.replace('\n', ' ')


def getMusicId(url):
    pos = url.find('.htm')
    if pos > 0:
        sub_lists = url[0: pos].split('/')
        music_id = sub_lists[len(sub_lists) - 1]
        return int(music_id)
    return 0


def getMusicInfo(music_id):
    path = math.floor(music_id / 1000) + 1
    url = 'http://www.9ku.com/html/playjs/{0}/{1}.js'.format(path, music_id)
    req = requests.get(url)
    if 200 != req.status_code:
        return False
    content = req.content.decode()
    song_josn = json.loads(content[1:-1])
    data = {'name': song_josn['mname'], 'singer': song_josn['singer'], 'url': song_josn['wma'], 'id': song_josn['id']}
    return data


def getMusicUrl(song):
    list_song_as = song.find_all('a', class_='songName')
    list_song_jsons = []
    for song_a in list_song_as:
        music_id = getMusicId(song_a.attrs['href'])
        data = getMusicInfo(music_id)
        if data is not False:
            list_song_jsons.append(data)
    return list_song_jsons


def getMusicList(url, index):
    req = requests.get(url)
    if 200 != req.status_code:
        return False
    content = req.content.decode()
    soup = BeautifulSoup(content, 'html5lib')
    list_sond_blocks = soup.find_all('div', class_='songList clearfix')
    if -1 == list_sond_blocks:
        list_sond_blocks = soup.find_all('div', class_='songList songList960 clearfix')
    list_song_jsons = []
    count = len(list_sond_blocks)
    i = 0
    for sond_block in list_sond_blocks:
        print('完成:{}%'.format(i/count * 10 + index))
        list_song_jsons.append(getMusicUrl(sond_block))
        i += 1
    return list_song_jsons


def getMusicType(url):
    req = requests.get(url)
    if 200 != req.status_code:
        return False
    content = req.content.decode()
    soup = BeautifulSoup(content, 'html5lib')
    ul = soup.find('ul', class_='navList clearfix')
    list_type = []
    for li in ul:
        tag = li.find('a')
        if -1 != tag:
            data = {'url': tag.attrs['href'], 'type': tag.string}
            list_type.append(data)

    return list_type[1:-1]

if "__main__" == __name__:
    # http://www.9ku.com/play/473477.htm
#    music_id = getMusicId('http://www.9ku.com/play/1003606.htm')
#    getMusicInfo(music_id)
    list_type = getMusicType('http://www.9ku.com/')
    i = 0
    count = len(list_type)
    list_data = []
    for song_type in list_type:
        j = i / count * 100
        print('完成:{}%'.format(j))
        print('get type', song_type['type'])
        content = getMusicList('http://www.9ku.com/' + song_type['url'], j)
        data = {'type': song_type['type'], 'content': content}
        list_data.append(data)
        i += 1
    print('finish----------------->')
    print(list_data)
    #print(getMusicList('http://www.9ku.com/music/t_new.htm'))
