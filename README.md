# PI-HPC-Bench 

This is the High Performance Computing Benchmark with Paramount Iterations. This benchmaks contains the main benchmarks found in the literature. However, we instrumented the applications code with Paramount Iterations. This benchmarks constais the follows applications:

* Applications from NAS Parallel Benchmarks (NPB)
* Applications from Exascale Proxy Applications (ECP Proxy Apps)
* Applications from LLNL ASC Proxy Apps
* Real applications (LAMMPS and ExaML)

Our Paramount Iterations library source code is in /utils/ path. Every benchmark directory has a /exec_scripts/run_exec.sh file: file for executing the applications of benchmark.

## CITATION


```
@article{2022,
title={PB3Opt: Profile‐based biased Bayesian optimization to select computing clusters on the cloud},
ISSN={1532-0634},
url={http://dx.doi.org/10.1002/cpe.7540},
DOI={10.1002/cpe.7540},
journal={Concurrency and Computation: Practice and Experience},
publisher={Wiley},
author={Camacho, Thais Aparecida Silva and Rosario, Vanderson Martins do and Napoli, Otávio Oliveira and Borin, Edson},
year={2022},
month={Dec}
}
```

## LICENSE

This project was developed at the Institute of Computing - Unicamp as part of [@thaisacs](https://github.com/thaisacs) master dissertation.
You are free to use this code under the [MIT LICENSE](https://choosealicense.com/licenses/mit/).
