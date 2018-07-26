# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Municat(NysolMOD_CORE):
	_kwd = n_core.getparalist("municat")
	_inkwd = ["i=","m="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Municat,self).__init__("municat",nutil.args2dict(args,kw_args,Nysol_Municat._kwd))

def municat(self,*args, **kw_args):
	return Nysol_Municat(nutil.args2dict(args,kw_args,Nysol_Municat._kwd)).addPre(self)

setattr(NysolMOD_CORE, "municat", municat)
