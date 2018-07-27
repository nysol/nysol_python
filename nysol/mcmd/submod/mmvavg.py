# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mmvavg(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd  = n_core.getparalist("mmvavg",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mmvavg,self).__init__("mmvavg",nutil.args2dict(args,kw_args,Nysol_Mmvavg._kwd))


def mmvavg(self,*args, **kw_args):
	return Nysol_Mmvavg(nutil.args2dict(args,kw_args,Nysol_Mmvavg._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mmvavg", mmvavg)
