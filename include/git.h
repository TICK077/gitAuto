#ifndef GIT_H
#define GIT_H

/* 基础环境 */
int git_exists(void);

/* 仓库 */
int git_is_repo(void);
int git_init_repo(void);

/* remote */
int git_has_remote(const char *name);
int git_add_remote(const char *name, const char *url);

/* branch */
int git_get_current_branch(char *buf, int size);
int git_checkout_branch(const char *branch);
int git_create_branch(const char *branch);

/* 状态 */
int git_has_changes(void);

/* 同步 */
int git_pull(const char *remote, const char *branch);
int git_commit_all(const char *msg);
int git_push(const char *remote, const char *branch);

#endif /* GIT_H */
