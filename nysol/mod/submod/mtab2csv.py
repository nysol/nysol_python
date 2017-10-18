# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mtab2csv(NysolMOD_CORE):
	kwd = n_core.getparalist("mtab2csv")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtab2csv,self).__init__("mtab2csv",nutil.args2dict(args,kw_args,Nysol_Mtab2csv.kwd))

