# -*- coding: utf-8 -*-
import shlex
import re

def args2dict(args, kw_args,klist,uk=None):

	if len(args)>1:
		print("arge only one")
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
						print("unkonwn parameter")			
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
			print(args)
			print("args type str or dict")
			return None

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



def arg2dict(args, kw_args,klist,uk=None):

	if len(args)>1:
		print("args only one")
		return None

	if len(args)==1:
		if isinstance(args[0],str) :
			kw_args["cmdstr"] = "'" + args[0] + "'" 

		elif isinstance(args[0],dict):
			kw_args.update(args[0])
		elif isinstance(args[0],tuple):
			pass

		else :
			print(args)
			print("args type str or dict")
			return None

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




