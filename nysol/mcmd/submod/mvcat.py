# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvcat(NysolMOD_CORE):
	_kwd = n_core.getparalist("mvcat")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvcat,self).__init__("mvcat",nutil.args2dict(args,kw_args,Nysol_Mvcat._kwd))

def mvcat(self,*args, **kw_args):
	return Nysol_Mvcat(nutil.args2dict(args,kw_args,Nysol_Mvcat._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvcat", mvcat)
