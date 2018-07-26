# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mcombi(NysolMOD_CORE):
	_kwd = n_core.getparalist("mcombi")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcombi,self).__init__("mcombi",nutil.args2dict(args,kw_args,Nysol_Mcombi._kwd))

def mcombi(self,*args, **kw_args):
	return Nysol_Mcombi(nutil.args2dict(args,kw_args,Nysol_Mcombi._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcombi", mcombi)

