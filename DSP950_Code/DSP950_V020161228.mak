# Generated by the VisualDSP++ IDDE

# Note:  Any changes made to this Makefile will be lost the next time the
# matching project file is loaded into the IDDE.  If you wish to preserve
# changes, rename this file and run it externally to the IDDE.

# The syntax of this Makefile is such that GNU Make v3.77 or higher is
# required.

# The current working directory should be the directory in which this
# Makefile resides.

# Supported targets:
#     DSP950_V020161228_Debug
#     DSP950_V020161228_Debug_clean

# Define this variable if you wish to run this Makefile on a host
# other than the host that created it and VisualDSP++ may be installed
# in a different directory.

ADI_DSP=D:\Program Files\Analog Devices\VisualDSP 5.1.2


# $VDSP is a gmake-friendly version of ADI_DIR

empty:=
space:= $(empty) $(empty)
VDSP_INTERMEDIATE=$(subst \,/,$(ADI_DSP))
VDSP=$(subst $(space),\$(space),$(VDSP_INTERMEDIATE))

RM=cmd /C del /F /Q

#
# Begin "DSP950_V020161228_Debug" configuration
#

ifeq ($(MAKECMDGOALS),DSP950_V020161228_Debug)

DSP950_V020161228_Debug : ./Debug/DSP950_V020161228.dxe 

./Debug/Channel_To_DA_Port.doj :src/Channel_To_DA_Port.c include/channel_To_DA_Port.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h include/input_types.h $(VDSP)/214xx/include/stdlib.h $(VDSP)/214xx/include/time.h $(VDSP)/214xx/include/yvals.h $(VDSP)/214xx/include/xcycle_count.h $(VDSP)/214xx/include/limits.h $(VDSP)/214xx/include/cycle_count_21xxx.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h $(VDSP)/214xx/include/complex.h include/PCM4104.h include/cs5368.h include/spdif_coax.h 
	@echo ".\src\Channel_To_DA_Port.c"
	$(VDSP)/cc21k.exe -c .\src\Channel_To_DA_Port.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\Channel_To_DA_Port.doj -MM

./Debug/CS4385A.doj :src/CS4385A.c include/cs4385a.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h 
	@echo ".\src\CS4385A.c"
	$(VDSP)/cc21k.exe -c .\src\CS4385A.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\CS4385A.doj -MM

./Debug/CS5368.doj :src/CS5368.c include/cs5368.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h include/spdif_coax.h include/input_types.h 
	@echo ".\src\CS5368.c"
	$(VDSP)/cc21k.exe -c .\src\CS5368.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\CS5368.doj -MM

./Debug/Dsp_Signal.doj :src/Dsp_Signal.c include/dsp_signal.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/stdlib.h $(VDSP)/214xx/include/time.h $(VDSP)/214xx/include/yvals.h $(VDSP)/214xx/include/xcycle_count.h $(VDSP)/214xx/include/limits.h $(VDSP)/214xx/include/cycle_count_21xxx.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h 
	@echo ".\src\Dsp_Signal.c"
	$(VDSP)/cc21k.exe -c .\src\Dsp_Signal.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\Dsp_Signal.doj -MM

./Debug/EQ.doj :src/EQ.c include/EQ.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/stdlib.h $(VDSP)/214xx/include/time.h $(VDSP)/214xx/include/yvals.h $(VDSP)/214xx/include/xcycle_count.h $(VDSP)/214xx/include/limits.h $(VDSP)/214xx/include/cycle_count_21xxx.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h $(VDSP)/214xx/include/complex.h $(VDSP)/214xx/include/filter.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h 
	@echo ".\src\EQ.c"
	$(VDSP)/cc21k.exe -c .\src\EQ.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\EQ.doj -MM

./Debug/init_PLL_SDRAM.doj :src/init_PLL_SDRAM.c $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/cdef21489.h 
	@echo ".\src\init_PLL_SDRAM.c"
	$(VDSP)/cc21k.exe -c .\src\init_PLL_SDRAM.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\init_PLL_SDRAM.doj -MM

./Debug/initSRU.doj :src/initSRU.c $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/cdef21489.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h 
	@echo ".\src\initSRU.c"
	$(VDSP)/cc21k.exe -c .\src\initSRU.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\initSRU.doj -MM

