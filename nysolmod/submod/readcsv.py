# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Readcsv(NysolMOD_CORE):
	kwd = n_core.getparalist("readcsv")
	def __init__(self,*args, **kw_args) :
		if len(args) != 1 :
			print("arge only one")
			return None

		if isinstance(args[0],str) :
			kw_args["i"] = args[0]
		elif isinstance(args[0],list) :
			kw_args["i"] = ','.join(args[0])
		else :
			print("unsuport type")
			return None
			
		super(Nysol_Readcsv,self).__init__("readcsv",nutil.args2dict((),kw_args,Nysol_Readcsv.kwd))

