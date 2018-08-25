# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Writecsv(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd = n_core.getparalist("writecsv",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Writecsv,self).__init__("writecsv",nutil.args2dict(args,kw_args,Nysol_Writecsv._kwd,uk="o"))


def writecsv(self,*args, **kw_args):
	return Nysol_Writecsv(nutil.args2dict(args,kw_args,Nysol_Writecsv._kwd,uk="o")).addPre(self)

setattr(NysolMOD_CORE, "writecsv", writecsv)

