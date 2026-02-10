prog_python = import('python').find_installation('python3')

r = run_command(prog_python, '-m', 'pytest', '--help', check: false)
errortxt = r.stderr().strip()
if errortxt.contains('No module named pytest')
  error('pytest for python3 is required to run the tests. Please `pip3 install pytest` and try again.')
endif

valgrind = find_program('valgrind', required : false)

fs = import('fs')
env = environment()

environment = [
  'PYTHONTRACEMALLOC=1',
  'VERSION=' + meson.project_version(),
  'LD_LIBRARY_PATH=' + fs.parent(libbzip2.full_path()),
  'DYLD_LIBRARY_PATH=' + fs.parent(libbzip2.full_path()),
  'PATH_SOURCE=' + meson.project_source_root(),
  'PATH_BUILD=' + meson.project_build_root(),
  'PATH_TMP=' + meson.current_build_dir(),
  'PATH_LIBBZ2=' + libbzip2.full_path(),
  'PATH_BZIP2=' + bzip2.full_path(),
]

if valgrind.found() == true
  environment += 'VALGRIND=' + valgrind.full_path()
endif

test(
  'quick_test.py',
  prog_python,
  args : ['-m', 'pytest', '-vv', meson.current_source_dir() + '/quick_test.py'],
  env : environment,
  timeout : 500
)

if (fs.exists(meson.current_source_dir() + '/input/bzip2-testfiles/README.md'))
  test(
    'large_test.py',
    prog_python,
    args : ['-m', 'pytest', '-vv', meson.current_source_dir() + '/large_test.py'],
    env : environment,
    timeout : 10000
  )
endif
