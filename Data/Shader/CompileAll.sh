GLSL_VALIDATOR=glslangValidator

for f in *.vert; do (echo $f & $GLSL_VALIDATOR -Od --target-env vulkan1.1 -o $f.spirv $f); done

for f in *.frag; do (echo $f & $GLSL_VALIDATOR -Od --target-env vulkan1.1 -o $f.spirv $f); done

