#!/usr/bin/env python3

import subprocess
import os
import sys
import shutil
import tempfile
from pathlib import Path

def compile_shader(slangc, input_file, target_api, profile, shader_stage, output_file, reflection_json=None):

	shader_stage = shader_stage.lower()
	if shader_stage == "vertex":
		entry = "VertexMain"
	elif shader_stage == "fragment":
		entry = "FragmentMain"
	else:
		print(f"Unsupported ShaderStage API: {shader_stage}")
		sys.exit(1)

	args = [
		slangc,
		input_file,
		"-target", target_api.lower(),
		"-stage", shader_stage,
		"-entry", entry,
		"-o", output_file,
	]

	if profile is not None:
		args.extend(["-profile", profile])

	# todo
	#if reflection_json is not None:
	#	args.extend(["-r", reflection_json])

	subprocess.run(args, check=True)

def embed_to_cpp_hpp(identifier, binary_path, text_path, out_cpp, out_hpp, tpl_cpp, tpl_hpp):

	def bytes_to_cpp_array(data: bytes, line_length=16) -> str:
		parts = [f'0x{b:02x}' for b in data]
		lines = []
		for i in range(0, len(parts), line_length):
			lines.append(', '.join(parts[i:i+line_length]))
		return ',\n\t'.join(lines)

	def make_cpp_identifier(identifier: str) -> str:
		base = os.path.basename(identifier)
		name = os.path.splitext(base)[0]
		return ''.join(c if c.isalnum() else '_' for c in name)

	def generate_from_template(output_path, template_path, varname, array_content, comment):
		with open(template_path, 'r') as tpl:
			template = tpl.read()
		output = (
			template
			.replace('@ESCAPED_FILE_NAME@', varname)
			.replace('@ESCAPED_FILE_CONTENTS@', array_content)
			.replace('@COMMENT@', comment)
		)
		with open(output_path, 'w') as f:
			f.write(output)

	with open(binary_path, 'rb') as f:
		data = f.read()

	comment = ''
	if os.path.exists(text_path):
		with open(text_path, 'r') as f:
			comment = f.read().strip()

	array_content = bytes_to_cpp_array(data)
	varname = make_cpp_identifier(identifier)

	generate_from_template(out_cpp, tpl_cpp, varname, array_content, comment)
	generate_from_template(out_hpp, tpl_hpp, varname, array_content, comment)

def main():
	if len(sys.argv) != 11:
		print("Usage: CompileReflectEmbedShader.py <slangc> <input> <target_api> <shader_stage> <config> <tpl_cpp> <tpl_hpp> <out_cpp> <out_hpp> <tmp_dir>")
		sys.exit(1)

	slangc, input_file, target_api, shader_stage, config, tpl_cpp, tpl_hpp, out_cpp, out_hpp, tmp_dir = sys.argv[1:]

	os.makedirs(tmp_dir, exist_ok=True)

	profile = None
	target_api = target_api.lower()
	if target_api == "vulkan":
		bytecode_ext = "spv"
		bytecode_target = "spirv"
		text_ext = "glsl"
		text_target = "glsl"
	elif target_api == "metal":
		bytecode_ext = "metallib"
		bytecode_target = "metallib"
		text_ext = "metal"
		text_target = "metal"
	elif target_api == "d3d12":
		bytecode_ext = "dxil"
		bytecode_target = "dxil"
		text_ext = "hlsl"
		text_target = "hlsl"
		if shader_stage.lower() == "vertex":
			profile = "vs_6_0"
		elif shader_stage.lower() == "fragment":
			profile = "ps_6_0"
		else:
			print(f"Unsupported stage: {shader_stage}")
			sys.exit(1)
	else:
		print(f"Unsupported target API: {target_api}")
		sys.exit(1)

	base_name = Path(input_file).stem
	binary_output = os.path.join(tmp_dir, f"{base_name}_{shader_stage}.{bytecode_ext}")
	text_output = os.path.join(tmp_dir, f"{base_name}_{shader_stage}.{text_ext}")
	reflection_output = os.path.join(tmp_dir, f"{base_name}_{shader_stage}.json")

	compile_shader(slangc, input_file, bytecode_target, profile, shader_stage.lower(), binary_output, reflection_output)
	if config == "Debug":
		compile_shader(slangc, input_file, text_target, profile, shader_stage.lower(), text_output)

	#todo embed reflection
	identifier = f"{base_name}_{shader_stage}"
	embed_to_cpp_hpp(identifier, binary_output, text_output, out_cpp, out_hpp, tpl_cpp, tpl_hpp)

if __name__ == "__main__":
	main()
