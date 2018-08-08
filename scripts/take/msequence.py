#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs 
import nysol.take as nt


args=margs.Margs(sys.argv,"i=,c=,x=,O=,tid=,time=,item=,cls=,taxo=,s=,S=,sx=,Sx=,g=,p=,-uniform,l=,u=,top=,gap=,win=,-padding,T=,-mcmdenv","i=")
nt.msequence(**(args.kvmap())).run()

