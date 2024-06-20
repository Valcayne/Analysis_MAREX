# Simple HTCondor submit description file
# Everything with a leading # is a comment
 
executable   = /afs/cern.ch/work/v/valcayne/2024_Er_U_ProgramsC6D6/ApsC6D6/EnergyCalibration/MakeEnergyCalibration12VIC
arguments    = $(Source)  $(DetectorsCalibrated) $(SourceType) $(DataFolder) $(RunList) $(FolderRootFiles) $(Expfname) $(ExpfnameBackground) $(SourceTypeBakground) $(NumberBinsExpfname) $(EMaxExpfname) $(MCfname) $(outFolder) $(GammaEnergy) $(rebin) $(Percen) $(npRes) $(Res) $(PERRes) $(npCalib) $(Calib) $(PERCalib)
#output       = outputs_condor/DTAnalyzer01_$(EXTRRun)_$(RAWRun)_det$(DetN)_$(Quant)_$(nEn)_$(nE1).out
output       = Condor/outputs/output_$(Source).out
error        = Condor/outputs/error_$(Source).err
#error        = outputs_condor/DTAnalyzer01_$(EXTRRun)_$(RAWRun)_det$(DetN)_$(Quant)_$(nEn)_$(nE1).err
log          = Condor/outputs/log_$(Source).err
transfer_output_files   = ""
requirements = (OpSysAndVer =?= "AlmaLinux9")
#requirements = (OpSysAndVer =?= "CentOS7")

#+JobFlavour = "workday"
#+JobFlavour = "espresso"
+JobFlavour = "microcentury"


queue Source,DetectorsCalibrated,SourceType, DataFolder, RunList, FolderRootFiles, Expfname,ExpfnameBackground,  SourceTypeBakground, NumberBinsExpfname, EMaxExpfname,    MCfname, outFolder, GammaEnergy, rebin, Percen, npRes, Res, PERRes, npCalib, Calib, PERCalib from /afs/cern.ch/work/v/valcayne/2024_Er_U_ProgramsC6D6/InputsFor_Er_Cu_U/Condor/Inputs/ArgumentsForCondor.txt
