#!/usr/bin/env python3
'''
Test correct decompression of good and bad .bz2 files found in:
https://gitlab.com/bzip2/bzip2-testfiles
'''

__copyright__ = 'Copyright (C) 2022 Micah Snyder'

from hashlib import md5
import os
from pathlib import Path

import testcase

TRY_VALGRIND = False # You can enable this if you want, but it will take a very
                     # long time to run, like a half hour or more.

path_source = Path(os.getenv('PATH_SOURCE'))

def generate_test_function(*args):
    '''
    Generate a test with the given args.
    '''
    def foo(self):
        self.run_test(*args)
    return foo

class TC(testcase.TestCase):

    def run_test(self, sample: Path, extra_arg: str = ''):
        '''
        Verify correct behavior when bzip2 tries to decompress the given file.
        '''
        if sample.suffix == '.bad':
            # Try to decompress, with the expectation that it will fail, gracefully:
            # - First with default settings
            # - Second time using `--small` mode

            # Decompress. We can keep the OG file.
            cmd = [str(TC.bzip2), '--decompress', '--keep', '--stdout', str(sample)]
            (ec, out, err) = self.execute(cmd, try_valgrind=TRY_VALGRIND)

            # Check that bzip2 failed gracefully.
            assert ec == 2 or ec == 1

            # Decompress small-mode. We can keep the OG file.
            cmd = [str(TC.bzip2), '--decompress', '--small', '--keep', '--stdout', str(sample)]
            (ec, out, err) = self.execute(cmd, try_valgrind=TRY_VALGRIND)

            # Check that bzip2 failed gracefully.
            assert ec == 2 or ec == 1

        elif sample.suffix == '.bz2':
            # Try to decompress, with the expectation that it will succeed and
            # that the decompressed file's MD5 matches the reference file.
            #
            # Then, compress the result again, and decompress it again, verifying
            # once more that the (second) decompressed file's MD5 matches the reference file.

            # Verify that the decompressed MD5 reference file DOES exist.
            with (sample.parent / (sample.stem + '.md5')).open('r') as md5_file:

                # Get the reference hash
                ref_hash = md5_file.read().split(' ')[0].strip()

                # Decompress. We can keep the OG file.
                cmd = [str(TC.bzip2), '--decompress', '--keep', '--stdout', str(sample)]
                if extra_arg != '':
                    cmd.append(extra_arg)
                (ec, out, err) = self.execute(cmd, try_valgrind=TRY_VALGRIND)

                # Check that bzip2 thinks it succeeded.
                assert ec == 0

                # Verify that the decompressed file matches the .md5 reference.
                out_hash = md5(out).hexdigest()
                assert out_hash == ref_hash

                # Write it to a temp file
                tempfile_path = TC.path_tmp / (sample.name + '.decompressed')
                print(f'Writing decompressed {sample.name} file to disk as {tempfile_path.name}...')
                with tempfile_path.open('wb') as tmpfile:
                    tmpfile.write(out)

                # Compress. No need to keep the temp file.
                cmd = [str(TC.bzip2), '--compress', '--stdout', str(tempfile_path)]
                if extra_arg != '':
                    cmd.append(extra_arg)
                (ec, out, err) = self.execute(cmd, try_valgrind=TRY_VALGRIND)

                # Check that bzip2 thinks it succeeded.
                assert ec == 0

                # Write it to a temp file
                tempfile_path = TC.path_tmp / (tempfile_path.name + '.bz2')
                print(f'Writing compressed {sample.name} file to disk as {tempfile_path.name}...')
                with tempfile_path.open('wb') as tmpfile:
                    tmpfile.write(out)

                # Decompress the compressed tempfile. No point keeping it.
                cmd = [str(TC.bzip2), '--decompress', '--stdout', str(tempfile_path)]
                if extra_arg != '':
                    cmd.append(extra_arg)
                (ec, out, err) = self.execute(cmd, try_valgrind=TRY_VALGRIND)

                # Check that bzip2 thinks it succeeded.
                assert ec == 0

                # Calculate hash of decompressed file.
                out_hash = md5(out).hexdigest()

                # Verify the decomp-comp-decomped file still matches the ref hash.
                assert out_hash == ref_hash


# loop through directories in 'bzip2/tests/input/bzip2-testfiles'...
#
# For each of those directories, run a test on the files within that have
# the '.bad' or '.bz2' suffix.
testfiles_path = path_source / 'tests' / 'input' / 'bzip2-testfiles'
if testfiles_path.is_dir:
    for sample in testfiles_path.glob('**/*'):
        if sample.suffix == '.bad' or sample.suffix == '.bz2':
            # Generate a test function for the sample.
            bug_validation_test = generate_test_function(sample)

            # Add the test function to the test class.
            setattr(TC, f'test_{sample.parent.name}_{sample.name}', bug_validation_test)

            # Generate a test function for the sample.
            bug_validation_test = generate_test_function(sample, '--small')

            # Add the test function to the test class.
            setattr(TC, f'test_{sample.parent.name}_{sample.name}_small', bug_validation_test)
