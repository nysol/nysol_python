# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mavg(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mavg",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mavg,self).__init__("mavg",nutil.args2dict(args,kw_args,Nysol_Mavg._kwd))

def mavg(self,*args, **kw_args):
	return Nysol_Mavg(nutil.args2dict(args,kw_args,Nysol_Mavg._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mavg", mavg)

