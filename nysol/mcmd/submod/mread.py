# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mread(NysolMOD_CORE):
	_kwd = n_core.getparalist("mread")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mread,self).__init__("mread",nutil.args2dict(args,kw_args,Nysol_Mread._kwd))

def mread(self,*args, **kw_args):
	return Nysol_Mread(nutil.args2dict(args,kw_args,Nysol_Mread._kwd)).addPre(self)


setattr(NysolMOD_CORE, "mread", mread)
