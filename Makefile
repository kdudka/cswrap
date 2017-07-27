# Copyright (C) 2014 Red Hat, Inc.
#
# This file is part of cswrap.
#
# cswrap is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# cswrap is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cswrap.  If not, see <http://www.gnu.org/licenses/>.

CMAKE ?= cmake
CTEST ?= ctest

.PHONY: all check clean distclean distcheck install

# define $(space) as " " to bude used in $(subst ...)
space :=
space +=

# generate ;-sepearated $(CTEST_CMD)
CTEST_CMD = $(subst $(space),;,$(CTEST) --output-on-failure)

all:
	mkdir -p cswrap_build
	cd cswrap_build && $(CMAKE) -DCMAKE_CTEST_COMMAND="$(CTEST_CMD)" ..
	$(MAKE) -C cswrap_build

check: all
	cd cswrap_build && $(MAKE) check

clean:
	if test -e cswrap_build/Makefile; then $(MAKE) clean -C cswrap_build; fi

distclean:
	rm -rf cswrap_build

distcheck: distclean
	$(MAKE) check

install: all
	$(MAKE) -C cswrap_build install
