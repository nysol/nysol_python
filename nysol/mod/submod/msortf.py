# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Msortf(NysolMOD_CORE):
	kwd = n_core.getparalist("msortf")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msortf,self).__init__("msortf",nutil.args2dict(args,kw_args,Nysol_Msortf.kwd))


def msortf(self,*args, **kw_args):
	return Nysol_Msortf(nutil.args2dict(args,kw_args,Nysol_Msortf.kwd)).addPre(self)

setattr(NysolMOD_CORE, "msortf", msortf)
