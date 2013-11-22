#! /usr/bin/python
# -*- coding: utf-8 -*-
import datetime
import time
from pymongo import MongoClient
import random

start = time.time()
# print start
# 打印格林威治时间的秒数
# print int(start)

#merge 1 hour ago data,utc time
now = datetime.datetime.now()
# 打印日期
# print now

merge_start = datetime.datetime(now.year,now.month,now.day,now.hour) - datetime.timedelta(hours=1)
merge_end = datetime.datetime(now.year,now.month,now.day,now.hour) - datetime.timedelta(hours=0)
print "merge_start:", merge_start
print "merge_end", merge_end

log_date = merge_start

#connect the mongodb
client = MongoClient('10.32.192.77', 27017)
db = client['test']
# source_collection = db['source_data']
target_collection = db['recentNetData']

k_name = ['1.3.6.1.2.2.2.1.1.1', '1.3.6.1.2.2.2.1.1.2', '1.3.6.1.2.2.2.1.1.3', '1.3.6.1.2.2.2.1.1.4', '1.3.6.1.2.2.2.1.1.5',
          '1.3.6.1.2.2.2.1.1.6', '1.3.6.1.2.2.2.1.1.7', '1.3.6.1.2.2.2.1.1.8', '1.3.6.1.2.2.2.1.1.9', '1.3.6.1.2.2.2.1.1.10']

for ipadd_a in range(0, 1):
    for ipadd_b in range(0, 1):
        for ipadd_c in range(0, 7):
            for ipadd_d in range(0, 10):
                n = str(ipadd_a) + '.' + str(ipadd_b) + '.' + str(ipadd_c) + '.' + str(ipadd_d)
                t = int(time.time())
                for k in k_name:
                    v = random.randrange(0,1001)
                    # print n + ':' + str(t) + ':' + k + ':' + str(v)
                    target_collection.insert({"n":n, "t":t, "k":k, "v":v})
                    # time.sleep(0.0006)


