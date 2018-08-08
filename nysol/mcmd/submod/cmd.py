# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Excmd(NysolMOD_CORE):
	_kwd ,_inkwd,_outkwd  = n_core.getparalist("cmd",3)
	_disabled_ouputlist = True

	def __init__(self,*args, **kw_args) :
		super(Nysol_Excmd,self).__init__("cmd",nutil.arg2dict(args,kw_args,Nysol_Excmd._kwd))


def cmd(self,*args, **kw_args):

	return Nysol_Excmd(nutil.arg2dict(args,kw_args,Nysol_Excmd._kwd)).addPre(self)

setattr(NysolMOD_CORE, "cmd", cmd)
