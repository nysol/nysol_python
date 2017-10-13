# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Writelist(NysolMOD_CORE):
	kwd = n_core.getparalist("writelist")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Writelist,self).__init__("writelist",nutil.args2dict(args,kw_args,Nysol_Writelist.kwd,uk="o"))

