'''
usa8 CI helper: make a fresh checkout look "old" to Ninja so that a build
directory restored from actions/cache is reused incrementally.

Every file in the checkout (submodules included) gets a fixed old mtime,
then files touched by non-root commits of the main repository get the time
of the newest commit that touched them. Objects in the cached build dir were
produced after those commits, so only files changed since then (or whose
compile command changed) are rebuilt.

usage: python restore_mtime.py <repo root> [skip dir names...]
'''
import os, subprocess, sys

OLD = 1577836800  # 2020-01-01

root = os.path.abspath(sys.argv[1])
skip = set(['.git'] + sys.argv[2:])

count = 0
for dirpath, dirnames, filenames in os.walk(root):
    dirnames[:] = [d for d in dirnames if d not in skip]
    for name in filenames:
        path = os.path.join(dirpath, name)
        # follow_symlinks=False is not supported by os.utime on Windows.
        if os.path.islink(path):
            continue
        try:
            os.utime(path, (OLD, OLD))
            count += 1
        except OSError:
            pass
print('restore_mtime: %d files set to %d' % (count, OLD))

log = subprocess.run(
    ['git', '-C', root, 'log', '--format=@%ct %P', '--name-only', '--no-renames', 'HEAD'],
    capture_output=True, text=True, encoding='utf-8', check=True).stdout

seen = set()
stamp = None
updated = 0
for line in log.splitlines():
    if line.startswith('@'):
        parts = line[1:].split()
        # Root commit (no parents): keep the old time for its files.
        stamp = int(parts[0]) if len(parts) > 1 else None
        continue
    path = line.strip()
    if not path or path in seen:
        continue
    seen.add(path)
    if stamp is None:
        continue
    full = os.path.join(root, path)
    if os.path.isfile(full) and not os.path.islink(full):
        os.utime(full, (stamp, stamp))
        updated += 1
print('restore_mtime: %d files set to their last commit time' % updated)
