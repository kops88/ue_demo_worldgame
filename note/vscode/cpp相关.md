
## 需要插件2
c/C++  1.23.5 IntelliSenseEngine 设置Disable
clang
unreal clang  github下载
LLVM 

### 问题3
1. 无法找到LLVM(无法跳转到定义)
settings.json 1级括号添加 "clangd.path": "C:\\Program Files\\LLVM\\bin\\clangd.exe"
langd/bin 加到环境变量

launch.json配置如下
```
{
	"version": "0.2.0",
	"configurations": [
		{
			"name": "Launch Unreal Editor",
			"type": "cppvsdbg",
			"request": "launch",
			"program": "D:\\Epic\\UE_5.3\\Engine\\Binaries\\Win64\\UnrealEditor.exe",
			"args": [
				"g:\\ueprj\\ue_demo_worldgame\\worldgame.uproject"
			],
			"stopAtEntry": false,
			"cwd": "D:\\Epic\\UE_5.3\\Engine\\Binaries\\Win64",
			"environment": [],
			"console": "externalTerminal",
			"visualizerFile": "D:\\Epic\\UE_5.3\\Engine\\Extras\\VisualStudioDebugging\\Unreal.natvis"
		},
		{
			"name": "Launch Game",
			"type": "cppvsdbg",
			"request": "launch",
			"program": "D:\\Epic\\UE_5.3\\Engine\\Binaries\\Win64\\UnrealEditor.exe",
			"args": [
				"g:\\ueprj\\ue_demo_worldgame\\worldgame.uproject",
				"-game"
			],
			"stopAtEntry": false,
			"cwd": "D:\\Epic\\UE_5.3\\Engine\\Binaries\\Win64",
			"environment": [],
			"console": "externalTerminal",
			"visualizerFile": "D:\\Epic\\UE_5.3\\Engine\\Extras\\VisualStudioDebugging\\Unreal.natvis"
		}
	]
}
```
 