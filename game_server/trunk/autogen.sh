#!/bin/bash
autoheader
aclocal
autoconf
automake --add-missing --include-deps --foreign
