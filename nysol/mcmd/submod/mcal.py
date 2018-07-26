# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mcal(NysolMOD_CORE):
	_kwd = n_core.getparalist("mcal")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcal,self).__init__("mcal",nutil.args2dict(args,kw_args,Nysol_Mcal._kwd))

def mcal(self,*args, **kw_args):
	return Nysol_Mcal(nutil.args2dict(args,kw_args,Nysol_Mcal._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcal", mcal)

