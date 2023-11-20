#
# Copyright 2023 Apropos org, Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Create a project
open_project -reset proj_hard_sydr_hls

# Add design files
set_top top_hls_hard_sydr
add_files source/acquisition.cpp
add_files source/acquisition.h
add_files source/channel.cpp
add_files source/channel.h
add_files source/constants.h
add_files source/fft.cpp
add_files source/fft.h
add_files source/float_calcu.cpp
add_files source/float_calcu.h
add_files source/gen_code.cpp
add_files source/gen_code.h
add_files source/pocketfft.cpp
add_files source/pocketfft.h
add_files source/saveChannelData.cpp
add_files source/saveChannelData.h
add_files source/source_main.cpp
add_files source/structures.h
add_files source/tracking.cpp
add_files source/tracking.h
add_files source/utils.cpp
add_files source/utils.h
add_files -tb source/testbench.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"

# Set the top-level function

open_solution "opt0" -flow_target vivado
set_part {xck26-sfvc784-2LV-c}
create_clock -period 10 -name default
config_compile -pipeline_loops 4
# export the IP as hls_ip_hsydr.zip in the  ../hls_out/ folder 
config_export -description HLS_IP_HSYDR -display_name HLS_IP_HSYDR \
    -format ip_catalog -output ../hls_out/hls_ip_hsydr \
    -rtl verilog -version 2.0 -vivado_clock 10
source "directives.tcl"


set hls_exec 3


if {$hls_exec == 1} {

	
} elseif {$hls_exec == 2} {
	csim_design -clean
	# Run Synthesis, RTL Simulation and Exit
	csynth_design
	# cosim_design -wave_debug -trace_level all
	cosim_design 

} elseif {$hls_exec == 3} { 
	csim_design -clean
	# Run Synthesis and Export IP  
    csynth_design
    # export_design -flow syn -rtl verilog -format ip_catalog -output ../hls_out/hls_ip_hsydr

	set currentTime [clock seconds]
	set formattedTime [clock format $currentTime -format "%y%m%d%H%M"]
	set outputPath "../hls_out/hls_ip_hsydr_${formattedTime}"
	set v_version "1.${formattedTime}"
	export_design -flow syn -rtl verilog -format ip_catalog -output $outputPath -version $v_version


} elseif {$hls_exec == 4} { 

} else {
	# Default is to exit after setup
	csynth_design
}

exit




