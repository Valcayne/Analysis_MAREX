# Simple HTCondor submit description file
# Everything with a leading # is a comment
 
executable   = /afs/cern.ch/user/v/valcayne/Workspace/2024_Er_U_ProgramsC6D6/Analysis_nTOF_v01/Make2DHistos/Merge2DHisto
arguments    = $(run)
#output       = outputs_condor/DTAnalyzer01_$(EXTRRun)_$(RAWRun)_det$(DetN)_$(Quant)_$(nEn)_$(nE1).out
output       = Condor/outputs/output_$(run).out
error        = Condor/outputs/error_$(run).err
#error        = outputs_condor/DTAnalyzer01_$(EXTRRun)_$(RAWRun)_det$(DetN)_$(Quant)_$(nEn)_$(nE1).err
log          = Condor/outputs/log_$(run).err
transfer_output_files   = ""
requirements = (OpSysAndVer =?= "AlmaLinux9")
#requirements = (OpSysAndVer =?= "CentOS7")
#+JobFlavour = "workday"
#+JobFlavour = "espresso"
+JobFlavour = "microcentury"



queue run from /afs/cern.ch/user/v/valcayne/Workspace/2024_Er_U_ProgramsC6D6/Analysis_nTOF_v01/Make2DHistos/Condor/Inputs/ArgumentsForCondorMerge2DHistos.txt
