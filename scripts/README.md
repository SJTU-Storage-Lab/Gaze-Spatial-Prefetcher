## Overview

This directory contains building (`./make`), running (`./run`) and plotting (`./draw`) scripts

## Compilation

The compilation does not take much time, so you can directly run:
```bash
cd make
python make_all.py
```

## Running Simulations
**Note:** Heterogeneous multi-core simulations take much longer, up to 4 days in some cases. Thus, please allocate dedicated CPU resources for the eight-core heterogeneous simulations.

Completing the full set of experiments takes approximately 7 days on a 384-core computing cluster.

The multi-core scripts exclude simulations that are duplicated in the single-core main experiment. For example, the single-core results required when plotting fig.14 are generated by the single-core main experiment, so they will not be re-executed in the multi-core scripts. The same applies to the supplementary experiments.

You can start with the main experiments first:  
```bash
cd run
python run_single_core_main.py  # main single-core simulations
python run_multi_core_hete.py   # heterogeneous multi-core simulations
python run_multi_core_homo.py   # homogeneous multi-core simulations
```

Supplementary simulations can be launched using the remaining run scripts.
```bash
python run_single_core_fig4.py  
python run_single_core_fig12.py  
python run_single_core_gaze_analysis.py  
python run_single_core_gaze_sensitivity.py  
python run_single_core_multi_level.py  
python run_single_core_system_sensitivity.py  
```

Each script launches all corresponding experiments at once, which may occupy excessive system resources. You can modify the script to run only a subset of them and execute the remaining ones later.

## Plotting Figures
After confirming that the relevant experiments have completed, you can use the following command to generate plots:  

```bash
cd draw
python fig1.py # fig/fig1.pdf
```

**Note** that the plotting code does not check whether the corresponding simulations have finished. You need to verify this through process status or the associated logs (`../log`).  

This is because, in a few multi-core simulations, ChampSim **actively** terminate due to invalid simulation states, which can result in invalid raw data. Thus, the plotting code is designed to handle invalid log and json files. *However, not all invalid files are generated from active termination; some are invalid because the simulations remain unfinished.*