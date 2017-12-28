from distutils.core import setup, Extension
import re
import subprocess
args = ['xml2-config','--cflags']
xxxxs= subprocess.check_output(args).decode().rstrip().split()

hedears = ['src','src/kgmod','src/mod']
for xxx in xxxxs:
	hedears.append(re.sub(r'^-I','',xxx))

module1 = Extension('nysol/_nysolshell_core',
                    sources = ['src/mod/kg2tee.cpp','src/mod/kgfifo.cpp','src/mod/kgtrafld.cpp',
                    						'src/kgexcmd.cpp','nysolshell.cpp','src/kgshell.cpp','src/kgload.cpp',
                    						'src/kgmod/kgArgFld.cpp',
																'src/kgmod/kgArgs.cpp','src/kgmod/kgCSV.cpp',
																'src/kgmod/kgCSVout.cpp','src/kgmod/kgCSVutils.cpp',
																'src/kgmod/kgEnv.cpp','src/kgmod/kgError.cpp',
																'src/kgmod/kgFldBuffer.cpp','src/kgmod/kgFldHash.cpp',
																'src/kgmod/kgFunction.cpp','src/kgmod/kgMessage.cpp',
																'src/kgmod/kgMethod.cpp','src/kgmod/kgRange.cpp',
																'src/kgmod/kgTempfile.cpp','src/kgmod/kgVal.cpp',
																'src/kgmod/kgWildcard.cpp','src/kgmod/kgmod.cpp',
																'src/kgmod/kgmodincludesort.cpp','src/kgmod/kgsortf.cpp',
																'src/mod/kgcal.cpp','src/mod/kgcut.cpp','src/mod/kgsum.cpp','src/mod/kgjoin.cpp',
																'src/mod/kg2cross.cpp','src/mod/kgaccum.cpp','src/mod/kgavg.cpp',
																'src/mod/kgbest.cpp','src/mod/kgbucket.cpp','src/mod/kgchgnum.cpp',
																'src/mod/kgchgstr.cpp','src/mod/kgcombi.cpp','src/mod/kgcommon.cpp',
																'src/mod/kgcount.cpp','src/mod/kgcross.cpp','src/mod/kgdelnull.cpp',
																'src/mod/kgdformat.cpp','src/mod/kgduprec.cpp','src/mod/kgfldname.cpp',
																'src/mod/kgfsort.cpp','src/mod/kghashavg.cpp','src/mod/kghashsum.cpp',
																'src/mod/kgkeybreak.cpp','src/mod/kgmbucket.cpp','src/mod/kgmvavg.cpp',
																'src/mod/kgmvsim.cpp','src/mod/kgmvstats.cpp','src/mod/kgnewnumber.cpp',
																'src/mod/kgnewrand.cpp','src/mod/kgnewstr.cpp','src/mod/kgnjoin.cpp',
																'src/mod/kgnormalize.cpp','src/mod/kgnrcommon.cpp','src/mod/kgnrjoin.cpp',
																'src/mod/kgnullto.cpp','src/mod/kgnumber.cpp','src/mod/kgpadding.cpp',
																'src/mod/kgpaste.cpp','src/mod/kgproduct.cpp','src/mod/kgrand.cpp',
																'src/mod/kgrjoin.cpp','src/mod/kgsed.cpp','src/mod/kgsel.cpp',
																'src/mod/kgselnum.cpp','src/mod/kgselrand.cpp','src/mod/kgselstr.cpp',
																'src/mod/kgsetstr.cpp','src/mod/kgshare.cpp','src/mod/kgsim.cpp',
																'src/mod/kgslide.cpp','src/mod/kgsplit.cpp','src/mod/kgstats.cpp',
																'src/mod/kgsummary.cpp','src/mod/kgtonull.cpp','src/mod/kgtra.cpp',
																'src/mod/kgtraflg.cpp','src/mod/kguniq.cpp','src/mod/kgcat.cpp',
																'src/mod/kgvcat.cpp','src/mod/kgvcommon.cpp','src/mod/kgvcount.cpp',
																'src/mod/kgvdelim.cpp','src/mod/kgvdelnull.cpp','src/mod/kgvjoin.cpp',
																'src/mod/kgvnullto.cpp','src/mod/kgvreplace.cpp','src/mod/kgvsort.cpp',
																'src/mod/kgvuniq.cpp','src/mod/kgwindow.cpp',
																'src/mod/kgarff2csv.cpp','src/mod/kgtab2csv.cpp','src/mod/kgxml2csv.cpp'
                    	],
										include_dirs=hedears,
										libraries=['pthread','boost_filesystem','boost_regex','boost_system','xml2']
										)

lcmmod = Extension('nysol/take/_lcmlib',
                    sources = ['src/take/lcmrap.c'],
										include_dirs=['src','src/kgmod','src/mod','src/take/lcm']
										)

seqmod = Extension('nysol/take/_lcmseqlib',
                    sources = ['src/take/lcmseqrap.c'],
										include_dirs=['src','src/kgmod','src/mod','src/take/lcmseq']
										)

seqmodzero = Extension('nysol/take/_lcmseq_zerolib',
                    sources = ['src/take/lcmseq_zerorap.c'],
										include_dirs=['src','src/kgmod','src/mod','src/take/lcmseq_zero']
										)

lcmtransmod = Extension('nysol/take/_lcmtranslib',
                    sources = ['src/take/lcmtransrun.cpp'],
										include_dirs=['src','src/kgmod','src/mod']
										)


sspcmod = Extension('nysol/take/_sspclib',
                    sources = ['src/take/sspcrap.c'],
										include_dirs=['src','src/kgmod','src/mod','src/take/sspc']
										)
grhfilmod = Extension('nysol/take/_grhfillib',
                    sources = ['src/take/grhfilrap.c'],
										include_dirs=['src','src/kgmod','src/mod','src/take/grhfil']
										)
macemod = Extension('nysol/take/_macelib',
                    sources = ['src/take/macerap.c'],
										include_dirs=['src','src/kgmod','src/mod','src/take/mace']
										)
simsetmod = Extension('nysol/take/_simsetlib',
                    sources = ['src/take/simsetrap.c'],
										include_dirs=['src','src/kgmod','src/mod','src/take/simset']
										)
medsetmod = Extension('nysol/take/_medsetlib',
                    sources = ['src/take/medsetrap.c'],
										include_dirs=['src','src/kgmod','src/mod','src/take/medset']
										)
setup (name = 'nysol',
      packages=['nysol','nysol/util','nysol/mod','nysol/mod/submod','nysol/mod/nysollib','nysol/take','nysol/take/lib','nysol/take/lib/base'],
       version = '0.1',
       description = 'This is a demo package',
			scripts=['scripts/take/mfriends.py','scripts/take/mitemset.py','scripts/take/mpolishing.py','scripts/take/msequence.py','scripts/take/mtra2gc.py','scripts/take/mpal.py'],
       ext_modules = [module1,lcmmod,sspcmod,grhfilmod,macemod,seqmod,seqmodzero,lcmtransmod,macemod,simsetmod,medsetmod])
       
