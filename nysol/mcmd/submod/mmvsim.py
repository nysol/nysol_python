# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mmvsim(NysolMOD_CORE):
	_kwd = n_core.getparalist("mmvsim")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mmvsim,self).__init__("mmvsim",nutil.args2dict(args,kw_args,Nysol_Mmvsim._kwd))

def mmvsim(self,*args, **kw_args):
	return Nysol_Mmvsim(nutil.args2dict(args,kw_args,Nysol_Mmvsim._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mmvsim", mmvsim)
