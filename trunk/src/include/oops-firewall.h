# header include file
#
# $Id$
#

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

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: set filetype=sh noet sw=4 ts=4 fdm=marker:
# vim<600: noet sw=4 ts=4:
#
