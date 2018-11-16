
## Predictable Synchronisation Algorithms for Asynchronous Critical Sections




### Basic SVN usage

The command `svn -u status` gives you a list of both local and remote changes,
so you will know in advance if there have been any such change.

How to checkout a SVN repository made in LISHA:
```
svn checkout https://epos.lisha.ufsc.br/svn/makers/predictable_synchronisation_algorithms_for_asynchronous_critical_sections
...
while LISHA SVN server sends "ra_serf: The server sent a truncated HTTP response body"
    svn cleanup; svn update
printf "Congratulations, you cloned a made in LISHA SVN repository!"
```
1. https://stackoverflow.com/questions/15664132/subversion-detect-a-renamed-folder
1. https://stackoverflow.com/questions/19218445/tortoisesvn-checkout-failed-error-svn-e120106-ra-serf-the-server-sent-a-trunc


Creating a branch is faster while operating directly on the SVN server:
```sh
svn copy https://epos.lisha.ufsc.br/svn/makers/predictable_synchronisation_algorithms_for_asynchronous_critical_sections/epos2trunk \
           https://epos.lisha.ufsc.br/svn/makers/predictable_synchronisation_algorithms_for_asynchronous_critical_sections/epos2trunk/my-new-branch \
           -m "Creating a private branch of /my-new-branch."
svn update
svn switch https://epos.lisha.ufsc.br/svn/makers/predictable_synchronisation_algorithms_for_asynchronous_critical_sections/epos2trunk/my-new-branch
```
1. http://svnbook.red-bean.com/en/1.7/svn.branchmerge.using.html
1. https://tortoisesvn.net/docs/release/TortoiseSVN_en/tsvn-dug-rename.html
1. https://stackoverflow.com/questions/40783548/how-to-rename-a-branch-in-tortoise-svn-while-keeping-history
1. https://stackoverflow.com/questions/5102758/how-to-rename-an-svn-branch-and-update-references-in-an-existing-sandbox


To see the branch log until the branch creation:
```sh
svn log -l 10
svn log -l 10 -v
svn log -v --stop-on-copy
```
1. http://svnbook.red-bean.com/en/1.7/svn.tour.history.html
1. https://stackoverflow.com/questions/4881129/how-do-you-see-recent-svn-log-entries
1. https://stackoverflow.com/questions/4015412/how-to-find-the-common-ancestor-of-two-branches-in-svn


To create a directory:
```sh
svn mkdir https://epos.lisha.ufsc.br/svn/makers/predictable_synchronisation_algorithms_for_asynchronous_Closures/my-cool-directory -m "Making my-cool-directory"
```
1. https://stackoverflow.com/questions/345683/svn-creating-trunk-directory-on-existing-repository
1. https://serverfault.com/questions/115112/how-when-to-create-trunk-branches-and-tags-directories-in-a-new-subversion-repo/115164




