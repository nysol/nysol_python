# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Marff2csv(NysolMOD_CORE):
	kwd = n_core.getparalist("marff2csv")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Marff2csv,self).__init__("marff2csv",nutil.args2dict(args,kw_args,Nysol_Marff2csv.kwd))

