# Contributing

Learners are welcome to contribute a small example, clearer explanation, exercise
solution or reproducible correction.

## Fork, clone and branch

After publishing this folder as a GitHub repository, contributors can click
**Fork** on that repository. Copy the actual clone URL of your fork, then run:

```bash
git clone <your-fork-clone-url>
cd socket-programming-zero-to-advanced
git switch -c improve-udp-exercise
```

The angle-bracket URL is a placeholder to replace, not a claimed existing
repository address. This ZIP does not create a remote repository for you.

## Add an example

1. Pick the appropriate chapter and preserve its teaching sequence.
2. Use C11 with visible POSIX operations and checked return values.
3. Document descriptor ownership, framing, limits and error behavior.
4. Add a Makefile target and `programs.json` entry for a new executable.
5. Provide runnable commands, expected behavior, a walkthrough and exercises.
6. Add a focused integration check if you change protocol or concurrency behavior.

Shared helpers must remain small and explained. Do not silently introduce
packages, external services or traffic to public hosts into default tests.

## Improve documentation

Use relative links, fenced code blocks and readable Mermaid diagrams. Distinguish
observed output from illustrative output. State OS-specific assumptions. Keep
answers conceptually accurate even when simplifying the first example.

## Verify and submit

```bash
make -j2
make test check-docs
```

Commit a focused change, push your branch to your fork, then open a pull request
against the upstream repository. Explain the learning problem, changed behavior,
tests run and any remaining limitations. Screenshots are optional; terminal
transcripts are often better evidence.

## Report a bug

Include the chapter, exact commands, OS/compiler versions, expected and actual
results, error messages and a minimal input. Remove credentials and unrelated
personal data from logs. Questions about course material should name the first
step that becomes unclear.
