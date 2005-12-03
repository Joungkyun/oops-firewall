# Color function
#
# $Id: color.h,v 1.1 2005-12-03 19:37:28 oops Exp $
#

conv_color() {
	[ -z "${1}" ] && __input="green" || __input=${1}

	case "${__input}" in
		gray) __mcols=30 ;;
		bgray) __mcols=40 ;;
		red) __mcols=31 ;;
		bred) __mcols=41 ;;
		green) __mcols=32 ;;
		bgreen) __mcols=42 ;;
		yellow) __mcols=33 ;;
		byellow) __mcols=43 ;;
		blue) __mcols=34 ;;
		bblue) __mcols=44 ;;
		magenta) __mcols=35 ;;
		bmagenta) __mcols=45 ;;
		cyan) __mcols=36 ;;
		bcyan) __mcols=46 ;;
		white) __mcols=37 ;;
		bwhite) __mcols=47 ;;
		*) __mcols=32
	esac

	return ${__mcols}
}
