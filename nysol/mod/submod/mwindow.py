# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mwindow(NysolMOD_CORE):
	kwd = n_core.getparalist("mwindow")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mwindow,self).__init__("mwindow",nutil.args2dict(args,kw_args,Nysol_Mwindow.kwd))

def mwindow(self,*args, **kw_args):
	return Nysol_Mwindow(nutil.args2dict(args,kw_args,Nysol_Mwindow.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mwindow", mwindow)

