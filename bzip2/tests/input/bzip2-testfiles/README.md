# BZ2 Test File Collection

This is a collection of "interesting" `.bz2` files that can be used to
test bzip2 works correctly. They come from different projects.

Each directory should contain:

- A README explaining where the test files originally came from.

- A LICENSE or a reference in the README to a license under which you may
  distribute the files.

- Files ending with `.bz2` extension.

  These are compressed files that should decompress correctly.
  Each `.bz2` file must come with a `.md5` file containing a hash of the
  original input file.

- Files ending with `.md5`.

  Each `.md5` file is used to check that bzip2 could correctly decompress the
  accompanying `.bz2` file.
  The original (non-compressed) files are deliberately not checked in.

- Files ending with a `.bz2.bad` extension.

  These are deliberately bad, and are used to see how bzip2 handles corrupt
  files. They are explicitly not intended to decompress correctly, but to catch
  errors in bzip2 trying to deal with bad data.

## Running the Tests

The the files here are intended to be tested using the "Large Test Suite"
integrated with BZip2's build system. For more details, see:
https://gitlab.com/bzip2/bzip2/-/blob/master/tests/README.md
and
https://gitlab.com/bzip2/bzip2/-/blob/master/COMPILING.md

## Adding Files to The Large Test Suite

> ***Read Me First***: If your new file demonstrates a bug that causes a crash,
> or buffer-overflow or something, you may have found a vulnerability!
>
> Vulnerabilities must be reported in confidence to the BZip2 maintainers.
> Please do not share such files publicly until the BZip2 maintainers have had
> adequate time to fix the issue - usually 60 to 90 days from day the issue was
> first reported.
>
> When reporting a vulnerability in a new issue, be sure to check the option
> "✅This issue is confidential" on the issue-creation page.

You may want to add a good or bad file to the test suite to demonstrate that a
fix to BZip2 resolves an issue when decompressing the given file. If you do,
please create a Merge Request to add the file here on the bzip2-testfiles
repository, and paste the link to the Merge Request in the bug report and/or
bug fix Merge Request at https://gitlab.com/bzip2/bzip2.

### Adding a Good `.bz2` Archive

If you want to add a good `.bz2` archive to the test suite as a regression test
for some fixed issue, you can generate the required `.md5` file like this:
```sh
md5sum < file > file.md5
```

This generates a `.md5` file that doesn't carry a file name (but just "-").
They can then be checked again with:
```sh
md5sum --check file.md5 < file
```

### Adding a Bad `.bz2` Archive

If you want to add a bad `.bz2` archive to the test suite, just add it and
make sure the extension ends with `.bz2.bad` correct.

## Credits

Thank you to Mark Wielaard for assembling these test files.

Thank you to the following for the flies found in this collection:
- the Apache Commons Compress project
- the DotNetZip Library project
- the Go Lang project
- the lbzip2 project
- the pyflate project
