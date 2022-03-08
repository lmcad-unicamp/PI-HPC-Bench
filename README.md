# PI-HPC-Bench 

PI-HPC-Bench is a collection of High Performance Computing (HPC) benchmarks instrumented with **Paramount Iterations (PI)**. It integrates widely used benchmark suites and real-world applications to evaluate HPC systems.

## Included Benchmarks
- **NAS Parallel Benchmarks (NPB)**
- **Exascale Proxy Applications (ECP Proxy Apps)**
- **LLNL ASC Proxy Apps**
- **Real Applications:** LAMMPS, ExaML

Our **Paramount Iterations (PI) library** source code is located in the `/utils/` directory. Each benchmark directory includes an execution script at `/exec_scripts/run_exec.sh`, which is used to run the benchmark applications.

## CITATION


```
@article{https://doi.org/10.1002/cpe.7540,
author = {Aparecida Silva Camacho, Thais and Martins do Rosario, Vanderson and Oliveira Napoli, Otávio and Borin, Edson},
title = {PB3Opt: Profile-based biased Bayesian optimization to select computing clusters on the cloud},
journal = {Concurrency and Computation: Practice and Experience},
volume = {35},
number = {18},
pages = {e7540},
keywords = {Bayesian optimization, cloud computing, high-performance computing},
doi = {https://doi.org/10.1002/cpe.7540},
url = {https://onlinelibrary.wiley.com/doi/abs/10.1002/cpe.7540},
eprint = {https://onlinelibrary.wiley.com/doi/pdf/10.1002/cpe.7540},
year = {2023}
}
```

## LICENSE

This project was developed at the Institute of Computing - Unicamp as part of [@thaisacs](https://github.com/thaisacs) master dissertation.
You are free to use this code under the [MIT LICENSE](https://choosealicense.com/licenses/mit/).
