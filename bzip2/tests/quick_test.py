#!/usr/bin/env python3
'''
basic compression/decompression tests.
'''

__copyright__ = 'Copyright (C) 2022 Micah Snyder'

from hashlib import md5
import os
from pathlib import Path

import testcase

TRY_VALGRIND = True

path_source = Path(os.getenv('PATH_SOURCE'))

def generate_test_function(*args):
    '''
    Generate a test with the given args.
    '''
    def foo(self):
        self.run_test(*args)
    return foo

class TC(testcase.TestCase):

    def tearDown(self):
        super().tearDown()
        self.verify_valgrind_log()

    def run_test(self, sample: Path, block_size: int = 0):
        '''
        Verify correct behavior when bzip2 tries to decompress the given file.

        Note: block_size is only used for the compression test.
        '''
        if sample.suffix == '.ref':
            # Try to decompress, with the expectation that it will fail, gracefully:
            # - First with default settings
            # - Second time using `--small` mode

            # Compress. We can keep the OG file.
            cmd = [str(TC.bzip2), '--compress', str(block_size), '--keep', '--stdout', str(sample)]
            (ec, out, err) = self.execute(cmd, try_valgrind=TRY_VALGRIND)

            # Check that bzip2 thinks it succeeded.
            assert ec == 0

            # Write it to a temp file
            tempfile_path = TC.path_tmp / (sample.name + '.bz2')
            print(f'Writing compressed {sample.name} file to disk as {tempfile_path.name}...')
            with tempfile_path.open('wb') as tmpfile:
                tmpfile.write(out)

            # Decompress the compresed tempfile. No point keeping it.
            cmd = [str(TC.bzip2), '--decompress', '--stdout', str(tempfile_path)]
            (ec, out, err) = self.execute(cmd, try_valgrind=TRY_VALGRIND)

            # Check that bzip2 thinks it succeeded.
            assert ec == 0

            # Calculate hash of decompressed file.
            out_hash = md5(out).hexdigest()

            # Verify that the MD5 of the original reference file matches MD5 of
            # the compressed & decompressed file.
            with sample.open('rb') as reffile:
                # Calcualte hash of reference file.
                refcontents = reffile.read()
                ref_hash = md5(refcontents).hexdigest()

                print(f'Checking that {tempfile_path.name} matches {sample.name} when decompressed...')
                assert out_hash == ref_hash, \
                    'decompression output and reference file differ:\n' + \
                    TC.hex_compare(out, refcontents)

        elif sample.suffix == '.bz2':
            # Try to decompress, with the expectation that it will succeed and
            # that the decompressed file's MD5 matches the reference file.
            #
            # Then, compress the result again, and decompress it again, verifying
            # once more that the (second) decompressed file's MD5 matches the reference file.

            # Verify that the MD5 of the original reference file matches MD5 of
            # the decompressed file.
            reffile_path = sample.parent / (sample.stem + '.ref')
            with reffile_path.open('rb') as reffile:
                # Calcualte hash of reference file.
                refcontents = reffile.read()
                ref_hash = md5(refcontents).hexdigest()

                # Decompress. We can keep the OG file.
                cmd = [str(TC.bzip2), '--decompress', '--keep', '--stdout', str(sample)]
                (ec, out, err) = self.execute(cmd, try_valgrind=TRY_VALGRIND)

                # Check that bzip2 thinks it succeeded.
                assert ec == 0

                # Calculate hash of decompressed file.
                out_hash = md5(out).hexdigest()

                # Verify that the decompressed file matches the MD5 of the original reference file.
                print(f'Checking that {sample.name} matches {reffile_path.name} when decompressed...')
                assert out_hash == ref_hash, \
                    'decompression output and reference file differ:\n' + \
                    TC.hex_compare(out, refcontents)


# loop through directories in 'bzip2/tests/input/quick'...
#
# For each of those directories, run a test on the files within that have
# the '.bad' or '.bz2' suffix.
testfiles_path = path_source / 'tests' / 'input' / 'quick'
if testfiles_path.is_dir:
    for sample in testfiles_path.glob('**/*'):

        if sample.suffix == '.ref':
            for block_size in [-1, -2, -3]:
                # Generate a test function for the sample.
                bug_validation_test = generate_test_function(sample, block_size)

                # Add the test function to the test class.
                setattr(TC, f'test_comp_decomp_{sample.name}_{abs(block_size)}', bug_validation_test)

        elif sample.suffix == '.bz2':
            # Generate a test function for the sample.
            bug_validation_test = generate_test_function(sample)

            # Add the test function to the test class.
            setattr(TC, f'test_decomp_only_{sample.name}', bug_validation_test)
