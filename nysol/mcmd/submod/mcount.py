# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mcount(NysolMOD_CORE):
	_kwd = n_core.getparalist("mcount")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcount,self).__init__("mcount",nutil.args2dict(args,kw_args,Nysol_Mcount._kwd))

def mcount(self,*args, **kw_args):
	return Nysol_Mcount(nutil.args2dict(args,kw_args,Nysol_Mcount._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcount", mcount)
