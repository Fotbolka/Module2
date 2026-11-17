#!/usr/bin/env bash
# Build all assignments using their own Makefiles. Stops at the first failure.
set -euo pipefail
cd "$(dirname "$0")"
for assignment in 2/2.1 2/2.2 2/2.3 3/3.1 3/3.2 4/4.1 4/4.2 4/4.3 6/6.1 6/6.2 6/6.3; do
    echo "== $assignment =="
    make -C "$assignment" clean
    make -C "$assignment"
done
echo 'Сборка всех 11 заданий завершена успешно.'
