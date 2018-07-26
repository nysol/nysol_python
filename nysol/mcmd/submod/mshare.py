# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mshare(NysolMOD_CORE):
	_kwd = n_core.getparalist("mshare")
	_inkwd = ["i="]
	_outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mshare,self).__init__("mshare",nutil.args2dict(args,kw_args,Nysol_Mshare._kwd))


def mshare(self,*args, **kw_args):
	return Nysol_Mshare(nutil.args2dict(args,kw_args,Nysol_Mshare._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mshare", mshare)
