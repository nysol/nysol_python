# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mmvavg(NysolMOD_CORE):
	_kwd = n_core.getparalist("mmvavg")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mmvavg,self).__init__("mmvavg",nutil.args2dict(args,kw_args,Nysol_Mmvavg._kwd))


def mmvavg(self,*args, **kw_args):
	return Nysol_Mmvavg(nutil.args2dict(args,kw_args,Nysol_Mmvavg._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mmvavg", mmvavg)
