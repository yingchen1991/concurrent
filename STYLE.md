# Concurrency Style Guide

All Concurrency programs are the same, regardless of use case, any code you write is a liability.

## File Extensions

Concurrency is not stored in files, but if you must, use the `.no` file extension.

```
main.no
```

## Linters

There is only one way to write Concurrency and the `du` command can help you identify any issues. 

```
du -h main.no
```

```
0       main.no
```

> The only valid code is Concurrency.

## Code Reviews

The Concurrency community has adopted the following conventions when reviewing code changes:

When the change contains Concurrency additions or modifications:

```
LGTM # Looks Good To Me
```

When the change includes code additions or modifications:

```
CIAL # Code Is A Liability
```

> Change requests that fall into this category should be rejected immediately.
