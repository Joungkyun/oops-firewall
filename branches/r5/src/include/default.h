# Default variables define
#
# $Id$
#

_ver=$(${c_cat} ${_includes}/version.h)
_gcol=40
_testmode=0
_verbose=0
_iprange=0
_noansi=0

export _ver _gcol _testmode _verbose _iprange _noansi

_tmpfile="/tmp/oops-firewall-tmp.$$"
_userport="1024:65535"
_allport="1:65535"

# network status
_nN="NEW"
_nE="ESTABLISHED"
_nR="RELATED"
_nI="INVALID"

_logformat="-m limit --limit 1/h --limit-burst 3 -j LOG"

# configration file name list
_configs="filter masq forward tos"

export _tmpfile _userport _nN _nE _nR _nI
export _logformat _configs
