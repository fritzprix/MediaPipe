# MediaPipe

## About 
>MediaPipe is modular multimedia framework to deliver real-time streaming media with low latency with maximum scalability


## How-To-Build
### Dependencies 
>- python / pip    
>- gnu toolchain (at least 4.9)   
>- git   

### Configure Project 
MediaPipe use jconfigpy as its build system combined with gnu make, git.
below shell command download jconfigpy package and execute config script of MediaPipe to download dependent library from git repository and command line dialog is prompted to get configuration options.
```
shell > make config   
```

build process is performed with below shell command after configuration is completed.
```
shell > make clean
shell > make all
```



## License
	BSD-2