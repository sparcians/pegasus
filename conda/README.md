# Conda environments

The conda environment created using yml file specification in this
directory can be used to build and run Atlas.

## Download and install miniconda

If `conda` is not already set up or the preference is to have a local
`conda` environment, consider installing [miniconda](https://docs.conda.io/en/latest/miniconda.html) first.

## Create a New Environment or Update Existing Conda Environment

A new environment can be created using `environment.yaml` file as follows:

```
conda env create -f environment.yml
```

This will create a conda environment named `riscv_func_model` in the default conda path.

If an environment named `riscv_func_model` exists, it can be updated as follows:

```
conda env update --file environment.yml  --prune
```
