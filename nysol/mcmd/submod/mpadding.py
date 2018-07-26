# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mpadding(NysolMOD_CORE):
	_kwd = n_core.getparalist("mpadding")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mpadding,self).__init__("mpadding",nutil.args2dict(args,kw_args,Nysol_Mpadding._kwd))

def mpadding(self,*args, **kw_args):
	return Nysol_Mpadding(nutil.args2dict(args,kw_args,Nysol_Mpadding._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mpadding", mpadding)
