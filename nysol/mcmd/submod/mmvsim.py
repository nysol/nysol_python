# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mmvsim(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mmvsim",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mmvsim,self).__init__("mmvsim",nutil.args2dict(args,kw_args,Nysol_Mmvsim._kwd))

def mmvsim(self,*args, **kw_args):
	return Nysol_Mmvsim(nutil.args2dict(args,kw_args,Nysol_Mmvsim._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mmvsim", mmvsim)
