# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/Project-Tick/ProjT-Launcher/blob/main/tomlplusplus/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

project(
	'tomlplusplus',
	'cpp',
	license: 'MIT',
	# Use three-component version (major.minor.patch) — tweak/info not allowed by Meson
	version: '0.0.4',
	meson_version: '>=0.61.0',
	default_options: [
		# https://mesonbuild.com/Builtin-options.html
		# core options
		'buildtype=release',
		'default_library=shared',
		# base options
		'b_lto=false',
		'b_ndebug=if-release',
		# compiler options
		'cpp_std=c++17'
	]
)

# Derive full package version (including tweak) from include/impl/version.hpp so
# we can expose a 4-component version string to packaging files while keeping
# `meson.project_version()` a valid X.Y.Z for shared library versioning.
python = find_program('python3', required: false)
version_full = meson.project_version()
if python.found()
	ver_file = meson.project_source_root() / 'include' / 'toml++' / 'impl' / 'version.hpp'
	    # Python one-liner: read file, regex major/minor/patch/tweak (tweak optional)
	    cmd = [
		python,
		'-c',
		'import re,sys; path=sys.argv[1]; text=open(path,"r",encoding="utf-8").read(); m=re.search(r"#\\s*define\\s+TOML_LIB_MAJOR\\s+([0-9]+)[^0-9].*#\\s*define\\s+TOML_LIB_MINOR\\s+([0-9]+)[^0-9].*#\\s*define\\s+TOML_LIB_PATCH\\s+([0-9]+)[^0-9].*(?:#\\s*define\\s+TOML_LIB_TWEAK\\s+([0-9]+)[^0-9])?", text, re.I|re.S); import sys as _s; print(_s.argv[2] if not m else (f"{int(m.group(1))}.{int(m.group(2))}.{int(m.group(3))}.{int(m.group(4))}" if m.group(4) else f"{int(m.group(1))}.{int(m.group(2))}.{int(m.group(3))}"))',
		str(ver_file), meson.project_version()
	    ]
	    res = run_command(cmd)
	if res.returncode() == 0
		version_full = res.stdout().strip()
	endif
endif


#-----------------------------------------------------------------------------------------------------------------------
# global vars + imports
#-----------------------------------------------------------------------------------------------------------------------

compiler = meson.get_compiler('cpp')
message('target cpu_family: @0@'.format(host_machine.cpu_family()))
message('target cpu: @0@'.format(host_machine.cpu()))
message('target system: @0@'.format(host_machine.system()))
message('target endian: @0@'.format(host_machine.endian()))

is_devel = get_option('devel')
is_debug = get_option('debug')
is_release = not is_debug
is_pedantic = get_option('pedantic') or is_devel
is_permissive = get_option('permissive')
is_windows = host_machine.system() == 'windows'
is_x64 = host_machine.cpu_family() == 'x86_64'
is_subproject = meson.is_subproject()

cpp = meson.get_compiler('cpp')
is_gcc = cpp.get_id() == 'gcc'
is_clang = cpp.get_id() == 'clang'
is_msvc = cpp.get_id() == 'msvc'
is_icc_cl = cpp.get_id() == 'intel-cl'
is_icc = is_icc_cl or cpp.get_id() == 'intel'
is_lld = cpp.get_linker_id() == 'ld.lld'
has_exceptions = get_option('cpp_eh') != 'none'
unreleased_features = get_option('unreleased_features')

build_tests = (get_option('build_tests') or is_devel) and not is_subproject
build_examples = (get_option('build_examples') or is_devel) and not is_subproject
build_tt = (get_option('build_tt') or is_devel) and not is_subproject and has_exceptions and not unreleased_features
build_lib = get_option('build_lib') or get_option('compile_library') or build_tests or build_examples or build_tt

#-----------------------------------------------------------------------------------------------------------------------
# global_args
#
# these are the arguments common to everything in the project
# *** they are not forwarded to dependents when using this as a submodule. ***
#-----------------------------------------------------------------------------------------------------------------------

