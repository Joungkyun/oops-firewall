# header include file
#
# $Id: oops-firewall.h,v 1.4 2011-01-13 12:55:07 oops Exp $
#

BRIDGE_NAME="brg0"

if [ -f "/etc/redhat-relase" ]; then
	export distribution="redhat"
elif [ -f "/etc/debian_version" ]; then
	export distribution="debian"
fi

source ${_includes}/color.h
source ${_includes}/print.h
source ${_includes}/check.h
source ${_includes}/command.h
source ${_includes}/default.h
source ${_includes}/device.h
source ${_includes}/modules.h
source ${_includes}/init.h
source ${_includes}/rule.h
source ${_includes}/masq.h
source ${_includes}/forward.h
source ${_includes}/bridge.h

