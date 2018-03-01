# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mnumber(NysolMOD_CORE):
	kwd = n_core.getparalist("mnumber")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnumber,self).__init__("mnumber",nutil.args2dict(args,kw_args,Nysol_Mnumber.kwd))

def mnumber(self,*args, **kw_args):
	return Nysol_Mnumber(nutil.args2dict(args,kw_args,Nysol_Mnumber.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnumber", mnumber)
