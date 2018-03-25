# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mmvavg(NysolMOD_CORE):
	kwd = n_core.getparalist("mmvavg")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mmvavg,self).__init__("mmvavg",nutil.args2dict(args,kw_args,Nysol_Mmvavg.kwd))


def mmvavg(self,*args, **kw_args):
	return Nysol_Mmvavg(nutil.args2dict(args,kw_args,Nysol_Mmvavg.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mmvavg", mmvavg)
