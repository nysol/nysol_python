# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_List2Csv(NysolMOD_CORE):
	_kwd ,_inkwd,_outkwd = n_core.getparalist("readlist",3)

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
		
		if "header" in kw_args :
			if isinstance(kw_args["header"],list) :
				if len(kw_args["i"])==0 or len(kw_args["header"]) == len(kw_args["i"][0]):
					kw_args["i"].insert(0,kw_args["header"])
				else :
					print("unmatch size")
					return None
	
			elif isinstance(kw_args["header"],str) :
				hd = re.split(r",", kw_args["header"])
				if len(kw_args["i"])==0 or len(kw_args["header"]) == len(hd):
					kw_args["i"].insert(0,hd)
				else :
					print("unmatch size")
					return None

			else :
				print("unsuport type")
				return None

			del kw_args["header"]

		super(Nysol_List2Csv,self).__init__("readlist",nutil.args2dict((),kw_args,Nysol_List2Csv._kwd))

