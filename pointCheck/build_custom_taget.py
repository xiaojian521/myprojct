#!/usr/bin/env python
#-*- coding: utf-8 -*-

import os, re, sys, xlwt, xlrd, xlutils, datetime
from xlutils.copy import copy

#=======================================================================================
def checkExcludePathInner(directory, projectPath):
    if directory.find(projectPath + "/build") != -1 or \
    directory.find(projectPath + "/board") != -1 or \
    directory.find(projectPath + "/out") != -1 or \
    directory.find(projectPath + "/.repo") != -1 or \
    directory.find(projectPath + "/device") != -1:
        return True

    if directory.find('test') != -1 or directory.find('Test') != -1 \
       or directory.find('TEST') != -1 or directory.find('tests') != -1:
        return True
    if directory.find('.git') != -1:
        return True
    return False

#=======================================================================================
def checkExcludePath(directory):
    if directory.find("/home/xiaojian/workplace/17Cy") != -1:
	return checkExcludePathInner(directory, "/home/xiaojian/workplace/17Cy")
    else:
        print ("invalid directory : %s") % directory
        sys.exit()
	
#=======================================================================================
def genCheckMap(sourcePath, nameMap):
    
    checkMap = {}
    
    for root, rootdirs, files in os.walk(sourcePath):
	if checkExcludePath(root):
	    continue
	
	actor = '' 
	vendorname = ''
	
	for key, value in nameMap.items():
	    repositoryNamePos = key.find("17Cy")
	    if repositoryNamePos == -1:
		continue
	    repositoryName = key[repositoryNamePos+4:]
	    if root.find(repositoryName) != -1:
		actor = value
		vendorname = key
	if '' == actor:
	    continue
	for filename in files:
	    if not filename.endswith('.mk'):
       		continue
	    
	    flag = False
	    lineNum = 0
	    localmoudul = ''
	    
	    pathname = os.path.join(root, filename)
	    lines = open(pathname, 'rd').readlines()

	    for line in lines:
		line = line.strip().replace(' ', '')
       		lineNum += 1
		if line.startswith('#'):
		    continue
		if line.find('CLEAR_VARS') == -1:
		    flag = True
		if True == flag:
		    if line.startswith('LOCAL_MODULE:='):
			localmoudul = line[line.find('=')+1:]
			if localmoudul == '':
			    print ("do not have localmodule value")
			    exit(1)
		    if line.find('BUILD_') != -1:
			if line.find('BUILD_CUSTOM_TARGET') != -1:
			    if not checkMap.has_key(vendorname):
				checkMap[vendorname] = [('BUILD_CUSTOM_TARGET', lineNum, localmoudul, actor)]
			    else:
				checkMap[vendorname].append(('BUILD_CUSTOM_TARGET', lineNum, localmoudul, actor))			
			flag = False
			continue
    return checkMap

#=======================================================================================
def readExcel(excelName):
    
    repositoryAndNameMap = {}
    
    workbook = xlrd.open_workbook(excelName)
    table = workbook.sheets()[4]
    sheetName = workbook.sheet_names()[4]
    nrows = table.nrows
    for rowtable in range(nrows):
	if rowtable < 3:
	    continue
	#====change=====
	if table.cell(rowtable, 8).value.strip() == '':
	    continue   
	repositoryAndNameMap[table.cell(rowtable, 0).value.strip()] =\
	    table.cell(rowtable, 4).value.strip()
    return repositoryAndNameMap

#=======================================================================================
def writeExcel(checkMap,effectiveExcel):
    now_time = datetime.datetime.now().strftime('%Y-%m-%d')
    file = xlwt.Workbook(encoding='utf-8')
    sheet = file.add_sheet('t1t2plus')
    sheet.write(0, 0, 'branch')
    sheet.write(0, 1, '17cyplus/master/t1t2plus')
    sheet.write(1, 0, '仓库名')
    sheet.write(1, 1, '点检内容')
    sheet.write(1, 2, '行号')
    sheet.write(1, 3, 'LOCAL_MODULE')
    sheet.write(1, 4, '担当')
    sheet.write(1, 5, '点检日期')
    sheet.write(1, 6, '是否有问题')
    sheet.write(1, 7, '点检完成日期 ')
    sheet.write(1, 8, '修正日期')
    sheet.write(1, 9, '备考')
    row = 1 
    for key, values in checkMap.items():
	for value in values:
	    row += 1
	    sheet.write(row, 0, key)
	    sheet.write(row, 1, value[0])
	    sheet.write(row, 2, value[1])
	    sheet.write(row, 3, 'LOCAL_MODULE := ' + value[2])
	    sheet.write(row, 4, value[3])	    
	    sheet.write(row, 5, now_time)
    file.save(effectiveExcel)

#=======================================================================================
def wirteAddExcel(checkMap,effectiveExcel):
    
    now_time = datetime.datetime.now().strftime('%Y-%m-%d')
    
    workbook = xlrd.open_workbook(effectiveExcel)
    excel = copy(workbook) #将xlrd的对象转化为xlwt的对象
    #====change=====
    sheet = excel.get_sheet(14) #获取要操作的sheet
    sheet.write(0, 0, 'branch')
    #====change=====
    sheet.write(0, 1, '17Cy/MileStone/17CY_T1T2_Stable_278B')
    sheet.write(1, 0, '仓库名'.decode('utf-8'))
    sheet.write(1, 1, '点检内容'.decode('utf-8'))
    sheet.write(1, 2, '行号'.decode('utf-8'))
    sheet.write(1, 3, 'LOCAL_MODULE'.decode('utf-8'))
    sheet.write(1, 4, '担当'.decode('utf-8'))
    sheet.write(1, 5, '点检日期'.decode('utf-8'))
    sheet.write(1, 6, '是否有问题'.decode('utf-8'))
    sheet.write(1, 7, '点检完成日期 '.decode('utf-8'))
    sheet.write(1, 8, '修正日期'.decode('utf-8'))
    sheet.write(1, 9, '备考'.decode('utf-8'))
    #row = workbook.sheets()[2].nrows
    row = 1
    for key, values in checkMap.items():
	for value in values:
	    row += 1
	    sheet.write(row, 0, key)
	    sheet.write(row, 1, value[0])
	    sheet.write(row, 2, value[1])
	    sheet.write(row, 3, 'LOCAL_MODULE := ' + value[2])
	    sheet.write(row, 4, value[3])	    
	    sheet.write(row, 5, now_time)
    excel.save(effectiveExcel)

#=======================================================================================
def main(sourcepath, excelNamepath, effectiveExcel):
    nameMap = readExcel(excelNamepath)
    checkMap = genCheckMap(sourcepath,nameMap)
    if os.path.exists(effectiveExcel):
	wirteAddExcel(checkMap,effectiveExcel)
    else:
	writeExcel(checkMap,effectiveExcel)

#=======================================================================================
if __name__ == "__main__":
    if not (len(sys.argv) == 1) :
        print('param error exit!')
        exit(1)
    #sourcepath = sys.argv[1]  #/home/xiaojian/workplace/17Cy
    #testexcelpath = sys.argv[2]  #/home/xiaojian/testexcle.xlsx
    #main(sourcepath, localexcelpath) 
    main('/home/xiaojian/workplace/17Cy', '/home/xiaojian/iAuto项目二进制发布处理日程表及进度更新.xlsx', '/home/xiaojian/build_custom_target.xls') 
    
#=======================================================================================
    

