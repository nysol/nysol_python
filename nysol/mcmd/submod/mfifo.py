# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mfifo(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mfifo",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mfifo,self).__init__("mfifo",nutil.args2dict(args,kw_args,Nysol_Mfifo._kwd))


def mfifo(self,*args, **kw_args):
	return Nysol_Mfifo(nutil.args2dict(args,kw_args,Nysol_Mfifo._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mfifo", mfifo)
