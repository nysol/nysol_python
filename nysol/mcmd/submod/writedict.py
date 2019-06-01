# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Writedict(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("writelist",3)
	_kwd[0].append("dtype")
	_kwd[1].append("dict")

	def __init__(self,*args, **kw_args) :
		kw_args["dict"] = True
		super(Nysol_Writedict,self).__init__("writelist",nutil.args2dict(args,kw_args,Nysol_Writedict._kwd,uk="o"))


def writedict(self,*args, **kw_args):
	kw_args["dict"] = True
	return Nysol_Writedict(nutil.args2dict(args,kw_args,Nysol_Writedict._kwd,uk="o")).addPre(self)

setattr(NysolMOD_CORE, "writedict", writedict)

