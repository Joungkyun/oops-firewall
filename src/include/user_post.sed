# $Id: user_post.sed 308 2012-03-26 18:44:55Z oops $

:sumline
# Case that the last character of ther line is '/'
/\\[ \t]*$/ {
	# input next line to patern space.
	N

	# removed next line comment
	s/\([[:space:]]\+\)\?#.*\|"//g

	# remove '/' character and newline
	s/[ \t]*\\[ \t]*[\r\n]\+[ \t]*/ /g

	# repeat Until '/' character don't exists.
	t sumline
}

# remove first space each lines.
s/^[ \t]\+//g

# if iptables command is exists after separator,
# remove iptables command
s/@.*iptables[ \t]/@/g

# removed comment
s/[ \t]*#.*//g

# remove blank line
/^$/d

# print
/^@/p
