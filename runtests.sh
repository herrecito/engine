#!/usr/bin/env bash

set -e

for t in tests/*_test; do
    ./$t
done
