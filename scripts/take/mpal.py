#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs
import nysol.take.mpal as mpal


args=margs.Margs(sys.argv,"i=,tid=,item=,ro=,eo=,no=,s=,S=,filter=,lb=,ub=,sim=,dir=,rank=,-prune,-num,-verbose","i=,tid=,item=,eo=,no=")
mpal.mpal(args).run()

