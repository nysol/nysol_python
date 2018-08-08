# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvreplace(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mvreplace",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvreplace,self).__init__("mvreplace",nutil.args2dict(args,kw_args,Nysol_Mvreplace._kwd))


def mvreplace(self,*args, **kw_args):
	return Nysol_Mvreplace(nutil.args2dict(args,kw_args,Nysol_Mvreplace._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvreplace", mvreplace)
