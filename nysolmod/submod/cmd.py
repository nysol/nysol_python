# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Excmd(NysolMOD_CORE):
	kwd = n_core.getparalist("cmd")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Excmd,self).__init__("cmd",nutil.arg2dict(args,kw_args,Nysol_Excmd.kwd))

