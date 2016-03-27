# MediaPipe
	MediaPipe is modular multimedia framework to deliver real-time streaming media with low latency with maximum scalability

# Add Dependency into source tree
	build system of MediaPipe is based on jconfigpy , which uses JSON as a configuration script, and GNU make. jconfigpy lets various distributed module be easily integrated into large project. to make your repository jconfigpy recognizable, you first put 'package.json' in your repository, which includes build description for your repository and package name and so on. and you have to add 'repo' attribute in config.json file in the directory your project will resides. 
	
	- package.json example
	{
		"name":"ENTER_YOUR_REPO_NAME",         /* put your project name */
		"include":["./include","./"],          /* put your header directory */
		"output":["libcdsl.a","libcdsl.so"],   /* output file name will be copied to 'dep' directory */
		"version":"0.1",                       /* version of your repo*/
		"buildcmd":[                           /* make command (will be executed by python) */
			"make config",                    
			"make release"
		]
	}
	
	- 'repo' attribute 
	"YOUR_REPO":{
	   "type" : "repo",
	   "name" : "ENTER_YOUR_REPO_NAME",
	   "url" : "ENTER_YOUR_REPO_URL",
	   "out" : "./dep/"
	}

	