import json

from matplotlib.pyplot import close

processFunc = lambda s: " ".join(s.split())


path = './debug.json'
jsonfile = open(path,mode="r") 
jsonstr:str = jsonfile.read()
print(jsonstr)
jsonstr = processFunc(jsonstr)

jsonfile = close()
jsonfile = open(path,mode="w") 
jsonfile.write(jsonstr)
jsonfile = close()

  
'''

model.py class CourseRecord
data_import.py func: load_CouRecord
course_views.py func: showCourseRecord downloadCourseRecord
course_utils.py func: cal_participate_num check_post_and_modify


'''