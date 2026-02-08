// SPDX-License-Identifier: GPL-3.0
// Console stub for non-Windows builds.

#ifndef _WIN32
extern "C" int projt_console_stub(void)
{
    return 0;
}
#endif
