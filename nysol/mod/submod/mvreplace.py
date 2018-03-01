# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mvreplace(NysolMOD_CORE):
	kwd = n_core.getparalist("mvreplace")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvreplace,self).__init__("mvreplace",nutil.args2dict(args,kw_args,Nysol_Mvreplace.kwd))


def mvreplace(self,*args, **kw_args):
	return Nysol_Mvreplace(nutil.args2dict(args,kw_args,Nysol_Mvreplace.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvreplace", mvreplace)
