#ifndef IGNORE_H
#define IGNORE_H

/* 初始化 .gitignore（如不存在） */
void ignore_init(void);

/* 确保某一条规则存在 */
void ignore_ensure_entry(const char *entry);

#endif /* IGNORE_H */
