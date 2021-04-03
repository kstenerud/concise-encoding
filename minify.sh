#!/bin/sh

set -eux

# sudo npm install -g html-minifier
# sudo apt install yui-compressor

html-minifier --collapse-whitespace \
			  --remove-comments \
			  --remove-optional-tags \
			  --remove-redundant-attributes \
			  --remove-script-type-attributes \
			  --remove-tag-whitespace \
			  --use-short-doctype \
			  src/index.html >index.html

html-minifier --collapse-whitespace \
			  --remove-comments \
			  --remove-optional-tags \
			  --remove-redundant-attributes \
			  --remove-script-type-attributes \
			  --remove-tag-whitespace \
			  --use-short-doctype \
			  src/resources.html >resources.html

html-minifier --collapse-whitespace \
			  --remove-comments \
			  --remove-optional-tags \
			  --remove-redundant-attributes \
			  --remove-script-type-attributes \
			  --remove-tag-whitespace \
			  --use-short-doctype \
			  src/support-ce.html >support-ce.html

yui-compressor src/main.css >main.css
