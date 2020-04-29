# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_List2Csv(NysolMOD_CORE):

	_kwd = [["i","o","header"],[]]
	_inkwd  = ["i"] 
	_outkwd = ["o"]

	def __init__(self,*args, **kw_args) :

		if len(args) != 1 :
			print("arge only one")
			return None

		if isinstance(args[0],list) :
			kw_args["i"] = args[0]
		else :
			print("unsuport type")
			return None
			
		if not isinstance(kw_args["i"],list) :
			print("unsuport type")
			return None

		# headerがあればmcut 		
		if "header" in kw_args :

			if isinstance(kw_args["header"],list) :
				fld =[]
				for i,v in enumerate(kw_args["header"]):
					fld.append("%d:%s"%(i,v))
				
				kw_args["f"] = fld
	
			elif isinstance(kw_args["header"],str) :
				hd = re.split(r",", kw_args["header"])
				for i,v in enumerate(hd):
					fld.append("%d:%s"%(i,v))
			else :
				print("unsuport type")
				return None

			kw_args["nfni"] = True
			del kw_args["header"]
			super(Nysol_List2Csv,self).__init__("mcut",kw_args)

		else:
			kw_args["f"] = "*"		
			super(Nysol_List2Csv,self).__init__("mcut",kw_args)