./Debug/Input_Types.doj :src/Input_Types.c include/Input_Types.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h include/spi.h include/cs5368.h include/mic.h include/PCM4104.h include/channel_To_DA_Port.h include/spi_code.h include/spdif_coax.h include/rca.h $(VDSP)/214xx/include/time.h $(VDSP)/214xx/include/yvals.h $(VDSP)/214xx/include/xcycle_count.h $(VDSP)/214xx/include/limits.h $(VDSP)/214xx/include/cycle_count_21xxx.h 
	@echo ".\src\Input_Types.c"
	$(VDSP)/cc21k.exe -c .\src\Input_Types.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\Input_Types.doj -MM

./Debug/main.doj :main.c $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h include/spi.h include/cs4385a.h include/input_types.h $(VDSP)/214xx/include/time.h $(VDSP)/214xx/include/yvals.h $(VDSP)/214xx/include/xcycle_count.h $(VDSP)/214xx/include/limits.h $(VDSP)/214xx/include/cycle_count_21xxx.h include/cs5368.h include/spi_code.h include/pcm4104.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h 
	@echo ".\main.c"
	$(VDSP)/cc21k.exe -c .\main.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\main.doj -MM

./Debug/MIC.doj :src/MIC.c include/mic.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/cdef21489.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h 
	@echo ".\src\MIC.c"
	$(VDSP)/cc21k.exe -c .\src\MIC.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\MIC.doj -MM

./Debug/PCM4104.doj :src/PCM4104.c include/pcm4104.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h include/spi.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h include/cs4385a.h include/input_types.h 
	@echo ".\src\PCM4104.c"
	$(VDSP)/cc21k.exe -c .\src\PCM4104.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\PCM4104.doj -MM

./Debug/RCA.doj :src/RCA.c include/RCA.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/cdef21489.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h 
	@echo ".\src\RCA.c"
	$(VDSP)/cc21k.exe -c .\src\RCA.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\RCA.doj -MM

./Debug/Spdif_Coax.doj :src/Spdif_Coax.c include/spdif_coax.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h $(VDSP)/214xx/include/string.h include/cs5368.h include/pcm4104.h 
	@echo ".\src\Spdif_Coax.c"
	$(VDSP)/cc21k.exe -c .\src\Spdif_Coax.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\Spdif_Coax.doj -MM

./Debug/SPI.doj :src/SPI.c include/SPI.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/cdef21489.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h 
	@echo ".\src\SPI.c"
	$(VDSP)/cc21k.exe -c .\src\SPI.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\SPI.doj -MM

./Debug/Spi_Code.doj :src/Spi_Code.c $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h $(VDSP)/214xx/include/complex.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h include/dsp_signal.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h include/volume.h 
	@echo ".\src\Spi_Code.c"
	$(VDSP)/cc21k.exe -c .\src\Spi_Code.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\Spi_Code.doj -MM

./Debug/SPORT0_isr.doj :src/SPORT0_isr.c include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/time.h $(VDSP)/214xx/include/yvals.h $(VDSP)/214xx/include/xcycle_count.h $(VDSP)/214xx/include/limits.h $(VDSP)/214xx/include/cycle_count_21xxx.h $(VDSP)/214xx/include/string.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h include/pcm4104.h include/input_types.h 
	@echo ".\src\SPORT0_isr.c"
	$(VDSP)/cc21k.exe -c .\src\SPORT0_isr.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\SPORT0_isr.doj -MM

./Debug/Volume.doj :src/Volume.c include/volume.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/sru.h $(VDSP)/214xx/include/sru21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h 
	@echo ".\src\Volume.c"
	$(VDSP)/cc21k.exe -c .\src\Volume.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\Volume.doj -MM