global_args = cpp.get_supported_arguments(
	# clang/gcc
	'-ferror-limit=5',
	'-fmax-errors=5',
	'-Wno-unused-command-line-argument',
	'-Wno-reserved-macro-identifier',
	'-Wno-init-list-lifetime',
	'-fchar8_t',
	# msvc
	'/bigobj',
	'/Gy', # function-level linking
	'/GF', # string pooling
	'/openmp-',
	'/utf-8',
	'/volatile:iso',
	'/Zc:__cplusplus',
	'/Zc:inline',
	'/Zc:externConstexpr',
	'/Zc:preprocessor'
)
if has_exceptions
	global_args += cpp.get_supported_arguments('/Zc:throwingNew', '-D_HAS_EXCEPTIONS=1')
else
	global_args += cpp.get_supported_arguments('-D_HAS_EXCEPTIONS=0')
endif
if is_permissive
	global_args += cpp.get_supported_arguments('/permissive', '-DTOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA=1')
else
	global_args += cpp.get_supported_arguments('/permissive-')
endif
if is_pedantic
	global_args += cpp.get_supported_arguments(
		# clang
		'-Weverything',
		# gcc
		'-Wcast-align',
		'-Wcast-qual',
		'-Wctor-dtor-privacy',
		'-Wdisabled-optimization',
		'-Wfloat-equal',
		'-Wimport',
		'-Winit-self',
		'-Wlogical-op',
		'-Wmissing-declarations',
		'-Wmissing-field-initializers',
		'-Wmissing-format-attribute',
		'-Wmissing-include-dirs',
		'-Wmissing-noreturn',
		'-Wold-style-cast',
		'-Woverloaded-virtual',
		'-Wpacked',
		'-Wpointer-arith',
		'-Wredundant-decls',
		'-Wshadow',
		'-Wsign-conversion',
		'-Wsign-promo',
		'-Wstack-protector',
		'-Wstrict-null-sentinel',
		'-Wswitch-default',
		'-Wswitch-enum',
		'-Wundef',
		'-Wunreachable-code',
		'-Wunused',
		'-Wunused-parameter',
		'-Wuseless-cast',
		'-Wvariadic-macros',
		'-Wwrite-strings',
		'-Wmissing-noreturn'
	)
endif
# unnecessary pedantry:
global_args += cpp.get_supported_arguments(
	'-Wno-c++98-compat',
	'-Wno-c++98-compat-pedantic',
	'-Wno-documentation',
	'-Wno-documentation-unknown-command',
	'-Wno-switch-enum',
	'-Wno-covered-switch-default',
	'-Wno-padded',
	'-Wno-float-equal'
)
if get_option('time_trace')
	global_args += cpp.get_supported_arguments('-ftime-trace')
endif

#-----------------------------------------------------------------------------------------------------------------------
# global_link_args
#
# these are the linker arguments common to everything in the projectwhen compiling shared libraries and executables.
# *** they are not forwarded to dependents when using this as a submodule. ***
#-----------------------------------------------------------------------------------------------------------------------

global_link_args = []

if is_release
	global_link_args += cpp.get_supported_link_arguments(
		# msvc
		'/OPT:REF,ICF=3',
		'/INCREMENTAL:NO',
	)
endif

#-----------------------------------------------------------------------------------------------------------------------
# global_overrides
#
# these are the meson overrides common to everything in the project
# *** they are not forwarded to dependents when using this as a submodule. ***
#-----------------------------------------------------------------------------------------------------------------------

global_overrides = [ ]
if is_pedantic
	global_overrides += [
		'warning_level=3',
		'werror=true',
		]
endif

#-----------------------------------------------------------------------------------------------------------------------
# subdirectories + files
#-----------------------------------------------------------------------------------------------------------------------

public_headers = []
internal_headers = []

# Empty dependency that will be filled either in src/ or include/
tomlplusplus_dep = dependency('', required: false)

subdir('include')

if build_lib
	subdir('src')
endif

if build_tests
	subdir('tests')
endif

if build_examples
	subdir('examples')
endif

if build_tt
	subdir('toml-test')
endif

# Allow subproject usage
meson.override_dependency(meson.project_name(), tomlplusplus_dep)
