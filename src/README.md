Release Process
===============

## TODO:

* Update `publish_release.py` to grab the release tag instead of the main branch.
* Update `publish_release.py` to also put the current release table in resources.html


Versioning
----------

Major releases will contain only the major version number (e.g. `1`, `2`, etc).

Minor releases will use `major.minor` style versioning (e.g. `1.1`, `2.5`, etc),
and **MUST NOT** contain changes in functionality (corrections and clarifications _only_).

The git version tag will be prefixed with a `v` (e.g. `v1`, `v2.5`, etc).


Release Steps
-------------

1. Git tag the main branch with the new version (prefixed with a `v`) and push
2. Switch branch to gh-pages
3. Add new release to `releases.csv`
4. Run `publish_release.py <release version>` (**not** with a `v` prefix)
5. Verify locally in a browser
6. Check in result
