#
#	KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
#
#	Copyright (C) 2020-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
#
#	This file is part of MacCAN-KvaserCAN.
#
#	MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
#	it under the terms of the GNU Lesser General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	MacCAN-KvaserCAN is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General Public License
#	along with MacCAN-KvaserCAN.  If not, see <http://www.gnu.org/licenses/>.
#
all:
	@./build_no.sh
	@echo "\033[1mBuilding MacCAN-KvaserCAN...\033[0m"
	$(MAKE) -C Trial $@
	$(MAKE) -C Libraries/CANAPI $@
	$(MAKE) -C Libraries/KvaserCAN $@
	$(MAKE) -C Utilities/can_test $@
	$(MAKE) -C Utilities/can_moni $@

clean:
	$(MAKE) -C Trial $@
	$(MAKE) -C Libraries/CANAPI $@
	$(MAKE) -C Libraries/KvaserCAN $@
	$(MAKE) -C Utilities/can_test $@
	$(MAKE) -C Utilities/can_moni $@

pristine:
	$(MAKE) -C Trial $@
	$(MAKE) -C Libraries/CANAPI $@
	$(MAKE) -C Libraries/KvaserCAN $@
	$(MAKE) -C Utilities/can_test $@
	$(MAKE) -C Utilities/can_moni $@

install:
#	$(MAKE) -C Trial $@
	$(MAKE) -C Libraries/CANAPI $@
	$(MAKE) -C Libraries/KvaserCAN $@
#	$(MAKE) -C Utilities/can_test $@
#	$(MAKE) -C Utilities/can_moni $@

build_no:
	@./build_no.sh
	@cat Sources/build_no.h
