#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import re
import nysol.util.margs as margs
import nysol.mining.mspade as mm

helpMSG="""
概要) 
内容) 
書式) mspade.py i= f= [c=] [cf=] [minSup=|minSupProb=] [maxSize=] 
                [maxLen=] [minGap=] [maxGap=] [maxWin=] 
                ["topk=] [minSize=] [minLen=] [maxSup=] [minPprob=] 
                [oPats=] [oStats=] [oOccs=] [-maximal] [-help]

  i parameter
    i=
    f=
    c=
    cf= 
  e parameter
    minSup=
    minSupProb=
    maxSize=
    maxLen=
    minGap=
    maxGap=
    maxWin=
  o parameter
    -maximal
    topk=
    minSize=
    minLen=
    maxSup=
    minPprob=
    oPats=
    oStats=
    oOccs=

  その他
    -help : ヘルプの表示

備考)


# Copyright(c) NYSOL 2012- All Rights Reserved.
		"""



paraList=[
	["i=","f=","c=","cf="] ,# ipara
	["minSup=","minSupProb=","maxSize=","maxLen=","minGap=","maxGap=","maxWin="] ,# epara
	["-maximal","topk=","minSize=","minLen=","maxSup=","minPprob=","oPats=","oStats=","oOccs="] # opara
]
paraType={
	"i=":"str",
	"f=":"fldstr",
	"c=":"str",
	"cf=":"fldstr",
	"minSup=":"int",
	"minSupProb=":"float",
	"maxSize=":"int",
	"maxLen=":"int",
	"minGap=":"int",
	"maxGap=":"int",
	"maxWin=":"int",
	"-maximal":"bool",
	"topk=":"int",
	"minSize=":"int",
	"minLen=":"int",
	"maxSup=":"int",
	"minPprob=":"float",
	"oPats=":"str",
	"oStats=":"str",
	"oOccs=":"str"
}


paraconvList={
	"i=":"iFile",
	"f=":"iNames",
	"c=":"cFile",
	"cf=":"cNames",
	"-maximal":"maximal"
}

if "-help" in sys.argv:
	print(helpMSG)
	exit()

flat =[]
for para in paraList:
	flat.extend(para)	




args=margs.Margs(sys.argv,",".join(flat),"i=,f=")
##make パラメータ


#ipara
iParams={}
for p in paraList[0] :
	val = None
	if paraType[p] == "int":
		val = args.int(p)	
	elif paraType[p] == "str":
		val = args.str(p)	
	elif paraType[p] == "fldstr":
		val = args.str(p).split(",")
	elif paraType[p] == "float":
		val = args.float(p)	
	elif paraType[p] == "bool":
		val = args.bool(p)	

	if p in paraconvList :
		iParams[paraconvList[p]] = val
	else:
		iParams[re.sub(r'=$',"",p)] = val

eParams={}
for p in paraList[1] :
	val = None
	if paraType[p] == "int":
		val = args.int(p)	
	elif paraType[p] == "str":
		val = args.str(p)	
	elif paraType[p] == "fldstr":
		val = args.str(p).split(",")
	elif paraType[p] == "float":
		val = args.float(p)	
	elif paraType[p] == "bool":
		val = args.bool(p)	

	if p in paraconvList :
		eParams[paraconvList[p]] = val
	else:
		eParams[re.sub(r'=$',"",p)] = val

oParams={}
for p in paraList[2] :
	val = None
	if paraType[p] == "int":
		val = args.int(p)	
	elif paraType[p] == "str":
		val = args.str(p)	
	elif paraType[p] == "fldstr":
		val = args.str(p).split(",")
	elif paraType[p] == "float":
		val = args.float(p)	
	elif paraType[p] == "bool":
		val = args.bool(p)	

	if p in paraconvList :
		oParams[paraconvList[p]] = val
	else:
		oParams[re.sub(r'=$',"",p)] = val


spade=mm.Spade(iParams,eParams,oParams)
rules=spade.run()


