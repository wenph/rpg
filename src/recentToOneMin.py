import datetime
from pymongo import MongoClient

now = datetime.datetime.now()
move_start = datetime.datetime(now.year,now.month,now.day) - datetime.timedelta(days=11)
move_end = datetime.datetime(now.year,now.month,now.day) - datetime.timedelta(days=10)


log_date = move_start

#move counter
i = 0

#connect the mongodb
client = MongoClient('10.32.192.77', 27017)
db = client['test']
source_collection = db['OneMinDB']
target_collection = db['OneMinHistoryDB']

#first hour flag,used to judge either insert or update
flag = {}

#find all doc need to move
for item in source_collection.find({"d":{"$gte":move_start,"$lt":move_end}}).sort("d",1):
    i = i + 1
    n = item["n"]
    k = item["k"]
    d = item["d"]
    values = item["values"]
    flag_key = n + "|" + k
    need_insert = flag.get(flag_key) is None
    if need_insert:
        dv = []
        for hour in range(24):
            hv = []
            for min in range(60):
                hv.append(0.00000001)#pre allocate space
            dv.append({"h":move_start + datetime.timedelta(hours=hour),"hv":hv})
        target_collection.insert({"d":move_start,"n":n,"k":k,"dv":dv})
        flag[flag_key] = 1

    v_array = []
    for min in range(60):
        v_array.append(0.00000001)
    for value in values:
        v_array[value["t"].minute] = value["v"]

    target_collection.update({"d":move_start,"n":n,"k":k},{"$set":{"dv." + str(d.hour) + ".hv":v_array}},multi=True)
