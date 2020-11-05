
#Change this file to the correct output version.
FILE=./bin/shaderc_poll-1.0.1_x86d

if test -f "$FILE"; then
  ./bin/shaderc_poll-1.0.1_x86d '~/git/shader_poll/shaderc/bin/glslc' './test/test_input' './test/test_output' -errors -warnings -ms=250 -recursive -cpd=on -colors -mt -force -- --target-env=vulkan1.2
else 
  echo "./shaderc_poll executable not found."
fi

sleep 3s
