# PI-HPC-Bench 

This is the High Performance Computing Benchmark with Paramount Iterations. This benchmaks contains the main benchmarks found in the literature. However, we instrumented the applications code with Paramount Iterations. This benchmarks constais the follows applications:

* Applications from NAS Parallel Benchmarks (NPB)
* Applications from Exascale Proxy Applications (ECP Proxy Apps)
* Applications from LLNL ASC Proxy Apps
* Real applications (LAMMPS and ExaML)

Our Paramount Iterations library source code is in /utils/ path. Every benchmark directory have a /exec_scripts/run_exec.sh file. This file is responsible for executing the applications of benchmark in question.

## LICENSE

This project is being developed at the Institute of Computing - Unicamp as part of @thaisacs master dissertation.
You are free to use this code under the [MIT LICENSE](https://choosealicense.com/licenses/mit/).
