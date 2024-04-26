## Introduction


<img align="right" src="https://raw.githubusercontent.com/aproposorg/KV260-PYNQ-tutorial/main/image/HWSYDR.png" height="140">

- This work is an extension of Python-based GNSS receiver design called SyDR https://github.com/aproposorg/sydr
- It extends the software design to the FPGA domain, which allows the algorithm to be run at the hardware, thus the design called Hard SyDR

- The design is based on Xilinx KV260 board.

For additional details on the architecture and a more in-depth analysis of the results, you can refer to our publication:

Grenier, A., Lei, J., Damsgaard, H. J., Quintana-Ortí, E. S., Ometov, A., Lohan, E. S., & Nurmi, J. (2024). Hard SyDR: A Benchmarking Environment for Global Navigation Satellite System Algorithms. Sensors, 24(2), 409.

We would appreciate if you could cite this reference in case you wish to mention Hard SyDR in your work!

```
@article{grenier2024hard,
  title={Hard SyDR: A Benchmarking Environment for Global Navigation Satellite System Algorithms},
  author={Grenier, Antoine and Lei, Jie and Damsgaard, Hans Jakob and Quintana-Ort{\'\i}, Enrique S and Ometov, Aleksandr and Lohan, Elena Simona and Nurmi, Jari},
  journal={Sensors},
  volume={24},
  number={2},
  pages={409},
  year={2024},
  publisher={MDPI}
}
```

## Structure of the repo

```
.
├── .gitignore
├── LICENSE
├── README.md
├── hls_out
│   └── hls_ip_hsydr_2311162147.zip
├── misc
│   └── HWSYDR.png
├── vivado_out
│   ├── black_design_design_1.pdf
│   ├── hard_sydr_FPGA.bit
│   └── hard_sydr_FPGA.hwh
├── ws_hls
│   ├── directives.tcl
│   ├── run_hard_sydr_hls.tcl
│   └── source
│       ├── acquisition.cpp
│       ├── acquisition.h
        ...
├── ws_pynq
│   ├── hard_sydr_FPGA.bit
│   ├── hard_sydr_FPGA.hwh
│   ├── hard_sydr_jup.ipynb
│   ├── rfdata_int64.txt
│   ├── rfdata_int64_300ms.txt
│   └── track_PL_300ms_ref_results.txt
└── ws_vivado
    └── make_hard_sydr_block_design.tcl

```
The repo contains two workspaces, the first one is `ws_hls` the HLS project can be built here. It contains the `tcl` script to run the design flow of Xilinx HLS. In the end of HLS workflow, the hard SyDR HLS IP can be generated and saved to 
```
├── hls_out
│   └── hls_ip_hsydr_XXXX.zip
```

To build the project, go to `ws_hls`, then `vitis_hls -f run_hard_sydr_hls.tcl`

---

When complect the HLS workflow. We can start to generate the `bitstream` of the design by following Vivado workflow in workspace `ws_vivado`. 

We included the `tcl` file that you can build the project rapidly, more on this in the coming section. 

---
For those users who want to run the design on the board straightaway, we prepared the compiled file so that you can run the design at the `PYNQ` environment out of the box.
```
├── ws_pynq
│   ├── hard_sydr_FPGA.bit
│   ├── hard_sydr_FPGA.hwh
│   ├── hard_sydr_jup.ipynb
│   ├── rfdata_int64.txt
│   ├── rfdata_int64_300ms.txt
│   └── track_PL_300ms_ref_results.txt
```
 
## Software Requirements 
To proceed with the running or building of the design. Please make sure your software has met the following requirements:

- AMD Xilinx HLS 2022.1
- AMD Xilinx Vivado 2022.1
- Linux x86 machine that runs at the supported OS for Xilinx 2022.1 tools 


### Only to run the design at the PYNQ environment at the KV260 board:

- The version of PYNQ for KV260 should be 3.0.X, https://github.com/Xilinx/Kria-PYNQ/releases/tag/v3.0

## Run the Design out of the box

Go to your KV260 board. Make sure it running the PYNQ 3.0.X and with Jupiter lab open. 

Copy all the files from `ws_pynq` directory, and run the `hard_sydr_jup.ipynb` file. 

## Steps to build the Design

### Build the Hard SyDR HLS IP

- Go to your x86 machine. Go to `ws_hls` and run the workflow as `vitis_hls -f run_hard_sydr_hls.tcl`. 
- Make sure you inspect the `tcl` file, you can choose the option for co-simulation. 
When compilation finishes, you will see your IP generated in `hls_out` folder with its timestamp.

```
├── hls_out
│   └── hls_ip_hsydr_2311162147.zip
```

### Build the Vivado

- Go to your x86 machine. Open Vivado in GUI mode. 
- Create a new project with KV260 as the target board. 
- Proceed the project creation without adding the source file. 
- Now you have entered a new layout, on your left you can see the steps for compilation flow now choose the `tcl` file in our repo and run 
    `Tool -> Run TCL scrips -> make_hard_sydr_block_design.tcl`
- Now add the IP into your system. Note, it might be tricky here. 
- From the left panel, open `IP catalog`.
- Right-click to add `IP repository`, then right-click to `Add IP to repository`
- Add `hls_ip_hsydr_2311162147.zip` into the block diagram you just created from `tcl` file
- Make the connections
    ```
    top_hls_hard_sydr_0 : s_axicontrol   --> axi_interconnect_0 : M01_AXI
    top_hls_hard_sydr_0 : IN_0           --> axi_dma_0          : M_AXIS_MM2S
    top_hls_hard_sydr_0 : ap_clk         --> axi_dma_0          : s_axi_lite_aclk
    top_hls_hard_sydr_0 : ap_rst_n       --> axi_dma_0          : axi_resetn
    top_hls_hard_sydr_0 : OUT_0          --> axi_dma_0          : M_AXIS_S2MM
    top_hls_hard_sydr_0 : interrupt      --> NOT CONNECTED
    ```
- Now validate the design, or press F6
- Click the block design from the source, and generate HDL wrapper
- From the left panel, click `Generate bitstream`
- When the bit stream is generated, export it to a location
- In the file system of the Vivado project, search for `design_1.hwh` file, copy this and rename it as same as the name of the bitstream
- Build finished, you can use the `.bit` and `.hwh` files to run your design at PYNQ environment.  


## Acknowledgement


- The work is conducted within the project APROPOS. This project has received funding from the European Union’s Horizon 2020 (H2020) Marie Sklodowska-Curie Innovative Training Networks H2020-MSCA-ITN-2020 call, under the Grant Agreement no 956090. Project link: https://apropos-project.eu/

- License: 
```
/*
 APROPOS - Organization for the EU Horizon 2020 APROPOS project, grant no. 956090.
 Copyright 2023 Antoine Grenier, Jie Lei, Hans Jakob Damsgaard

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
```


### Source code may contain work from:

- ``Sydr`` An open-source GNSS Software Defined Radio for algorithm benchmarking. https://github.com/aproposorg/sydr developed by Antoine Grenier.
- AMD Xilinx tutorial.
