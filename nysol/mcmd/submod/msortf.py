# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Msortf(NysolMOD_CORE):
	_kwd = n_core.getparalist("msortf")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msortf,self).__init__("msortf",nutil.args2dict(args,kw_args,Nysol_Msortf._kwd))


def msortf(self,*args, **kw_args):
	return Nysol_Msortf(nutil.args2dict(args,kw_args,Nysol_Msortf._kwd)).addPre(self)

setattr(NysolMOD_CORE, "msortf", msortf)
