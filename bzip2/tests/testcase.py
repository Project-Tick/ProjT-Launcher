
'''
Wrapper for Python's unittest.TestCase that sets up BZip2 testing environment.
'''

__copyright__ = "Copyright (C) 2022 Micah Snyder"

from math import ceil
import os
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile
import unittest
from typing import Tuple, Union, NamedTuple

# Use older Python 3.5 syntax.
CmdResult = NamedTuple('CmdResult', [('ec', int), ('out', bytes), ('err', bytes)])

class TestCase(unittest.TestCase):

    version = ""

    path_source = None
    path_build = None
    path_tmp = None

    bzip2 = None

    valgrind = "" # Not 'None' because we'll use this variable even if valgrind not found.
    valgrind_args = []

    original_working_directory = ""

    @classmethod
    def setUpClass(cls):
        '''
        Prepare test environment:
        - Create a temporary testing directory.
        - Get paths needed for tests from environment variables.
        '''
        cls.operating_system = platform.platform().split("-")[0].lower()

        # The bzip2 program uses the BZIP and BZIP2 environment variables as
        # additional input. We must purge them to prevent OS environment
        # variables from affecting the test suite.
        os.environ.pop('BZIP', None)
        os.environ.pop('BZIP2', None)

        # Version may be used for testing bzip2 --version output, etc.
        cls.version = os.getenv("VERSION")
        if cls.version == None:
            raise Exception("VERSION environment variable not defined! Aborting...")

        # Get test paths from environment variables.
        cls.path_source = Path(os.getenv("PATH_SOURCE"))
        cls.path_build =  Path(os.getenv("PATH_BUILD"))
        cls.bzip2 =       Path(os.getenv("PATH_BZIP2")) if os.getenv("PATH_BZIP2") != None else None

        # Generate temp directory
        cls.path_tmp = Path(tempfile.mkdtemp(prefix=(cls.__name__ + "-"), dir=os.getenv("TMP")))

        # Enable valgrind testing if VALGRIND variable set to path of Valgrind executable.
        if os.getenv('VALGRIND') != None:
            valgrind = Path(os.getenv("VALGRIND"))

            if valgrind.is_file():
                cls.valgrind = valgrind

                logfile = cls.path_tmp / 'valgrind.log'
                cls.valgrind_args = [
                    '-v',
                    '--trace-children=yes',
                    '--track-fds=yes',
                    '--leak-check=full',
                    '--gen-suppressions=all',
                    '--show-leak-kinds=definite',
                    '--errors-for-leak-kinds=definite',
                    f'--log-file={logfile}',
                    '--error-exitcode=123',
                ]

        # Perform all tests with cwd set to the cls.path_tmp, created above.
        cls.original_working_directory = os.getcwd()
        os.chdir(cls.path_tmp)

    @classmethod
    def tearDownClass(cls):
        '''
        Clean up after ourselves,
        Delete the generated tmp directory.
        '''
        print("")

        # Restore current working directory before deleting cls.path_tmp.
        os.chdir(cls.original_working_directory)

        if None == os.getenv("KEEPTEMP"):
            try:
                shutil.rmtree(cls.path_tmp)
                print("Removed tmp directory: {}".format(cls.path_tmp))
            except Exception:
                print("No tmp directory to clean up.")

    def setUp(self):
        print('\n')

    def tearDown(self):
        print('\n')

    def execute(self, cmd: list, try_valgrind: bool = True) -> CmdResult:
        '''
        Execute a subprocess.Popen list of commands.
        Return a tuple of
        '''
        # Use valgrind if we have it.
        if try_valgrind and self.valgrind != '':
            cmd = [str(self.valgrind),] + self.valgrind_args + cmd

        print(f"Running: {' '.join(cmd)}\n")
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()

        # Check the valgrind log for errors.
        if try_valgrind and self.valgrind != '':
            self.verify_valgrind_log()

        return CmdResult(p.returncode, out, err)

    def verify_valgrind_log(self, log_file: Union[Path, None]=None):
        '''
        Check if valgrind log file contains errors.
        If valgrind not enabled this is basically a nop.
        '''
        if self.valgrind == "":
            return

        if log_file == None:
            log_file = self.path_tmp / 'valgrind.log'

        if not log_file.exists():
            raise AssertionError('{} not found. Valgrind failed to run?'.format(log_file))

        errors = False
        print('Verifying {}...'.format(log_file))
        try:
            with log_file.open('r') as the_log:
                assert 'ERROR SUMMARY: 0 errors' not in the_log
        except AssertionError:
            print("*" * 80)
            print('Valgrind test failed!'.center(80, ' '))
            print('Please submit a bug report with this log to https://gitlab.com/bzip2/bzip2/issues'.center(69, ' '))
            print(str(log_file).center(80, ' '))
            print("*" * 80)
            errors = True
        finally:
            with log_file.open('r') as log:
                found_summary = False
                for line in log.readlines():
                    if 'ERROR SUMMARY' in line:
                        found_summary = True
                    if (found_summary or errors) and len(line) < 500:
                        print(line.rstrip('\n'))
            if errors:
                raise AssertionError('Valgrind test FAILED!')

    @staticmethod
    def hex_compare(actual: bytes, expected: bytes, size: int = 16):
        '''
        Return string with hex comparison of two buffers
        '''
        a_lines = ceil(float(len(actual)) / float(size))
        e_lines = ceil(float(len(expected)) / float(size))
        lines = max(a_lines, e_lines)

        comparison = '          ' + \
                    f'output ({len(actual)}):'.ljust(size*2 + 3) + \
                    f'expected ({len(expected)}):\n'

        def render_slice(to_print, to_compare):
            line = ''
            for byte in range(0, size):
                if byte == size / 2:
                    line += ' '

                if byte < len(to_print):
                    if byte >= len(to_compare) or to_print[byte] != to_compare[byte]:
                        line += '\x1b[1;33m{:02x}\x1b[0m'.format(to_print[byte])  # bold yellow
                    else:
                        line += '{:02x}'.format(to_print[byte])                   # plain
                else:
                    line += '  '

            return line

        prev_is_dots = False
        for line in range(0, lines):
            a_line = actual[line * size : line * size + size]
            e_line = expected[line * size : line * size + size]

            if a_line == e_line:
                if prev_is_dots == False:
                    comparison += " ...\n"
                    prev_is_dots = True
            else:
                text_line = '{:8d}: {}  {}'.format(
                    line * size,
                    render_slice(a_line, e_line),
                    render_slice(e_line, a_line)
                )
                comparison += text_line + '\n'
                prev_is_dots = False

        return comparison + '\n'
