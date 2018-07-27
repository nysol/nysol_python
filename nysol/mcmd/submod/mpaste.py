# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mpaste(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mpaste",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mpaste,self).__init__("mpaste",nutil.args2dict(args,kw_args,Nysol_Mpaste._kwd))

def mpaste(self,*args, **kw_args):
	return Nysol_Mpaste(nutil.args2dict(args,kw_args,Nysol_Mpaste._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mpaste", mpaste)
