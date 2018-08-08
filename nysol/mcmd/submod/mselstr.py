# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mselstr(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mselstr",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mselstr,self).__init__("mselstr",nutil.args2dict(args,kw_args,Nysol_Mselstr._kwd))

def mselstr(self,*args, **kw_args):
	return Nysol_Mselstr(nutil.args2dict(args,kw_args,Nysol_Mselstr._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mselstr", mselstr)
