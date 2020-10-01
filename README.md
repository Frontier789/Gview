# Gview 
Research project center around supporting code comprehension via interactively visualizing parts of the Semantic Program Graph of Erlang projects. \
Multiple graph layouts are supported, such as layered tree or force-directed layout.

|   |   |
|:-:|:-:|
![](screenshots\mnesia_modules.PNG)|![](screenshots\tree.png)
Modules of Mnesia|Test tree view

## Compilation
Gview is built upon [flib](https://github.com/frontier789/flib), to compile the project, flib needs to be installed. To compile the project, simply call `make` in the root directory. To specify a path to flib, call `make FPATH=path/to/f`.

## Usage
Simply starting the application without arguments loads a test scene of a grid graph. To specify another test, the `--test` parameter can be used with `grid` or `tree`. To see further options, such as GPU acceleration or caching, call `gview -h`.

To use gview with RefactorErl, to let it access the Semantic Program Graph produced by it, gview has to be launched by RefactorErl. Please refer to the documentation of RefactorErl for that.

## Publications
During my studies at Eötvös Loránd University of Budapest in 2018 and 2019 the following articles were submitted under the wings of Melinda Tóth and István Bozó.
 - Journal paper accepted into STUDIA UNIV. BABES¸–BOLYAI, INFORMATICA Volume LX 2018 [pdf](https://frontier711.com/publications/MACS18.pdf)
 - Conference paper, winning 1st prize on first round of National Scientific Student Research Conference (TDK) 2018 [pdf](https://frontier711.com/publications/TDK18.pdf)
 - Conference paper accepted into Conference on Software Technology and Cyber Security 2019 [pdf](https://frontier711.com/publications/STCS19.pdf)
 - Conference paper submitted to te seventh IEEE Working Conference on Software Visualization (eventually not accepted) 2019 [pdf](https://frontier711.com/publications/VISSOFT19.pdf)
 - Conference paper, winning 1st prize on first round of National Scientific Student Research Conference (TDK) 2019 [pdf](https://frontier711.com/publications/TDK19.pdf)
 - My bachelor thesis, awarded summa cum laude 2019 [pdf](https://frontier711.com/publications/THESIS.pdf)

