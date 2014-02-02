#!/bin/sh

a="150.1.1.10.in-addr.arpa domain name pointer CENTOS-X64-1.genesisdev.net."

echo "${a##*\ }" | cut -f1 -d.
