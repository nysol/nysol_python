# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mkeybreak(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mkeybreak",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mkeybreak,self).__init__("mkeybreak",nutil.args2dict(args,kw_args,Nysol_Mkeybreak._kwd))

def mkeybreak(self,*args, **kw_args):
	return Nysol_Mkeybreak(nutil.args2dict(args,kw_args,Nysol_Mkeybreak._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mkeybreak", mkeybreak)
