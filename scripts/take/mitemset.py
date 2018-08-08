#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs
import nysol.take as nt


args=margs.Margs(sys.argv,"i=,x=,O=,tid=,item=,cls=,taxo=,type=,s=,S=,sx=,Sx=,g=,p=,-uniform,l=,u=,top=,T=,-replaceTaxo")
nt.mitemset(**(args.kvmap())).run()

