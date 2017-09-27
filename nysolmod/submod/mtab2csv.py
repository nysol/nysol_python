# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mtab2csv(NysolMOD_CORE):
	kwd = n_core.getparalist("mtab2csv")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtab2csv,self).__init__("mtab2csv",nutil.args2dict(args,kw_args,Nysol_Mtab2csv.kwd))

