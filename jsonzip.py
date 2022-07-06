import json

from matplotlib.pyplot import close

processFunc = lambda s: " ".join(s.split())



jsonfile = open('./debug.json',mode="r") 
jsonstr:str = jsonfile.read()
print(jsonstr)
jsonstr = processFunc(jsonstr)

jsonfile = close()
jsonfile = open('./debug.json',mode="w") 
jsonfile.write(jsonstr)
jsonfile = close()
