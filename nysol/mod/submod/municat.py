# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Municat(NysolMOD_CORE):
	kwd = n_core.getparalist("municat")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Municat,self).__init__("municat",nutil.args2dict(args,kw_args,Nysol_Municat.kwd))

def municat(self,*args, **kw_args):
	return Nysol_Municat(nutil.args2dict(args,kw_args,Nysol_Municat.kwd)).addPre(self)

setattr(NysolMOD_CORE, "municat", municat)