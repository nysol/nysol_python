# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mtrafld(NysolMOD_CORE):
	kwd = n_core.getparalist("mtrafld")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtrafld,self).__init__("mtrafld",nutil.args2dict(args,kw_args,Nysol_Mtrafld.kwd))

def mtrafld(self,*args, **kw_args):
	return Nysol_Mtrafld(nutil.args2dict(args,kw_args,Nysol_Mtrafld.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mtrafld", mtrafld)
