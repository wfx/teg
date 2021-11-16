Checklist for a new release
===========================

Preparing the release
---------------------

* Does it compile on the CI?
* Append all new contributors to /PEOPLE
* Fill out /CHANGELOG
  * use ``gitk »previous-release-tag« »current-release-branch«`` to review the
    changes since the last release
  * summary of the most important changes
  * reference issues,pull requests or git commit hashes when needed, especially
    for security related events

Doing the release
-----------------

Create an annotated tag in the form ``»num«.»num«.»num«``. Don't add any prefix
or suffix. The tag has to match the version of ``AC_INIT`` in ``/configure.ac``

After the release
-----------------

* Do a version bump in ``/configure.ac`` for the following bug fix release
* Merge the release branch to ``master`` (keep an eye on the Changelog and
  version, since they tend to get messed up during the merge)
