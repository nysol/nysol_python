# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mwindow(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mwindow",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mwindow,self).__init__("mwindow",nutil.args2dict(args,kw_args,Nysol_Mwindow._kwd))

def mwindow(self,*args, **kw_args):
	return Nysol_Mwindow(nutil.args2dict(args,kw_args,Nysol_Mwindow._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mwindow", mwindow)

