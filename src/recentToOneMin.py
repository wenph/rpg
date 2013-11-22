#! /usr/bin/python
import datetime
import time
from pymongo import MongoClient

start = time.time()
# print start
# print int(start)

#merge 1 hour ago data,utc time
now = datetime.datetime.now()
# print now


merge_start = datetime.datetime(now.year,now.month,now.day,now.hour) - datetime.timedelta(hours=1)
merge_end = datetime.datetime(now.year,now.month,now.day,now.hour) - datetime.timedelta(hours=0)
print "merge_start:", merge_start
print "merge_end", merge_end

log_date = merge_start

#merge counter
i = 0
#connect the mongodb
client = MongoClient('10.32.192.77', 27017)
db = client['test']
source_collection = db['recentNetData']
target_collection = db['OneMinDB']

#find all doc need to merge
cache = {}
#for item in source_collection.find({"t":{"$gte":merge_start,"$lt":merge_end}}).sort("$natural",-1):
for item in source_collection.find():
    i = i + 1
    n = item["n"]
    k = item["k"]

    #cache key
    key = n + "|" + k
    value = cache.get(key)

    # put in cache
    # value is key's value, it is a list
    if value is None:
        #array = [{"t":item["t"],"v":item["v"],"s":item["s"],"b":item["b"]}]
        array = [{"t":item["t"],"v":item["v"]}]
        cache[key] = array
    else:
        #value.append({"t":item["t"],"v":item["v"],"s":item["s"],"b":item["b"]})
        value.append({"t":item["t"],"v":item["v"]})

query_end = datetime.time()

#batch insert merged doc to target
#all active devices set
# active_devices = set()
for ky in cache.keys():
    if cache[ky] is not None:
        k_array = ky.split("|")
        k_n = k_array[0]
        k_k = k_array[1]
        k_value = cache[ky]
        target_collection.insert({"n":k_n,"k":k_k,"v":k_value})
        # active_devices.add(k_n)

# active_devices_file = open("active_devices","w")
# active_devices_file.truncate(0)
# for active_device in active_devices:
#     active_devices_file.write(active_device + "\n")
# active_devices_file.close()
print "i:", i


