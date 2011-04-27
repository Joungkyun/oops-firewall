# $Id: config.sed,v 1.4 2009-04-30 13:33:37 oops Exp $

# removed comment
s/#.*\|"//g

:sumline
# Case that the last character of ther line is '/'
/\\[ \t]*$/ {
  # input next line to patern space.
  N

  # remove '/' character and newline
  s/[ \t]*\\[ \t]*[\r\n]\+[ \t]*/ /g

  # repeat Until '/' character don't exists.
  t sumline
}

# remove line of direction format
/^[^=]\+$/d

# remove blank line
/^$/d

# remove first white space each lines.
s/^[ \t]\+//g

# remove white space before or after equal mark
s/[ \t]*=[ \t]*/="/g

# close quote of variable's value
s/$/";/g

# print
p
