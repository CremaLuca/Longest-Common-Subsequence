# Parallel-algorithms-2021

## Getting started

The sequential version and the parallel one are in their respective folders `LCS_sequential` and `LCS_parallel`.

### Sequential

You can compile the source file using the `makefile` or by running `gcc main.c -OX -o main_oX.o` replacing `X` with 1, 2 and 3.

To execute the code on the CAPRI machine you can run `sbatch lcs_sequential.slurm` which will run the three optimization version on all of the input sizes in one job.

### Parallel

You can compile the source file using the `makefile` or by running `mpiicpc main.cpp utils.cpp -OX -o lcs_oX.o` on CAPRI after spack-loading intel parallel studio using `spack load intel-parallel-studio@professional.2019.4` and replacing `X` with 1 and 3.

To execute the code on the CAPRI machine you can run any slurm file in the `slurm` folder which are of type `lcs_oX_nP.slurm` where X is the optimization code and P is the number of processors between 1, 2, 4, 8, 16 and 32.

## Report

The printed verstion of the [report](report.md) pdf has been printed using StackEdit with the following CSS applied.

```css
body {
    font-size: 16px !important;
}
code, pre {
    font-size: 14px !important;
    line-height: 1.35;
    clear: both;
    page-break-before: always;
}
#preview-contents {
    font-size: 16px !important;
}
h1, h2, h3, h4, h5, h6 {
    margin: .5em 0 .5em 0 !important;
}
h1:after, h2:after{
    top: 0.25em;   
}
blockquote, dl, ol, p, pre, ul {
    margin: .4em 0;
}
.katex{
    font-size:1em;
}
.katex-display {
    margin-top: 1em;
}
.table-wrapper td, .table-wrapper th{
    padding: 4px 6px;   
}
.table-wrapper table{
    margin:auto;
    font-size: 14px !important;
}
```
