#!/usr/bin/env python3
import requests
from bs4 import BeautifulSoup
import re

URL = "https://sourceware.org/git/bzip2.git/log/?ofs=0"

HEADERS = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                  "(KHTML, like Gecko) Chrome/120.0 Safari/537.36",
    "Accept": "text/html",
}

r = requests.get(URL, headers=HEADERS, timeout=30)
r.raise_for_status()

soup = BeautifulSoup(r.text, "html.parser")

commits = []
seen = set()

for a in soup.select('a[href*="commit/?id="]'):
    href = a.get("href", "")
    commit = href.split("commit/?id=")[-1]
    if re.fullmatch(r"[0-9a-f]{7,40}", commit):
        if commit not in seen:
            commits.append(commit)
            seen.add(commit)

# newest → oldest geliyor
commits.reverse()

if not commits:
    raise RuntimeError("Commit bulunamadı — sourceware HTML hâlâ filtreliyor")

for c in commits:
    print(c)