# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mselrand(NysolMOD_CORE):
	kwd = n_core.getparalist("mselrand")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mselrand,self).__init__("mselrand",nutil.args2dict(args,kw_args,Nysol_Mselrand.kwd))

def mselrand(self,*args, **kw_args):
	return Nysol_Mselrand(nutil.args2dict(args,kw_args,Nysol_Mselrand.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mselrand", mselrand)