./Debug/Write_File.doj :src/Write_File.c include/write_file.h $(VDSP)/214xx/include/stdio.h $(VDSP)/214xx/include/stdio_21xxx.h include/ADDS_21489_EzKit.h $(VDSP)/214xx/include/signal.h $(VDSP)/214xx/include/cdef21489.h $(VDSP)/214xx/include/def21489.h $(VDSP)/214xx/include/sysreg.h $(VDSP)/214xx/include/math.h $(VDSP)/214xx/include/math_21xxx.h 
	@echo ".\src\Write_File.c"
	$(VDSP)/cc21k.exe -c .\src\Write_File.c -file-attr ProjectName=DSP950_V020161228 -g -structs-do-not-overlap -no-multiline -I .\include -double-size-32 -swc -warn-protos -si-revision 0.2 -proc ADSP-21489 -o .\Debug\Write_File.doj -MM

./Debug/DSP950_V020161228.dxe :./ADSP-21489.LDF $(VDSP)/214xx/lib/21479_rev_any/21489_hdr.doj ./Debug/Channel_To_DA_Port.doj ./Debug/CS4385A.doj ./Debug/CS5368.doj ./Debug/Dsp_Signal.doj ./Debug/EQ.doj ./Debug/init_PLL_SDRAM.doj ./Debug/initSRU.doj ./Debug/Input_Types.doj ./Debug/main.doj ./Debug/MIC.doj ./Debug/PCM4104.doj ./Debug/RCA.doj ./Debug/Spdif_Coax.doj ./Debug/SPI.doj ./Debug/Spi_Code.doj ./Debug/SPORT0_isr.doj ./Debug/Volume.doj ./Debug/Write_File.doj $(VDSP)/214xx/lib/21479_rev_any/libc.dlb $(VDSP)/214xx/lib/21479_rev_any/libio.dlb $(VDSP)/214xx/lib/21479_rev_any/libcpp.dlb $(VDSP)/214xx/lib/21479_rev_any/libdsp.dlb 
	@echo "Linking..."
	$(VDSP)/cc21k.exe .\Debug\Channel_To_DA_Port.doj .\Debug\CS4385A.doj .\Debug\CS5368.doj .\Debug\Dsp_Signal.doj .\Debug\EQ.doj .\Debug\init_PLL_SDRAM.doj .\Debug\initSRU.doj .\Debug\Input_Types.doj .\Debug\main.doj .\Debug\MIC.doj .\Debug\PCM4104.doj .\Debug\RCA.doj .\Debug\Spdif_Coax.doj .\Debug\SPI.doj .\Debug\Spi_Code.doj .\Debug\SPORT0_isr.doj .\Debug\Volume.doj .\Debug\Write_File.doj -T .\ADSP-21489.LDF -map .\Debug\DSP950_V020161228.map.xml -L .\Debug -L .\lib -add-debug-libpaths -swc -flags-link -od,.\Debug -o .\Debug\DSP950_V020161228.dxe -proc ADSP-21489 -si-revision 0.2 -flags-link -MM

endif

ifeq ($(MAKECMDGOALS),DSP950_V020161228_Debug_clean)

DSP950_V020161228_Debug_clean:
	-$(RM) ".\Debug\Channel_To_DA_Port.doj"
	-$(RM) ".\Debug\CS4385A.doj"
	-$(RM) ".\Debug\CS5368.doj"
	-$(RM) ".\Debug\Dsp_Signal.doj"
	-$(RM) ".\Debug\EQ.doj"
	-$(RM) ".\Debug\init_PLL_SDRAM.doj"
	-$(RM) ".\Debug\initSRU.doj"
	-$(RM) ".\Debug\Input_Types.doj"
	-$(RM) ".\Debug\main.doj"
	-$(RM) ".\Debug\MIC.doj"
	-$(RM) ".\Debug\PCM4104.doj"
	-$(RM) ".\Debug\RCA.doj"
	-$(RM) ".\Debug\Spdif_Coax.doj"
	-$(RM) ".\Debug\SPI.doj"
	-$(RM) ".\Debug\Spi_Code.doj"
	-$(RM) ".\Debug\SPORT0_isr.doj"
	-$(RM) ".\Debug\Volume.doj"
	-$(RM) ".\Debug\Write_File.doj"
	-$(RM) ".\Debug\DSP950_V020161228.dxe"
	-$(RM) ".\Debug\*.ipa"
	-$(RM) ".\Debug\*.opa"
	-$(RM) ".\Debug\*.ti"
	-$(RM) ".\Debug\*.pgi"
	-$(RM) ".\*.rbld"

endif


