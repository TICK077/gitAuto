#ifndef GIT_H
#define GIT_H

int git_exists(void);
int git_is_repo(void);
int git_has_changes(void);

int git_pull(const char *remote, const char *branch);
int git_commit_all(const char *message);
int git_commit_auto(void);
int git_push(const char *remote, const char *branch);

#endif
