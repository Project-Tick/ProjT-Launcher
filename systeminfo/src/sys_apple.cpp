#include "sys.h"

#include <sys/sysctl.h>
#include <sys/utsname.h>

#include <QDebug>
#include <QString>
#include <QStringList>

Sys::KernelInfo Sys::getKernelInfo()
{
    Sys::KernelInfo out;
    struct utsname buf;
    uname(&buf);
    out.kernelType = KernelType::Darwin;
    out.kernelName = buf.sysname;
    QString release = out.kernelVersion = buf.release;

    char cpu_brand[256];
    size_t cpu_brand_len = sizeof(cpu_brand);
    out.isCursed = false;
    if (sysctlbyname("machdep.cpu.brand_string", &cpu_brand, &cpu_brand_len, NULL, 0) == 0) {
        QString brand = QString::fromLatin1(cpu_brand);
        if (brand.contains("QEMU", Qt::CaseInsensitive) || brand.contains("Virtual", Qt::CaseInsensitive) ||
            brand.contains("VMware", Qt::CaseInsensitive)) {
            out.isCursed = true;
        }
    }

    out.kernelMajor = 0;
    out.kernelMinor = 0;
    out.kernelPatch = 0;
    auto sections = release.split('-');
    if (sections.size() >= 1) {
        auto versionParts = sections[0].split('.');
        if (versionParts.size() >= 3) {
            out.kernelMajor = versionParts[0].toInt();
            out.kernelMinor = versionParts[1].toInt();
            out.kernelPatch = versionParts[2].toInt();
        } else {
            qWarning() << "Not enough version numbers in " << sections[0] << " found " << versionParts.size();
        }
    } else {
        qWarning() << "Not enough '-' sections in " << release << " found " << sections.size();
    }
    return out;
}

uint64_t Sys::getSystemRam()
{
    uint64_t memsize;
    size_t memsizesize = sizeof(memsize);
    if (!sysctlbyname("hw.memsize", &memsize, &memsizesize, NULL, 0)) {
        return memsize;
    } else {
        return 0;
    }
}

Sys::DistributionInfo Sys::getDistributionInfo()
{
    DistributionInfo result;
    return result;
}
