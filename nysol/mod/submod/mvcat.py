# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mvcat(NysolMOD_CORE):
	kwd = n_core.getparalist("mvcat")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvcat,self).__init__("mvcat",nutil.args2dict(args,kw_args,Nysol_Mvcat.kwd))

def mvcat(self,*args, **kw_args):
	return Nysol_Mvcat(nutil.args2dict(args,kw_args,Nysol_Mvcat.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvcat", mvcat)
