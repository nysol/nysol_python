# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mxml2csv(NysolMOD_CORE):
	kwd = n_core.getparalist("mxml2csv")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mxml2csv,self).__init__("mxml2csv",nutil.args2dict(args,kw_args,Nysol_Mxml2csv.kwd))

