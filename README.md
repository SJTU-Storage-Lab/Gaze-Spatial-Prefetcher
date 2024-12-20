# Gaze Artifact

This repository is used to evaluate Gaze, the first spatial prefetcher that utilizes footprint-internal temporal correlations to efficiently characterize spatial patterns. 

Gaze is proposed in the following paper:
>*Zixiao Chen, Chentao Wu, Yunfei Gu, Ranhao Jia, Jie Li, Minyi Guo*\
>"**Gaze into the Pattern: Characterizing Spatial Patterns with Internal Temporal Correlations for Hardware Prefetching**".\
>To appear in _31st IEEE International Symposium on High Performance Computer Architecture_ (HPCA 2025).

## Overview

Gaze is implemented using [ChampSim](https://github.com/ChampSim/ChampSim) simulator.
This repository provides all the necessary information to reproduce the main experiments presented in the paper. We describe how to prepare the required datasets, set up the system, and run the artifact. This artifact includes the source code of the ChampSim simulator used in our experiments and the scripts to build all binaries, run the experiments, and draw figures. 

A full run of this artifact takes about 7 days when using a compute cluster with 384 cores: key single-core results (1 day, fig. 1, 6-8, 11), multi-core results (4-5 days, fig. 14-15), supplementary single-core results (2-3 days, fig. 4, 9, 10, 12-13, 16-18).

### Tested Environment & Dependencies

- Ubuntu 18.04.6 LTS
- G++ 11.4.0
- Linux Kernel 5.4.0-150-generic
- Python 3.8.18
  - matplotlib 3.7.2
  - numpy 1.24.3
  - pandas 2.0.3
  - scipy 1.10.1


### Directory Structure

This repository is organized as follow:
- `./ChampSim`: ChampSim code
  - `./ChampSim/prefetcher/gaze`: Gaze code
- `./json` and `./log`: directories used to store log and json files.
- `./scripts`: directory that contains building, running and plotting scripts
  - `./scripts/make`: directory that contains building scripts.
  - `./scripts/run`: directory that contains running scripts.
  - `./scripts/dram`: directory that contains plotting scripts.
  - `./scripts/trace_check.py`: script used to check if the required traces are ready.
  - `./scripts/workloads.py`: util structures used by other scripts
- `./traces`: directory used place traces

## Preparing Traces

The traces used in our experiments are primarily from championships or previous work. Below is the detailed information about obtaining these traces. They should be manually downloaded. 

We have archived traces that are currently unavailable, or require conversion, or are generated by us:\
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.14252372.svg)](https://doi.org/10.5281/zenodo.14252372)
 
### Main Traces
- **SPEC06 and SPEC17 traces.** Available from DPC-3 (https://dpc3.compas.cs.stonybrook.edu/).
- **PARSEC and Ligra traces.** Originally from Pythia (not available now), we have archived a copy of our used traces.
- **CloudSuite traces.** Available from CRC-2 (https://crc2.ece.tamu.edu/).

### More Traces
- **GAP traces** (only used in supplementary experiments). Archived.
- **QMM traces** (only used in supplementary experiments). Archived. The original traces are also available, but they need to be converted manually. 


All traces should be placed directly in the trace directory `$GAZE_HOME/traces`.

Run the following command to check if the required traces are ready.

```bash
cd $GAZE_HOME/scripts   
python trace_check.py
```


## Installation

### Compiling ChampSim
First, ChampSim needs to be successfully built. The following are the build steps from `ChampSim/README.md`. For more detailed information, please refer to `ChampSim/README.md`.


Clone Gaze from GitHub repository
 
```bash
git clone https://github.com/SJTU-Storage-Lab/Gaze-Spatial-Prefetcher.git
```


Download ChampSim's dependencies
```bash
cd ChampSim
git clone https://github.com/microsoft/vcpkg.git
vcpkg/bootstrap-vcpkg.sh
vcpkg/vcpkg install
```  

### Compiling ChampSim with all evaluated prefetchers

Then, we can make all evaluated prefetchers.
```bash
cd ../scripts/make
python make_all.py
```

## Experimental Workflow
This section lists instructions to execute experiments. 
The running and plotting scripts are located in `scripts/run` and `scripts/draw`, respectively.
The simulation will generate log and json files that will be used for figure plotting.
Some eight-core heterogeneous simulations take more than four days to complete, therefore, please allocate dedicated CPU resources for multi-core heterogeneous simulations.
A single running script launches multiple experiments simultaneously. It can be modified to start only a subset of experiments. 
Before generating a figure, please ensure all corresponding experiments are completed.

Main single-core simulations (fig. 1, 6-8, 11)
```bash
cd $GAZE_HOME/scripts/run
python run_single_core_main.py
# wait for simulation
cd ../draw
python fig1.py # fig/fig1.pdf
```


Multi-core homogeneous simulations (fig. 14a)
```bash
python run_multi_core_homo.py
```


Multi-core heterogeneous simulations (fig. 14b, 15)
```bash
python run_multi_core_hete.py
```

Supplementary simulations (fig. 4, 9, 10, 12-13, 16-18)
```bash
python choose_the_script.py 
```

## Expected output 

The simulation does not directly output results. Instead, it writes the raw data to a json file, which is saved in `GAZE_HOME/json` directory. Meanwhile, runtime logs are stored in `GAZE_HOME/log`.
The plotting scripts convert the raw data into result data and generate the figures included in the paper.
Therefore, the generated figures should be consistent with those presented in the paper.
