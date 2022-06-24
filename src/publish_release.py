#!/usr/bin/env python3

# Requirements:
#   sudo apt install pandoc npm
#   sudo npm i -g puppeteer-cli

import atexit
import csv
import inspect
import os
import pathlib
import re
import string
import sys
import requests
import shutil
import subprocess
import uuid

SCRIPT_DIR = os.path.realpath(os.path.dirname(inspect.getsourcefile(lambda:0)))
TEMP_DIR = os.path.join("/tmp", "ce-" + str(uuid.uuid4()))

os.makedirs(TEMP_DIR)
@atexit.register
def cleanup():
    shutil.rmtree(TEMP_DIR)

def replace_multiple(string, replacements_dict):
    pattern_str = "|".join([re.escape(k) for k in sorted(replacements_dict, key=len, reverse=True)])
    pattern = re.compile(pattern_str, flags=re.DOTALL)
    return pattern.sub(lambda x: replacements_dict[x.group(0)], string)

def generate_temp_filename(extension=None):
    path = os.path.join(TEMP_DIR, str(uuid.uuid4()))
    if extension:
        return path + "." + extension
    return path

def ensure_path_exists(path):
    if not os.path.exists(path):
        os.makedirs(path)

def exec(args, check=True, run_path=None):
    cwd = None
    if run_path:
        cwd = os.getcwd()
        os.chdir(run_path)
    subprocess.run(args, check=check)
    if run_path:
        os.chdir(cwd)

def git_clone(src, dst, branch=None):
    args=[
        "git",
        "clone",
        "--depth", "1",
    ]
    if branch:
        args += ["--branch", branch]
    exec(args + [src, dst])
    return dst

def read_from_url(url):
    return requests.get(url).content

def read_string(file):
    with open(file) as f:
        return f.read()

def write_string(str, file):
    with open(file, 'w') as f:
        f.write(str)
    return file

def write_bin(bin, file):
    with open(file, 'wb') as f:
        f.write(bin)
    return file

def download_url(url, filename_suffix=None):
    dst = generate_temp_filename(filename_suffix)
    write_bin(read_from_url(url), dst)
    return dst

def is_url(path):
    return path.startswith("http://") or path.startswith("https://")

def prepare_path(path):
    if is_url(path):
        return download_url(path, pathlib.Path(path).suffix)
    else:
        return path

def build_md(src_file, dst_file, version):
    replacements = {
        '](ce-structure.md' : '](ce-structure-v' + version + '.md',
        '](cbe-specification.md' : '](cbe-specification-v' + version + '.md',
        '](cte-specification.md' : '](cte-specification-v' + version + '.md'
    }
    src_file = prepare_path(src_file)
    contents = replace_multiple(read_string(src_file), replacements)
    write_string(contents, dst_file)
    return dst_file


def generate_md(src, dst_base, version):
    src = prepare_path(src)
    dst = dst_base + ".md"
    if version == "":
        shutil.copyfile(src, dst)
    else:
        dst = dst_base + "-v" + version + ".md"
        build_md(src, dst, version)
    return dst

def generate_html(src, dst_base, version, title, search_path=[], fix_md_links=False):
    src = build_md(prepare_path(src), generate_temp_filename("md"), version)
    dst = dst_base + "-v" + version + ".html"
    if version == "":
        dst = dst_base + ".html"
    args = [
        "pandoc",
        "-f", "markdown",
        "-t", "html5",
        "--self-contained",
        "-c", "github-pandoc.css",
        "--metadata", 'title='+title,
        "--shift-heading-level-by=-1",
        "--resource-path", ':'.join(search_path),
        "-o", dst,
    ]
    if fix_md_links:
        args += ["--lua-filter", os.path.join(SCRIPT_DIR, "links-to-html.lua")]
    exec(args + [src])
    return dst

