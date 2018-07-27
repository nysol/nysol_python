# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvuniq(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd  = n_core.getparalist("mvuniq",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvuniq,self).__init__("mvuniq",nutil.args2dict(args,kw_args,Nysol_Mvuniq._kwd))

def mvuniq(self,*args, **kw_args):
	return Nysol_Mvuniq(nutil.args2dict(args,kw_args,Nysol_Mvuniq._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvuniq", mvuniq)

