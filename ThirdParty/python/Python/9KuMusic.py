# -*- coding: UTF-8 -*-

import requests
import math
import json
from bs4 import BeautifulSoup


def getMusicId(url):
    pos = url.find('.htm')
    if pos > 0:
        sub_lists = url[0: pos].split('/')
        music_id = sub_lists[len(sub_lists) - 1]
        return int(music_id)
    return 0


def getMusicUrl(song, list_song_jsons):
    list_song_as = song.find_all('a', class_='songName')
    for song_a in list_song_as:
        url = getMusicId(song_a.attrs['href'])
        print(url)
        list_song_jsons.append(url)

#    return list_song_jsons


def getMusicList(url):
    req = requests.get('http://www.9ku.com/' + url)
    if 200 != req.status_code:
        return False
    content = req.content.decode()
    soup = BeautifulSoup(content, 'html5lib')
    list_sond_blocks = soup.find_all('div', class_='songList clearfix')
    if -1 == list_sond_blocks:
        list_sond_blocks = soup.find_all('div', class_='songList songList960 clearfix')
    list_song_jsons = []
    for sond_block in list_sond_blocks:
        getMusicUrl(sond_block, list_song_jsons)
    return json.dumps(list_song_jsons)


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


def getMusicContent():
    list_type = getMusicType('http://www.9ku.com')
    return json.dumps(list_type)


def testArgs(s1, s2, s3):
    print(s1, s2, s3)
    return "success"



def main():
    list_type = getMusicList('music/t_new.htm')
    return json.dumps(list_type)
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

if "__main__" == __name__:
    # http://www.9ku.com/play/473477.htm
 #   music_id = getMusicId('http://www.9ku.com/play/1003606.htm')
 #   print(getMusicInfo(music_id))
    main()
'''
    return
    list_type = getMusicType('http://www.9ku.com/')
    print(li)
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
'''
    #print(getMusicList('http://www.9ku.com/music/t_new.htm'))
