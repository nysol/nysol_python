# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mfldname(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mfldname",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mfldname,self).__init__("mfldname",nutil.args2dict(args,kw_args,Nysol_Mfldname._kwd))

def mfldname(self,*args, **kw_args):
	return Nysol_Mfldname(nutil.args2dict(args,kw_args,Nysol_Mfldname._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mfldname", mfldname)
