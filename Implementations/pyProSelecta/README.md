# pyProSelecta

python bindings for ProSelectaCPP implemented with help from [cppyy](https://cppyy.readthedocs.io).

# Dependencies

This package requires `cppyy` to be available, you can install it in a variety of ways, see [here for instructions](https://cppyy.readthedocs.io/en/stable/installation.html). To build it from source, you can try:

```bash
STDCXX=17 MAKE_NPROCS=32 pip install --user --verbose cppyy --no-binary=cppyy-cling
```

This can take a while to run as it will build parts of ROOT/cling/llvm.