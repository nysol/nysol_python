# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnewstr(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd  = n_core.getparalist("mnewstr",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnewstr,self).__init__("mnewstr",nutil.args2dict(args,kw_args,Nysol_Mnewstr._kwd))

