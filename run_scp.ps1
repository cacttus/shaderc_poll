#TODO: Implement Powershell script.
If( Test-Path "./bin/shaderc_poll-1.0.1_x86d"){
	Write-Host "Found glslc."
	If(Test-Path "../shaderc/glslc/Debug/glslc.exe"){
	  Write-Host "Found shaderpoll."
	 ./bin/shaderc_poll-1.0.1_x86d '../shaderc/glslc/Debug/glslc.exe' './test/test_input' './test/test_output' -warnings -ms=250 -recursive -cpd=on -colors -mt -force -- --target-env=vulkan1.2
	}
	Else{
		Write-Host "shaderc not found - Download/Build shaderc and place in ..\shaderc\glslc\Debug\"
	}
}
Else{
    Write-Host "shaderc_poll not found."
    Write-Host "https://github.com/google/shaderc#downloads"
}

