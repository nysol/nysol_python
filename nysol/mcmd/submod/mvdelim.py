# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvdelim(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mvdelim",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvdelim,self).__init__("mvdelim",nutil.args2dict(args,kw_args,Nysol_Mvdelim._kwd))

def mvdelim(self,*args, **kw_args):
	return Nysol_Mvdelim(nutil.args2dict(args,kw_args,Nysol_Mvdelim._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvdelim", mvdelim)
