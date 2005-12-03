# Print function
#
# $Id: print.h,v 1.1 2005-12-03 19:37:28 oops Exp $
#

printBanner() {
	[ "${1}" = "clear" ] && clear || echo
	echo  "############################################################################"
	echo $"# OOPS Firewall - Very Easy Iptables Frontend v${_ver}"
	echo $"#      By Sep 27 2005 JoungKyun.Kim <http://www.oops.org>"
	echo  "############################################################################"
	echo
}

usage() {
	printBanner

	echo $"Usage: oops-firewall -[option]"
	echo $"Options:"
	echo $"         -c config_directory    set configuration directory"
	echo $"         -t                     run test mode"
	echo $"         -v                     verbose mode"
	echo $"         -V                     print current version"
	echo $"         -h                     help (this) message"
	echo

	exit 1
}

printVersion() {
	echo -ne "OOPS Firewall v${_ver}\n\n"
	exit 0
}

# $1  => RESULT
# $2  => USER COMMENT
# $3  => START COLOR
# $4  => END COLOR
print_result() {
	__cend="7;0m"

	if [ "$1" = "0" ]; then
		__result="OK"
		conv_color blue
		__mcol=$?
	else
		__result="Fail"
		conv_color red
		__mcol=$?
	fi

	if [ -n "$3" ]; then
		conv_color $3
		__mcol=$?
	fi

	if [ -n "$2" ]; then
		__result="$2"
	fi

	__cstart="1;${__mcol}m"

	if [ $_verbose -eq 1 ]; then
		echo -ne "\\033[${_gcol}G"
		echo -n ": "
		echo -ne "[${__cstart}${__result}[${__cend}\n"
	fi
}

o_echo() {
	if [ $_verbose -eq 1 ]; then
		_opt=
		if [ "$1" = "-ne" -o "$1" = "-n" -o "$1" = "-e" ]; then
			_opt=$1
			shift
		fi
		echo $_opt "$*"
	fi
}

WordToUpper() {
	ORGTEXT=$1
	ORGVAR=$2

	toU=
	ORGTMP=$(echo $ORGTEXT | ${c_sed} 's/\(.\)/\1 /g')

	for i in $ORGTMP
	do
		convVar=
		CharToUpper ${i} convVar
		toU="${toU}${convVar}"
	done

	if [ -n "${ORGVAR}" ]; then
		eval "${ORGVAR}=\"${toU}\""
	fi
}

WordToLower() {
	ORGTEXT=$1
	ORGVAR=$2

	toL=
	ORGTMP=$(echo $ORGTEXT | ${c_sed} 's/\(.\)/\1 /g')

	for i in $ORGTMP
	do
		convVar=
		CharToLower ${i} convVar
		toL="${toL}${convVar}"
	done

	if [ -n "${ORGVAR}" ]; then
		eval "${ORGVAR}=\"${toL}\""
	fi
}

CharToUpper() {
	ORGCHAR=$1
	UPPERVAR=$2

	case $ORGCHAR in
		a) RETURNCHAR=A ;;
		b) RETURNCHAR=B ;;
		c) RETURNCHAR=C ;;
		d) RETURNCHAR=D ;;
		e) RETURNCHAR=E ;;
		f) RETURNCHAR=F ;;
		g) RETURNCHAR=G ;;
		h) RETURNCHAR=H ;;
		i) RETURNCHAR=I ;;
		j) RETURNCHAR=J ;;
		k) RETURNCHAR=K ;;
		l) RETURNCHAR=L ;;
		m) RETURNCHAR=M ;;
		n) RETURNCHAR=N ;;
		o) RETURNCHAR=O ;;
		p) RETURNCHAR=P ;;
		q) RETURNCHAR=Q ;;
		r) RETURNCHAR=R ;;
		s) RETURNCHAR=S ;;
		t) RETURNCHAR=T ;;
		u) RETURNCHAR=U ;;
		v) RETURNCHAR=V ;;
		w) RETURNCHAR=W ;;
		x) RETURNCHAR=X ;;
		y) RETURNCHAR=Y ;;
		z) RETURNCHAR=Z ;;
		*) RETURNCHAR=$ORGCHAR ;;
	esac

	if [ -n "$UPPERVAR" ]; then
		eval "${UPPERVAR}=\"${RETURNCHAR}\""
	fi
}

CharToLower() {
	ORGCHAR=$1
	LOWERVAR=$2

	case $ORGCHAR in
		A) RETURNCHAR=a ;;
		B) RETURNCHAR=b ;;
		C) RETURNCHAR=c ;;
		D) RETURNCHAR=d ;;
		E) RETURNCHAR=e ;;
		F) RETURNCHAR=f ;;
		G) RETURNCHAR=g ;;
		H) RETURNCHAR=h ;;
		I) RETURNCHAR=i ;;
		J) RETURNCHAR=j ;;
		K) RETURNCHAR=k ;;
		L) RETURNCHAR=l ;;
		M) RETURNCHAR=m ;;
		N) RETURNCHAR=n ;;
		O) RETURNCHAR=o ;;
		P) RETURNCHAR=p ;;
		Q) RETURNCHAR=q ;;
		R) RETURNCHAR=r ;;
		S) RETURNCHAR=s ;;
		T) RETURNCHAR=t ;;
		U) RETURNCHAR=u ;;
		V) RETURNCHAR=v ;;
		W) RETURNCHAR=w ;;
		X) RETURNCHAR=x ;;
		Y) RETURNCHAR=y ;;
		Z) RETURNCHAR=z ;;
		*) RETURNCHAR=$ORGCHAR ;;
	esac

	if [ -n "$LOWERVAR" ]; then
		eval "${LOWERVAR}=\"${RETURNCHAR}\""
	fi
}

parseValue() {
	pV=$1
	pN=$2
	rV=0 

	WordToLower ${pV} pV

	if [ -n "${pV}" ]; then
		case ${pV} in 
			true) rV=1 ;;
			yes)  rV=1 ;;
			1)    rV=1 ;;
			*)    rV=0 ;;
		esac
	fi

	if [ -n "${pN}" ]; then
		eval "${pN}=${rV}"
	fi

  return $rV
}
