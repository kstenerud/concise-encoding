#!/usr/bin/env bash

set -eux

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <release version>"
    exit 1
fi

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-$0}"; )" &> /dev/null && pwd 2> /dev/null; )";
BASE_DIR="$( cd -- "$SCRIPT_DIR/.." &> /dev/null && pwd 2> /dev/null; )";

VERSION=$1

REPO_URL="https://github.com/kstenerud/concise-encoding.git"

RELEASES_DIR="$BASE_DIR/releases"
REPO_NAME="$(basename $REPO_URL)"
REPO_NAME="${REPO_NAME%.*}"
DEST_DIR="$RELEASES_DIR/$VERSION"
BUILD_DIR="$(mktemp -d)"
REPO_DIR="$BUILD_DIR/$REPO_NAME"


generate_markdown() {
	src_base="$1"
	dst_base="$2"
	cp "$src_base.md" "$dst_base.md"
}

generate_html() {
	src_base="$1"
	dst_base="$2"

	pandoc \
		-f markdown \
		-t html5 \
		--self-contained \
		-c github-pandoc.css \
		--lua-filter="$SCRIPT_DIR/links-to-html.lua" \
	    --resource-path="$REPO_DIR:$SCRIPT_DIR" \
	    -o "$dst_base.html" \
	    "$src_base.md"
}

generate_pdf() {
	# Unfortunately, this doesn't work because latex is a terrible language
	# that needs to die in a fire: https://texfaq.org/FAQ-toodeep
	src_base="$1"
	dst_base="$2"

	pandoc \
		-f markdown \
		-t pdf \
		--pdf-engine=xelatex \
	    --resource-path="$REPO_DIR:$SCRIPT_DIR" \
	    -o "$dst_base.pdf" \
	    "$src_base.md"
}

generate() {
	src_base="$REPO_DIR/$1"
	dst_base="$DEST_DIR/$1"

	generate_markdown "$src_base" "$dst_base"
	generate_html "$src_base" "$dst_base"
	# generate_pdf "$src_base" "$dst_base"
}

generate_index() {
	cat "$SCRIPT_DIR/release-index.md" | sed "s/INSERT_RELEASE_HERE/$VERSION/g" >"$BUILD_DIR/index.md"
	pandoc \
		-f markdown \
		-t html5 \
		--self-contained \
	    --resource-path="$SCRIPT_DIR" \
		-c github-pandoc.css \
	    -o "$DEST_DIR/index.html" \
	    "$BUILD_DIR/index.md"

	pandoc \
		-f markdown \
		-t html5 \
		--self-contained \
	    --resource-path="$SCRIPT_DIR" \
		-c github-pandoc.css \
	    -o "$RELEASES_DIR/index.html" \
	    "$SCRIPT_DIR/release-list-index.md"
}

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
rm -rf "$DEST_DIR"
mkdir -p "$DEST_DIR"
# TODO: Get by tag
git clone --depth 1 "$REPO_URL" "$REPO_DIR"

generate_index

generate cbe-specification
generate cte-specification
generate ce-structure

rm -rf "$BUILD_DIR"
