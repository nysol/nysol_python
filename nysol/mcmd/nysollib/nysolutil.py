# -*- coding: utf-8 -*-
import shlex
import re

def para2strStr(vals):
	rtnStr = ""
	for k, v in vals.items():

		if isinstance(v,bool) :
			if v==True:
				rtnStr += "-" + k + " "

		elif isinstance(v,str) :
			rtnStr += k + "=" + v + " "

		elif isinstance(v,float) :
			rtnStr += k + "=" + str(v) + " "

		elif isinstance(v,int) :
			rtnStr += k + "=" + str(v) + " "

		elif isinstance(v,list) :
			plist = []
			for val in v:
				if isinstance(val,str) :
					plist.append(val)
				elif isinstance(val,float) or isinstance(v,int) :
					plist.append(str(val))
			rtnStr += k + "=" + ",".join(plist) + " "

		elif isinstance(v,dict) :

			plist = []
			for vk, vv in v.items():
				v1=""
				if isinstance(vk,str) :
					v1 = vk
				elif isinstance(vk,float) or isinstance(vk,int) :
					v1 = str(vk)
				v2=""
				if isinstance(vv,str) :
					v2 = vv
				elif isinstance(vv,float) or isinstance(vv,int) :
					v2 = str(vk)
				plist.append(v1+":" +v2)
			rtnStr += k + "=" + ",".join(plist) + " "

	return rtnStr


def para2str(vals): #クラスごとにいれる?

	rtnStr = []
	for k, v in vals.items():

		#特殊パラメータ　runfunc_*
		if k == "runfunc_func" or k == "runfunc_args" or k == "runfunc_kwd" :
			rtnStr.append([k,v])

		elif isinstance(v,bool) :
			if v==True:
				rtnStr.append("-" + k )

		elif isinstance(v,str) :
			rtnStr.append( k+"="+v)

		elif isinstance(v,float) :
			rtnStr.append(k + "=" + str(v))

		elif isinstance(v,int) :
			rtnStr.append(k + "=" + str(v) )

		elif isinstance(v,list) :
			plist = []
			for val in v:
				if isinstance(val,str) :
					plist.append(val)
				elif isinstance(val,float) or isinstance(v,int) :
					plist.append(str(val))

			rtnStr.append(k + "=" + ",".join(plist) )

		elif isinstance(v,dict) :

			plist = []
			for vk, vv in v.items():
				v1=""
				if isinstance(vk,str) :
					v1 = vk
				elif isinstance(vk,float) or isinstance(vk,int) :
					v1 = str(vk)
				v2=""
				if isinstance(vv,str) :
					v2 = vv
				elif isinstance(vv,float) or isinstance(vv,int) :
					v2 = str(vk)
				plist.append(v1+":" +v2)

			rtnStr.append(k + "=" + ",".join(plist) )

		else:
			rtnStr.append([k,v])
							
	return rtnStr



def args2dict(args, kw_args,klist,uk=None):

	if len(args)>1:
		raise TypeError("arge only one")
		return None

	if len(args)==1:
		if isinstance(args[0],str) :
			for para in shlex.split(args[0]):
				val = para.split("=",1)
				if len(val)==1 :
					if val[0][0] == '-':
						kw_args[re.sub(r'^-', "", val[0])] = True
					elif uk!=None:
						kw_args[uk] = val[0]
					else:
						raise TypeError("arge only one")
				elif len(val)==2 :
					kw_args[val[0]] = val[1]
				else:
					print("unkonwn parameter")
		elif isinstance(args[0],dict):
			kw_args.update(args[0])
		elif isinstance(args[0],list) and uk!=None:
			kw_args[uk] = args[0]
		elif isinstance(args[0],tuple):
			pass

		else :
			raise TypeError("args type str or dict")
			#print(args)
			#print("args type str or dict")
			#return None

	# check parameter
	exval = []
	for k,v in kw_args.items():
		if k in klist[0] and isinstance(v,str):
			pass
		elif k in klist[0] :
			pass
		elif k in klist[1] :
			if v== True:
				pass
			else:
				exval.append(k)
		elif k == "tag" or k == "dlog" or k == "sysadd" :
			pass

		else:
			exval.append(k)
			wxc = TypeError(k + " is not keyword")
			raise (wxc)
			
	for k in exval:
		del kw_args[k]

	return kw_args



def arg2dict(args, kw_args,klist,uk=None):

	if len(args)>1:
		print("args only one")
		return None

	if len(args)==1:
		if isinstance(args[0],str) :
			#kw_args["cmdstr"] = "'" + args[0] + "'" 
			kw_args["cmdstr"] = args[0]

		elif isinstance(args[0],dict):
			kw_args.update(args[0])
		elif isinstance(args[0],tuple):
			pass

		else :
			raise TypeError
			#print(args)
			#print("args type str or dict")
			#return None

	# check parameter
	exval = []
	for k,v in kw_args.items():
		if k in klist[0] and isinstance(v,str):
			pass
		elif k in klist[0] :
			pass
		elif k in klist[1] and v== True:
			pass
		elif k == "tag" or k == "dlog" :
			pass
		else:
			exval.append(k)
			print ( k + " is not keyword")
			
	for k in exval:
		del kw_args[k]

	return kw_args



