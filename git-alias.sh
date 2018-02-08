#http://stackoverflow.com/questions/4479225/how-to-output-git-log-with-the-first-line-only
git config --global alias.pl "log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit"
git config --global alias.plx "log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit"
git config --global core.editor "vim"
git config --global alias.publish "push origin HEAD:refs/for/automation_jcat"
git config --global alias.draft "push origin HEAD:refs/drafts/automation_jcat"
git config --global alias.fshow "show --pretty=format: --name-only"
git config --global alias.uncommit "reset --soft HEAD^"
git config --global alias.co "checkout"
git config --global alias.pull "pull --rebase"
git config --global alias.remove-commit "reset --hard HEAD^"
git config --global alias.st "status"