def generate_pdf(src, dst_base, version, title, search_path=[]):
    html_file = generate_html(src, generate_temp_filename(), version, title, search_path)
    dst_a4 = dst_base + "-v" + version + ".a4.pdf"
    dst_letter = dst_base + "-v" + version + ".letter.pdf"
    if version == "":
        dst_a4 = dst_base + ".a4.pdf"
        dst_letter = dst_base + ".letter.pdf"

    exec([
	    "puppeteer", "print",
		"--format=a4",
		"--margin-top=11mm",
		"--margin-bottom=11mm",
		"--margin-left=11mm",
		"--margin-right=11mm",
		html_file,
		dst_a4
    ])

    exec([
	    "puppeteer", "print",
		"--format=letter",
		"--margin-top=11mm",
		"--margin-bottom=11mm",
		"--margin-left=11mm",
		"--margin-right=11mm",
		html_file,
		dst_letter
    ])

    return dst_a4

def generate_release_list_index(dst_path):
    release_template = string.Template("""
        <article>
            <div class="resource">
                <a href="${release_url}"><img class="resource-img" src="../img/file.svg" alt="Specification"></a>
                <div class="resource-desc" style="display: block;">
                  <a href="${release_url}">Concise Encoding v${release_version}</a>
                  <br>
                  <span>Released: ${release_date}</span>
                </div>
            </div>
        </article>
""")

    releases = ""
    with open(os.path.join(SCRIPT_DIR, "releases.csv"), newline='') as f:
        reader = csv.reader(f)
        for row in reader:
            release_url = row[0] + "/index.html"
            release_version = row[0]
            release_date = row[1]
            releases += release_template.substitute(release_url=release_url, release_version=release_version, release_date=release_date)

    page_template = string.Template(read_string(os.path.join(SCRIPT_DIR, "release_list_index.html")))
    write_string(page_template.substitute(releases=releases), os.path.join(dst_path, "index.html"))

def generate_release_page_from_template(template_file, dst_file, version):
    page_template = string.Template(read_string(template_file))
    write_string(page_template.substitute(spec_version=version), dst_file)

def generate_release_index(dst_path, version):
    generate_release_page_from_template(os.path.join(SCRIPT_DIR, "per_release", "release_index.html"),
        os.path.join(dst_path, "index.html"), version)

def generate_spec(src_md, dst_dir, version, title):
    dst_base = os.path.join(dst_dir, os.path.splitext(os.path.basename(src_md))[0])
    search_path = [SCRIPT_DIR, os.path.dirname(src_md)]
    if is_url(src_md):
        search_path = [SCRIPT_DIR]

    generate_md(src_md, dst_base, version)
    generate_html(src_md, dst_base, version, title, search_path, fix_md_links=True)
    generate_pdf(src_md, dst_base, version, title, search_path)

def generate_release(version):
    releases_dir = os.path.realpath(os.path.join(SCRIPT_DIR, "..", "releases"))
    dst_dir = os.path.join(releases_dir, version)
    ensure_path_exists(dst_dir)

    generate_release_list_index(releases_dir)
    generate_release_index(dst_dir, version)
    generate_release_page_from_template(os.path.join(SCRIPT_DIR, "resources.html"),
        os.path.realpath(os.path.join(SCRIPT_DIR, "..", "resources.html")), version)
    generate_release_page_from_template(os.path.join(SCRIPT_DIR, "index.html"),
        os.path.realpath(os.path.join(SCRIPT_DIR, "..", "index.html")), version)

    # TODO: change branch=None to branch="v"+version
    ce_dir = git_clone("https://github.com/kstenerud/concise-encoding.git", generate_temp_filename(), branch=None)

    generate_spec(os.path.join(ce_dir, "ce-structure.md"), dst_dir, version, "Concise Encoding")
    generate_spec(os.path.join(ce_dir, "cbe-specification.md"), dst_dir, version, "Concise Binary Encoding")
    generate_spec(os.path.join(ce_dir, "cte-specification.md"), dst_dir, version, "Concise Text Encoding")
    generate_spec("https://raw.githubusercontent.com/kstenerud/compact-float/master/compact-float-specification.md", dst_dir, "", "Compact Float")
    generate_spec("https://raw.githubusercontent.com/kstenerud/compact-time/master/compact-time-specification.md", dst_dir, "", "Compact Time")

    print("Successfully generated release " + version)

##############################################################################

if len(sys.argv) != 2:
    print("Usage: " + sys.argv[0] + " <CE release version>")
    sys.exit()

generate_release(sys.argv[1])
