#!/bin/sh

set -eux

# sudo npm install -g html-minifier
# sudo apt install yui-compressor

minify() {
	local html=$1
	html-minifier --collapse-whitespace \
				  --remove-comments \
				  --remove-optional-tags \
				  --remove-redundant-attributes \
				  --remove-script-type-attributes \
				  --remove-tag-whitespace \
				  --use-short-doctype \
				  src/$html >$html
}

minify index.html
minify resources.html
minify support-ce.html
minify thankyou.html

yui-compressor src/main.css >main.css
