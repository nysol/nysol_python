# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mtra(NysolMOD_CORE):
	_kwd = n_core.getparalist("mtra")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtra,self).__init__("mtra",nutil.args2dict(args,kw_args,Nysol_Mtra._kwd))

def mtra(self,*args, **kw_args):
	return Nysol_Mtra(nutil.args2dict(args,kw_args,Nysol_Mtra._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mtra", mtra)
