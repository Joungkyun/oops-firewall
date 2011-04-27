# Default variables define
#
# $Id: default.h,v 1.4 2007-03-30 08:56:52 oops Exp $
#

_ver=$(${c_cat} ${_includes}/version.h)
_gcol=50
_testmode=0
_verbose=0
_bridgeclear=0

export _ver _gcol _testmode _verbose _bridgeclear _ftppassive

_tmpifile="/tmp/oops-firewall-tmpi.$$"
_tmpofile="/tmp/oops-firewall-tmpo.$$"
_passiveif="/tmp/oops-firewall-passivei.$$"
_passiveof="/tmp/oops-firewall-passiveo.$$"
_userport="1024:65535"
_allport="1:65535"

# network status
_nN="NEW"
_nE="ESTABLISHED"
_nR="RELATED"
_nI="INVALID"

_logformat="-m limit --limit 1/h --limit-burst 3 -j LOG"

# configration file name list
_configs="interface application filter masq forward tos bridge"

# bridge support
BRIDGE_SET=0

export _tmpifile _tmpofile _passiveif _passiveof
export _userport _nN _nE _nR _nI
export _logformat _configs BRIDGE_SET

[ -f "${_tmpifile}" ]  && rm -rf "${_tmpifile}"  >& /dev/null
[ -f "${_tmpofile}" ]  && rm -rf "${_tmpofile}"  >& /dev/null
[ -f "${_passiveif}" ] && rm -rf "${_passiveif}" >& /dev/null
[ -f "${_passiveof}" ] && rm -rf "${_passiveof}" >& /dev/null
