# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mshuffle(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd  = n_core.getparalist("mshuffle",3)
	_disabled_ouputlist = True

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mshuffle,self).__init__("mshuffle",nutil.args2dict(args,kw_args,Nysol_Mshuffle._kwd))

def mshuffle(self,*args, **kw_args):
	return Nysol_Mshuffle(nutil.args2dict(args,kw_args,Nysol_Mshuffle._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mshuffle", mshuffle)
