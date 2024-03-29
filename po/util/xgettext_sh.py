#!/usr/bin/python
# sh_xgettext
# Arnaldo Carvalho de Melo <acme@conectiva.com.br>
# Wed Mar 10 10:24:35 EST 1999
# Copyright Conectiva Consultoria e Desenvolvimento de Sistemas LTDA
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# Changelog
# Mon May 31 1999 Wanderlei Antonio Cavassin <cavassin@conectiva.com>
# * option --initscripts


from sys import argv
from string import find, split, strip

s = {}

def xgettext(arq, tokens_i18n):
	line = 0
	f = open(arq, "r")
        while 1:
		l = f.readline()
		if not l: break
		line = line + 1
		if l[0:1] == '#':       continue
		elif l[0:1] == '\n':    continue
		else:
			for token in tokens_i18n:
				pos = find(l, token + ' $"')
				if pos != -1:
					text = split(l[pos:], '"')[1]
					#if find (text, '$') != -1:
					#	continue
					if s.has_key(text):
						s[text].append((arq, line))
					else:
						s[text] = [(arq, line)]
	f.close()

def print_header():
	print 'msgid ""' 
	print 'msgstr ""' 
	print '"Project-Id-Version: \\n"'
	print '"PO-Revision-Date: YYYY-MM-DD HH:MM TZO DST\\n"'
	print '"Last-Translator: \\n"'
	print '"Language-Team: <XX@li.org>\\n"'
	print '"MIME-Version: 1.0\\n"'
	print '"Content-Type: text/plain; charset=ISO-8859-1\\n"'
	print '"Content-Transfer-Encoding: 8-bit\\n"\n'

def print_pot():
	print_header()

	for text in s.keys():
		print '#:',
		for p in s[text]:
			print '%s:%d' % p,
		if find(text, '%') != -1:
			print '\n#, c-format',
		print '\nmsgid "' + text + '"'
		print 'msgstr ""\n'
				
def main():
	i18n_tokens = []
	i18n_tokens.append('echo')
	i18n_tokens.append('echo -n')
	i18n_tokens.append('echo -e')
	i18n_tokens.append('echo -en')
	i18n_tokens.append('echo -ne')
	i18n_tokens.append('print_result 0')
	i18n_tokens.append('print_result 1')
	i18n_tokens.append('action')
	i18n_tokens.append('failure')
	i18n_tokens.append('passed')
	i18n_tokens.append('runcmd')
	i18n_tokens.append('success')
	i18n_tokens.append('/sbin/getkey -c $AUTOFSCK_TIMEOUT -m')

	for a in argv:
		xgettext(a, i18n_tokens)

	print_pot()

if __name__ == '__main__':
    main()
