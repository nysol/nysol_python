# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mrjoin(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mrjoin",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mrjoin,self).__init__("mrjoin",nutil.args2dict(args,kw_args,Nysol_Mrjoin._kwd))


def mrjoin(self,*args, **kw_args):
	return Nysol_Mrjoin(nutil.args2dict(args,kw_args,Nysol_Mrjoin._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mrjoin", mrjoin)
