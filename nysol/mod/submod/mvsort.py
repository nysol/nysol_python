# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mvsort(NysolMOD_CORE):
	kwd = n_core.getparalist("mvsort")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvsort,self).__init__("mvsort",nutil.args2dict(args,kw_args,Nysol_Mvsort.kwd))


def mvsort(self,*args, **kw_args):
	return Nysol_Mvsort(nutil.args2dict(args,kw_args,Nysol_Mvsort.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvsort", mvsort)

